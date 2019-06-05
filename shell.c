
//
//
// CS 361 Homework 2 Simple Shell
//
// Srinivas C Lingutla
// NetID: slingu2
// UIN: 655115444
//
//

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LENGTH 512

jmp_buf continueProcess;

int counter;

void signalhandler(int signal) {
	switch (signal) {

		case SIGUSR1: counter++; //increment the counter and continue
			 		  siglongjmp(continueProcess, 0);
					  break;
    case SIGTSTP:
            break;
    case SIGINT:
            break;
    case SIGSEGV:
            break;
		default:
            break;
	}
}

//Fork error wrapper
pid_t Fork()
{
  pid_t pid;
  if((pid = fork()) < 0){
    perror("fork error");
    exit(-2);
  }
  else
    return pid;
}


int main(int argc, char const *argv[]) {

	printf("This simple shell done by Srinivas C Lingutla, NetID: slingu2\n" );
	printf("--------------------------------------------------------------------\n" );

  int countermax = 3, return_status = 0;
  counter = 0;
  pid_t pid = 0;
  char * inputpronpt = "--> ";
  char cmds[LENGTH];
  char *commands[20]; //set the length of commands
  int i = 0;

	//setup the signal handlers
  if (signal(SIGUSR1, signalhandler) == SIG_ERR ||
			signal(SIGINT, signalhandler) == SIG_ERR  ||
			signal(SIGTSTP, signalhandler) == SIG_ERR ||
			signal(SIGSEGV, signalhandler) == SIG_ERR
		 )
				printf("\nError: Cannot catch the signal\n");


  // Read in the argument for the shell
  if(argc >= 2)
    i = atoi(argv[1]);

  if(i < 1) countermax = 6; //default precision counter up to 1000000
  else countermax = i;

  while (fputs(inputpronpt, stdout) > 0)
  {

    fgets(cmds, sizeof(cmds), stdin);

    i = 1;

    if(cmds[0]=='\n') continue; //user pressed enter

    commands[0] = strtok(cmds, " \t\r\n");

    //exit the program if the user typed in exit
    if(!strcmp(commands[0], "exit")) {
      printf("'exit' command read, program exiting....\n");
      exit(0);
    }

    while((commands[i] = strtok(NULL, " \t\r\n") ) != NULL) //get the arguments
    {
      i++;
    }

    pid = Fork(); //call the fork wrapper

    struct rusage rusage;

    if(pid == 0)
    {
      //Entering childprocess
      //printf("Entering child process\n" );
      //printf("%s, %s\n", commands[0], argv[0]);
      if ( execvp(commands[0], commands) < 0 ) {
          printf("Unable to process command\nError: %s\n", strerror(errno));
          exit(-1);
      }

    } // end of child process

    else //parent process
    {
        //printf("Entering parent process\n" );

        sigsetjmp( continueProcess, 1 );
        if (counter >= countermax){
          printf("KILLED SIGUSR2 because precision reached\n");
          kill(pid, SIGUSR2);
        }

        pid = wait4(pid, &return_status, WSTOPPED, &rusage);

        if ( WIFEXITED(return_status) == 1 ) {
            printf("\nChild %d terminated normally with the value %d\n", pid, WEXITSTATUS(return_status));
        }
        else {
            if ( WIFSIGNALED(return_status) ) {
                printf("\nChild %d exited by signal %d\n", pid, WTERMSIG(return_status));
            }
        }


        printf("Number of page faults occured: %ld\n", rusage.ru_majflt);
				printf("Number of SIGUSR1 signals: %d\n", counter);

    }
    printf("----------------------------------------\n" );
    counter = 0; //reset the counter for the next child

  }




  return 0;
}
