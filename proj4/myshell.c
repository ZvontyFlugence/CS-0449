// Z'vonty Flugence (zaf17)
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Gets command from stdin
void getInput(char* buffer) {
  fgets(buffer, 300, stdin);
}

// Tokenizes command into an array
void getTokens(char* buffer, char** tokens) {
  const char* delim = " \t\n";
  char* token = strtok(buffer, delim);
  int i;
  for(i = 0; token != NULL; i++) {
    tokens[i] = token;
    token = strtok(NULL, delim);
  }
  tokens[i] = NULL;
}

// Handles Writing output to a file
void handleWrite(char** tokens, int pos) {
  FILE* file = NULL;

  // write stdout to file
  file = freopen(tokens[pos+1], "w", stdout);
  // Checking if file failed to open
  if(file == NULL) {
    perror("Error");
    exit(1);
  }
  tokens[pos] = NULL;
}

// Handles Taking input from a file
void handleInput(char** tokens, int pos) {
  FILE* file = NULL;

  // read file to stdin
  file = freopen(tokens[pos+1], "r", stdin);
  // Checking if file failed to open
  if(file == NULL) {
    perror("Error");
    exit(1);
  }
  tokens[pos] = NULL;
}

// Attempts to run a non-builtin command
void run_cmd(char** tokens, char* buffer) {
  if(fork() == 0) {
    // child process
    // default signal handling
    signal(SIGINT, SIG_DFL);

    // IO Redirection
    int hasWritten = 0;
    int hasInputted = 0;    
    int size;
    for(size = 0; tokens[size] != NULL; size++) {
    }

    for(int i = size-1; i >= 0; i--) {
      if(strcmp(tokens[i], ">") == 0) {
        if(hasWritten == 0) {
          handleWrite(tokens, i);
          hasWritten = 1;
        } else {
          perror("Cannot redirect stdin more than once!\n");
          exit(1);
        }
      } else if(strcmp(tokens[i], "<") == 0) {
        if(hasInputted == 0) {
          handleInput(tokens, i);
          hasInputted = 1;
        } else {
          perror("Cannot redirect stdout more than once!\n");
          exit(1);
        }
      }
    }

    // Run shell command
    int status = execvp(tokens[0], &tokens[0]);
    // execvp error handling

    
    if(status == -1) {
      perror("Error");
      exit(1);
    }
    exit(0);
  } else {    
    // parent process
    int status;
    int childpid = waitpid(-1, &status, 0);

    // waitpid error handling
    if(childpid == -1) {
      perror("Error");
    }
    if(!WIFEXITED(status)) {
      if(WIFSIGNALED(status)) {
        printf("Terminated due to signal %s\n", strsignal(WTERMSIG(status)));
      }
    }
  }
}

// runs main shell loop
int main(int argc, char** argv) {
  // Ignore SIGINT (Ctrl+C)
  signal(SIGINT, SIG_IGN);

  while(1) {
    char buffer[300];
    printf("myshell> ");
    getInput(buffer);

    int num_tokens = strlen(buffer)/2;
    char* tokens[num_tokens];
    getTokens(buffer, tokens);

    // Handle Builtin Functions (exit, cd)
    if(tokens[0] != NULL) {
      if(strcmp(tokens[0], "exit") == 0) {
        if(tokens[1] != NULL) {
          int code = atoi(tokens[1]);
          exit(code);
        } else {
          exit(0);
        }
      } else if(strcmp(tokens[0], "cd") == 0) {
        if(tokens[1] != NULL) {
          chdir(tokens[1]);
        }
      } else {
        // Run non-builtin shell command
        run_cmd(tokens, buffer);
      }
    } else {
      continue;
    }
  }

  return 0;
}