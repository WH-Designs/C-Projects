#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

const char* PIPE = "|";
const char* REDIRECT = ">";
const char* SPACE = " ";

const int READ = 0; //READ
const int WRITE = 1; //WRITE

struct Pipe
{
    int rw_pipe[2];
};

void init_pipes(const int num_commands, struct Pipe* pipes);
void close_pipes(const int num_commands, struct Pipe* pipes);
void execute(const char* command, const int num_args, int read_pipe[2], int write_pipe[2], struct Pipe* pipes);
void wait_for_all(const int num_commands);

size_t split(const char* delimiter, const char* command, char** commands);
void trim(char* str);

int main(int argc, char* argv[])
{
    char command[1024];
    char* commands[64];
    struct Pipe* pipes;

    printf("Wy Shell. Type in quit to exit.");
    printf("$ ");
    fgets(command, 1024, stdin);

    while(strcmp(command, "quit\n") != 0)
    {
        
        /* 
        My attempt at doing redirect implementation but couldn't figure it out 

        if(strchr(commands[num_commands - 1], REDIRECT))
        {
            size_t num_commands = split(PIPE, command, commands);
            size_t num_commands_redirect = split(REDIRECT, command, commands);

            pipes = malloc(sizeof(struct Pipe) * num_commands - 1);
            init_pipes(num_commands - 1, pipes);

            printf("You typed in: %s\n", command);
            
            size_t i;
            
            if(num_commands == 1)
            {
                execute(commands[0], num_commands, NULL, NULL, pipes);
            }
            else{
                for (i = 0; i < num_commands; ++i) 
                {
                    if (i == 0)
                    {
                        //first command
                        execute(commands[i], num_commands, NULL, pipes[i].rw_pipe, pipes);
                    }
                    else if (i == num_commands - 1)
                    {
                        //last command
                        execute(commands[i], num_commands, pipes[i - 1].rw_pipe, NULL, pipes);
                    }
                    else
                    {
                        //middle command
                        execute(commands[i], num_commands, pipes[i - 1].rw_pipe, pipes[i].rw_pipe, pipes);
                    }
                }

                close_pipes(num_commands - 1, pipes);

        }else{ 
        */
        size_t num_commands = split(PIPE, command, commands);

        pipes = malloc(sizeof(struct Pipe) * num_commands - 1);
        init_pipes(num_commands - 1, pipes);

        printf("You typed in: %s\n", command);
            
        size_t i;
            
        if(num_commands == 1)
        {
            execute(commands[0], num_commands, NULL, NULL, pipes);
        }
        else{
            for (i = 0; i < num_commands; ++i) 
            {
                if (i == 0)
                {
                    //first command
                    execute(commands[i], num_commands, NULL, pipes[i].rw_pipe, pipes);
                }
                else if (i == num_commands - 1)
                {
                    //last command
                    execute(commands[i], num_commands, pipes[i - 1].rw_pipe, NULL, pipes);
                }
                else
                {
                    //middle command
                    execute(commands[i], num_commands, pipes[i - 1].rw_pipe, pipes[i].rw_pipe, pipes);
                }
            }

            close_pipes(num_commands - 1, pipes);
        }
            
        wait_for_all(num_commands);
        free(pipes);
        printf("$ ");
        fgets(command, 1024, stdin);

    }
    return 0;
}

size_t split(const char* delimiter, const char* command, char** commands)
{
    char *token;
    char* command_copy = strdup(command);

    size_t num_commands = 0;
   
   /* get the first token */
   token = strtok(command_copy, delimiter);
   
   /* walk through other tokens */
   while( token != NULL ) {
      //printf( " %s\n", token );
      commands[num_commands] = malloc(sizeof(char) * strlen(token));
      trim(token);
      strcpy(commands[num_commands++], token);
    
      token = strtok(NULL, delimiter);
   }
   
    commands[num_commands] = NULL;

   return num_commands;
}

void trim(char* str)
{
    char *pstr = str;
    int len = strlen(str);

    while (isspace(pstr[len - 1]))
    {
        pstr[--len] = '\0';
    }

    while (*pstr && isspace(*pstr))
    {
        ++pstr;
        --len;
    }

    memmove(str, pstr, len + 1);
}

void execute(const char* command, const int num_commands, int read_pipe[2], int write_pipe[2], struct Pipe* pipes)
{
    if (strstr(command, REDIRECT))
    {
        char * command_with_redirect[64];
        size_t redirect_args = split(REDIRECT, command, command_with_redirect);
        char file_name[64];

        strcpy(file_name, command_with_redirect[redirect_args - 1]);

        command_with_redirect[redirect_args - 1] = NULL;

        pid_t pid;

        switch (pid = fork())
        {
            case -1:
                perror("Fork failed\n");
                break;
            case 0: //child
                if (read_pipe != NULL)
                {
                    if(dup2(read_pipe[READ], STDIN_FILENO) < 0)
                    {
                        perror("Failed to redirect STDIN\n");
                        exit(EXIT_FAILURE);
                    }
                }
                if (write_pipe != NULL)
                {
                    mode_t mode = O_WRONLY | O_CREAT | O_TRUNC;
                    
                    int fd;
                    fd = open(file_name, mode);

                    fchmod(fd, 0666);

                    if(dup2(write_pipe[READ], fd) < 0)
                    {
                        perror("Failed to redirect STDIN\n");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                }
                close_pipes(num_commands - 1, pipes);
                printf("%s", command_with_redirect[0]);
                execvp(command_with_redirect[0], command_with_redirect);
                perror("Error executing command\n");
                exit(EXIT_FAILURE);

                break;
        }
    }else
    {
        pid_t pid;
        char * command_with_args[64];
        size_t num_args = split(SPACE, command, command_with_args);

        switch (pid = fork())
        {
            case -1:
                perror("Fork failed\n");
                break;
            case 0: //child
                if (read_pipe != NULL)
                {
                    if(dup2(read_pipe[READ], STDIN_FILENO) < 0)
                    {
                        perror("Failed to redirect STDIN\n");
                        exit(EXIT_FAILURE);
                    }
                }

                if (write_pipe != NULL)
                {
                    if(dup2(write_pipe[WRITE], STDOUT_FILENO) < 0)
                    {
                        perror("Failed to redirect STDIN\n");
                        exit(EXIT_FAILURE);
                    }
                }

                close_pipes(num_commands - 1, pipes);
                execvp(command_with_args[0], command_with_args);
                perror("Error executing command\n");
                exit(EXIT_FAILURE);

                break;
    }

    
    }
}

void init_pipes(const int num_pipes, struct Pipe* pipes)
{
    size_t i;
    for (i = 0; i < num_pipes; ++i)
    {
        if (pipe(pipes[i].rw_pipe) < 0)
        {
            perror("Pipe initialization failed\n");
        }
    }
}

void close_pipes(const int num_pipes, struct Pipe* pipes)
{
    size_t i;
    for (i = 0; i < num_pipes; ++i)
    {
        if (close(pipes[i].rw_pipe[WRITE]) == -1 || pipes[i].rw_pipe[READ] == -1)
        {
            perror("Pipe initialization failed\n");
        }
    }
}
void wait_for_all(const int num_commands)
{
    size_t i;

    for (i = 0; i < num_commands; ++i)
    {
        wait(NULL);
    }
}