#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Incorrect number of arguments\n");
    return 1;
  }

  pid_t pid=fork();
  if (pid<0) {
    perror("Fork failed\n");
    return 1;
  }
  else if (pid==0) {
    execvp(argv[1],&argv[1]);
    
    perror("Execvp failed\n");
    return 1;
  }
  else {
    pid_t cpid = wait(NULL);
    printf("Command executed successfully\n");
    exit(0);
  }
  return 0;
}
