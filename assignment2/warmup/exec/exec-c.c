#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  printf("Just before exec system call");
  char *args[] = {"ls","-l",NULL};
  execvp("ls",args);
  printf("Just after exec system call");

  perror("Execvp failed");
  return 1;
}
