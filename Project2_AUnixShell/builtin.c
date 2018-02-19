#include "mysh.h"

//built_in command, like cd, pwd, wait, exit
int builtin_command(char* parament[], int param_cnt)
{
    char error_message[30] = "An error has occurred\n";
	if(strcmp(parament[0], "exit") == 0)
	{
		exit(0);
	}
	else if(strcmp(parament[0], "cd") == 0)
	{
        if(param_cnt > 2)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
		if(parament[1] == NULL)
		{
			parament[1] = getenv("HOME");
		}
		char* cd_path = (char*)malloc(sizeof(char) * (strlen(parament[1]) + 2));
		if(cd_path == NULL)
		{
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
		}
		strcpy(cd_path, parament[1]);
		if(chdir(cd_path) != 0)
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
			exit(0);
		}
		free(cd_path);
		return 1;
	}
	else if(strcmp(parament[0], "pwd") == 0)
	{
        if(param_cnt > 1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
		char* path = NULL;
		path = getcwd(NULL, 0);
		write(STDOUT_FILENO, path, strlen(path));
		write(STDOUT_FILENO, "\n", 1);
		free(path);
		return 1;
	}
	return 0;
}

