#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#define MAX_ARGS 1024 

int numargs(char* line, char* args[]);
void controlpipes(char* args[], int nargs);
int check_redirection(char* args[], int *nargs, char** output_file);
void execute(char* line);

int numargs(char* line, char* args[]){
  int i = 0;

  if((args[0] = strtok(line, "\n\t ")) == NULL)
    return 0;
  while((args[++i] = strtok(NULL, "\n\t ")) != NULL) {
     if(i >= MAX_ARGS) {
     printf("Too many arguments!\n");
     exit(1);
    }
 }
 return i;
 }

void controlpipes(char* args[], int nargs)
{

   int num_cmds = 0;
   char *cmd[256];
   int file[2];
   int i = 0;
   
   for(int i = 0; i < nargs; i++){
       if (strcmp(args[i], "|") == 0){
          num_cmds++;
       }
   }
   if(num_cmds == 0){
      return;
   }
   int j = 0;
   int end = 0;
   pid_t pid;
while(args[j] != NULL && end != 1)
{
            int k = 0;
 
        while (strcmp(args[j],"|") != 0)
         {
            cmd[k] = args[j];
            j++;
            if (args[j] == NULL)
             {
                end = 1;
                k++;
                break;
             }
            else
            {
               k++;
            }
         }
         cmd[k] = NULL;
          j++;

     if (i==0){
         pipe(file);
     }
     pid=fork();

     if(pid==-1){
         if (i == 0){
                close(file[1]); 
            }
        
        printf("Child process could not be created\n");
        return;
    }
    if(pid==0)
    {
       
        if (i == 0)
        {
            dup2(file[1],1);
        }
  
        else 
        {
           
            dup2(file[0],0);
        }
        execvp(cmd[0],cmd);
    }
    if (i == 0){
        close(file[1]);
    }
    else{
            close(file[0]);
    }

    waitpid(pid,NULL,0);

    i++;
  }
}



 int check_redirection(char* args[], int *nargs, char** output_file){
 
 //printf("searching for redirections\n");
 //printf("Completed successfully!\n");
 for(int i = 0; i < *nargs; i++){
    
    if (strcmp(args[i], ">>") == 0){
       *output_file = args[i+1];
        int j;
        for(j = i; j < *nargs; j++){
        if(j < *nargs){
                args[j] = args[j+2];
            }
        }
        *nargs = *nargs-2;
        i = 0;
    }

  }
 }



 void execute(char* line){
 int pid;
 char* args[MAX_ARGS];
  char *output_file;
 int nargs = numargs(line, args);
 controlpipes(args, nargs);
 check_redirection(args, &nargs, &output_file);
 if(nargs <= 0) return;
 if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) {
        exit(0);
 }

 pid = fork();
 if(pid == 0) {
  
     int output;
     output = open(output_file, O_CREAT | O_WRONLY, 0666);
     dup2(output, 1);
     close(output);
     //printf("Completed successfully!\n");
     execvp(args[0], args);

  perror("exec failed");
  exit(-1);
  } 
  else if(pid > 0) { 
      waitpid(pid, NULL, 0);
     }  
 else { 
           perror("fork failed");
           exit(1);
 } 
  
 }
 int main (int argc, char* argv [])
 {
 char line[BUFSIZ];

  while(argc<2) {
  printf("$ ");
  if(fgets(line, BUFSIZ, stdin) == NULL) {
    perror("fgets failed");
    exit(1);
  }
  execute(line);
 
 }
     execvp(argv[1],&argv[1]);
 return 0;
 }
