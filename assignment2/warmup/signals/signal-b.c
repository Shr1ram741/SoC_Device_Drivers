#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void signal_handler(int signum) {
  if (signum == SIGINT) {
    printf("I will run forever\n");
  }
}

int main(int argc, char *argv[])
{
  if (signal(SIGINT,signal_handler) == SIG_ERR) {
    printf("Couldn't catch SIGINT\n");
  }

  while(1) {
    sleep(10);
  }
  return 0;
}
