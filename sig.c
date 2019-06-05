
//
//
// CS 361 Homework 2 Signal Handler
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

static unsigned long int attempts = 0;
static unsigned long int faults = 0;
unsigned long int report = 10;
jmp_buf continueProcess;

void setupSignals();
void printCurrentResults();

void signalhandler(int signal)
{
  setupSignals();

  switch (signal) {

      case SIGSEGV:
              attempts++;
              faults++;
              siglongjmp( continueProcess, 0 );
              break;

      case SIGINT:
              printCurrentResults();
              printf("\nExiting Monte Carlo Simulation...\n");
              exit(SIGINT);
              break;

      case SIGUSR2:
              printCurrentResults();
              exit(SIGUSR2);
              break;

      case SIGTSTP:
              printCurrentResults();
              printf("\nContinuing simulation...\n");
              printf("-----------------------------\n");
              siglongjmp( continueProcess, 0 );
              break;

      case SIGALRM:
              printCurrentResults();
              printf("\nAlarm set off...\n");
              exit(SIGALRM);
              break;

      default: break;
  }
}

//setup the signalhandler with all the required signals
void setupSignals() {

	if (signal(SIGSEGV, signalhandler) == SIG_ERR)
  printf("\nError: Cannot catch the SIGSEGV\n");

  if (signal(SIGUSR2, signalhandler) == SIG_ERR)
  printf("\nError: Cannot catch the SIGUSR2\n");

	if (signal(SIGTSTP, signalhandler) == SIG_ERR)
  printf("\nError: Cannot catch the SIGTSTP\n");

	if (signal(SIGALRM, signalhandler) == SIG_ERR)
  printf("\nError: Cannot catch the SIGALRM\n");

  if (signal(SIGINT, signalhandler) == SIG_ERR)
  printf("\nError: Cannot catch the SIGINT\n");
}

void printCurrentResults() {
	float percentage = ( (float) faults / (float) attempts) * 100;

	printf("Monte Carlo Simulation Results\n");
	printf("   attempts: %lu\n", attempts);
	printf("   misses: %lu\n", faults);
	printf("   percentage: %02f\n", percentage);
}

int main(int argc, char const *argv[]) {

  printf("\nThis signal handler done by Srinivas C Lingutla, NetID: slingu2\n" );
  printf("--------------------------------------------------------------------\n" );

  setupSignals();


  //Get the timer for the carlo
  if (argc >= 2) {
    float timer = atof(argv[1]);
    if (timer > 0.0)
      alarm((int)timer); //set an alarm for that time
  }

  srand(time( NULL )); //to get the random address


	for (attempts = 0; attempts < ULONG_MAX; attempts++) {

    sigsetjmp( continueProcess, 1 );

    // Notify parent that power of ten has been reached
    if ( report == attempts){
      report = report * 10;
      kill(getppid(), SIGUSR1);
    }

		// Exit if maximum attempts have been reached
		if (attempts >= ULONG_MAX) {
      printCurrentResults();
      exit(0);
    }

    unsigned long int *randomNumber = (uintptr_t)(rand());
    uintptr_t * address = randomNumber;
    int randomAdress = * address;

	}

  //exit the program with the percentage
  printCurrentResults();
  float percentage = ((float)faults / (float)attempts)*100;
  int percent = (int) percentage;
  exit(percent);
  return 0;
}
