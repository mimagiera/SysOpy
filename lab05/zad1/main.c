#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMDS 20
#define MAX_ARGS 20

char *read_file(char *file_name)
{
	FILE* file = fopen(file_name, "r");
	fseek(file, 0 , SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0 , SEEK_SET);
	char *file_buffer = malloc(file_size + 1);
	fread(file_buffer, 1, file_size, file);
    fclose(file);
	return file_buffer;
}
typedef struct {
	char *name;
	char **argv;
} cmd;

typedef struct {
	cmd *list;
	size_t size;
} cmds;
void print_commands(cmds commands)
{
    for(int i=0;i<commands.size;i++)
    {
        cmd cmd = commands.list[i];
        printf("%s\n",cmd.name);
    }
}
cmd parse_command(char* cmd_str)
{
	char *saveptr;
	cmd cmd = {};
	char **args = calloc(MAX_ARGS + 1, sizeof(char*));

	char *arg = strtok_r(cmd_str, " ", &saveptr);

	for (int i = 0; arg != NULL; i++, arg = strtok_r(NULL, " ", &saveptr)) {
		if (i == 0)
			cmd.name = arg;
		if (i < MAX_ARGS)
			args[i] = arg;
		else
			printf("Too many arguments in command");
	}

	cmd.argv = args;

	return cmd;
}
cmds parse_commands(char *commands)
{
	char *saveptr;
	cmd *list = calloc(MAX_CMDS + 1, sizeof(cmd));

	char *cmd_str = strtok_r(commands, "|\n", &saveptr);

	int i;
	for (i = 0; cmd_str != NULL; i++, cmd_str = strtok_r(NULL, "|\n", &saveptr)) {
		if (i < MAX_CMDS)
			list[i] = parse_command(cmd_str);
		else
			printf("Too many commands");
	}

	cmds cmds = {};
	cmds.list = list;
	cmds.size = i;

	return cmds;
}

void exec_all_functions(cmds commands)
{
	int pipe0[2];
    int pipe1[2];

	int i = 0;
	for (i = 0; i < commands.size; i++) 
    {
		cmd cmd = commands.list[i];

		if (i > 0) {
            if(i%2==0)
            {
                close(pipe0[0]);
                close(pipe0[1]);
            }
            else
            {
                close(pipe1[0]);
                close(pipe1[1]);
            }
        }

        if(i%2==0) pipe(pipe0);
        else pipe(pipe1);
		
		pid_t pid = fork();
		if (pid == 0) {
			if (i < commands.size - 1) {
                if(i%2==0)
                {
                    close(pipe0[0]);
                    dup2(pipe0[1],STDOUT_FILENO);
                }
                else
                {
                    close(pipe1[0]);
                    dup2(pipe1[1],STDOUT_FILENO);              
                }
                
            }
            if (i > 0) {
                if(i%2==0)
                {
                    close(pipe1[1]);
                    dup2(pipe1[0],STDIN_FILENO);
                }
                else
                {
                    close(pipe0[1]);
                    dup2(pipe0[0],STDIN_FILENO);
                }
                
            }
			execvp(cmd.name, cmd.argv);
		}
	}
    if(i%2==0)
    {
        close(pipe0[0]);
        close(pipe0[1]);
    }
    else
    {
        close(pipe1[0]);
        close(pipe1[1]);
    }
}


int main(int argc, char **argv)
{
    if(argc==2)
    {
        char *file_name=argv[1];
        char *command_string = read_file(file_name);
        cmds commands = parse_commands(command_string);
        exec_all_functions(commands);

        for (int i = 0; i < commands.size; i++)
            wait(NULL); 

        free(command_string);
    }
    return 0;
}