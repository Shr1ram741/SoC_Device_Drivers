#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  execvp(argv[1],&argv[1]);

  perror("Execvp failed");
  return 1;
}
