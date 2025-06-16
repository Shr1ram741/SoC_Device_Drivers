#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

void child(void) {
  printf("Child's PID: %d\n",getpid());
  exit(0);
};

int main(void)
{
  int n;
  scanf("%d",&n);
  for (int i=0;i<n;i++) {
    pid_t pid;
    pid=fork();
    if (pid < 0) {
      perror("Fork failed");
      exit(1);
    }
    if (pid==0) {
      child();
    }      
  }
  return 0;
}
