#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<fcntl.h>

void shell();
char** tokenize(char* inputString);
void handleCommand(char** args);

int numArgs;//number of args in the input
int background;//when an & is at the end this will be 1
char userInput[100];
FILE* fp = NULL;
int status;


void shell()
{
    printf("Prompt> ");
    fgets(userInput, sizeof(userInput), stdin);
    char** tokens = tokenize(userInput);
    handleCommand(tokens);
}


void handleCommand(char** args)
{
    args[numArgs-1][strlen(args[numArgs-1])-1] = '\0';
    args[numArgs] = NULL;
    if(strcmp(args[numArgs-1], "&") == 0)
    {
        background = 1;
        args[numArgs-1] = NULL;
        numArgs--;
    }
    else
    {
        background = 0;
    }
    while(strcmp(args[0], "quit") != 0)
    {
        int out = 0;
        int in = 0;
        int j;
        char* outputFileName;
        char* inputFileName;
        int lessThanPos;
   
        for(j = 0 ; j < numArgs; j++)
        {
            if(strcmp(args[j], "<") == 0)
            {
                in = 1;
                inputFileName = args[j+1];
                break;
            }
        }
        for(j = 0 ; j < numArgs; j++)
        {
            //printf("-----------\n");
            //printf("%s\n",args[j]);
            
            if(strcmp(args[j], ">") == 0)
            {
                lessThanPos = j;
                out = 1;
                //printf("hi%shi\n",args[j]);
                args[j] = NULL;
                outputFileName = args[j+1];
                //break;
            }
        }
        if(in)
        {
            if(strcmp(args[0], "sort") == 0)
            {
                args[1] = inputFileName;
                args[2] = NULL;
            }
            else{
                args[lessThanPos] = NULL;
            }
        }
        int file_desc;
        int saved;
        if(out)
        {
            if (remove(outputFileName) == 0)
            {
                
            }
            fp = fopen(outputFileName, "a");
            file_desc = open(outputFileName, O_APPEND|O_WRONLY);
            saved = dup(1);
            dup2(file_desc, 1);
        }
        pid_t pid = fork();
        if (pid < 0) {
            exit(1);
        }
        else if(pid == 0){
            execvp(args[0], args);
            exit(0);
        }
        else{
            if(background == 1)
            {
                background = 0;
                signal(SIGCHLD,SIG_IGN);
                //waitpid(-1,&status,WNOHANG);
            }
            else{
                wait(NULL);
            }
        }
        if(out)
        {
            sleep(2);
            dup2(saved, 1);
            fclose(fp);
            close(saved);
            
            out = 0;
        }
        if(in)
        {
            in = 0;
        }

        printf("Prompt> ");
        background = 2;
        fgets(userInput, sizeof(userInput), stdin);
        args = tokenize(userInput);
        args[numArgs-1][strlen(args[numArgs-1])-1] = '\0';
        args[numArgs] = NULL;
        if(strcmp(args[numArgs-1], "&") == 0)
        {
            background = 1;
            args[numArgs-1] = NULL;
            numArgs--;
        }
        else
        {
            background = 0;
        }
    }
}

//returns an array of tokens
char** tokenize(char* inputString)
{
    char** args = malloc(100 * sizeof(char*));
    int i;
    for (i =0 ; i < 20; i++)
    {
        args[i] = malloc(100 * sizeof(char));
    }
    char* token = strtok(inputString, " ");
    int j = 0;
    while (token) {
        args[j] = token;
        token = strtok(NULL, " ");
        j++;
    }
    numArgs = j;
    return args;
}

int main()
{
    shell();
    return 0;
}
