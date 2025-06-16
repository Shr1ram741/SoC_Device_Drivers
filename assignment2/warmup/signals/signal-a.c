#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  pid_t pid=fork();
  if (pid < 0) {
    perror("Fork failed");
    return 1;
  }
  else if (pid==0) {
    while(1) {
      sleep(1);
    }
  }
  else {
    int status = kill(pid,SIGTERM);
    pid_t cpid = wait(NULL);
    printf("Child with PID %d terminated\n");
    exit(0);
  }
}
