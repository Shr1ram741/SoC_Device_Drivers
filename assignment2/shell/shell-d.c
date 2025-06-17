#include  <stdio.h>
#include  <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_PROCESSES 100
pid_t processes[MAX_PROCESSES];
int proc_count=0;

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void add_process(pid_t pid) {
  if (proc_count<MAX_PROCESSES) {
    processes[proc_count++]=pid;
  }
}

void terminate_processes(void) {
  for (int i=0;i<proc_count;i++) {
    if (kill(processes[i],0)==0) {
      if (kill(processes[i],SIGKILL)!=0) {
        perror("Kill failed");
      }
    }
  }
  proc_count=0;
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
    char cwd[256];
    getcwd(cwd,sizeof(cwd));
		printf("%s $ ",cwd);
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */
    if (line[0]=='\0') {continue;}

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them
    if (strcmp(tokens[0],"cd")==0) {
        if (chdir(tokens[1]) != 0) {
          perror("chdir failed");
        }
    }
    else if (strcmp(tokens[0],"exit")==0) {
        terminate_processes();
        exit(0);
    }

    int status;
    pid_t pid = fork();
    if (pid<0) {
      perror("Fork failed");
    }
    else if (pid==0) {
      if (strcmp(tokens[0],"cd")!=0) {
        execvp(tokens[0],tokens);

        perror("Exec failed");
        exit(1);
      }
    }
    else if (pid>0) {
      add_process(pid);
      wait(&status);

      if (WIFEXITED(status)) {
        int c_status = WEXITSTATUS(status);
        printf("EXITSTATUS: %d\n",c_status); 
      }
    }

		//for(i=0;tokens[i]!=NULL;i++){
		//	printf("found token %s (remove this debug output later)\n", tokens[i]);
		//}
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
