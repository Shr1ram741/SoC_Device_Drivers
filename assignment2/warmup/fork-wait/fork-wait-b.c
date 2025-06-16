#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void child(void) {
  printf("I am child\n");
  exit(0);
};

void parent(void) {
  pid_t cpid=wait(NULL);
  printf("I am parent\n");
  exit(0);
};

int main(int argc, char *argv[])
{
  pid_t pid;
  pid=fork();
  if (pid < 0) {
    perror("Fork failed\n");
    exit(1);
  }
  if (pid==0) {
    child();
  }
  else {
    parent();
  }
  return 0;
}
