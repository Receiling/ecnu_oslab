#include "mysh.h"

int main(int argc, char* argv[])
{
    char error_message[30] = "An error has occurred\n";
	if(argc > 1)
	{
		FILE *fp = fopen(argv[1], "r");
		if(fp == NULL)
		{
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
		}
		myproc(fp, 1);
		fclose(fp);
	}
	else myproc(stdin, 0);
	return 0;
}
