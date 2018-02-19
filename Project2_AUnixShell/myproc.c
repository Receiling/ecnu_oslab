#include "mysh.h"

//shell main process
void myproc(FILE* instream, int model)
{
    char Redir[2] = ">";
    char Backgd[2] = "&";
    char Pipe[2] = "|";
    char error_message[30] = "An error has occurred\n";
    int BackProc_cnt = 0;
    pid_t BackProc[MAXNBGPROC];
    int status;
	char* buffer = (char*)malloc(sizeof(char) * (MAXSIZE + 5));
	if(buffer == NULL)
	{
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(0);
	}
	char** parament = (char**)malloc(sizeof(char*) * (MAXPARAM + 2));
	if(parament == NULL)
	{
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(0);
	}
	while(TRUE)
	{
        if(model == 0)
        {
            write(STDOUT_FILENO, "mysh> ", 6);
        }
		//memset(buffer, 0, sizeof(buffer));
		char* res_fgets = fgets(buffer, MAXSIZE, instream);
		if(res_fgets == NULL)
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
			exit(0);
		}
		//write(STDOUT_FILENO, buffer, strlen(buffer));
		if(model)
		{
            write(STDOUT_FILENO, buffer, strlen(buffer));
		}
		if(strlen(buffer) > 512)
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
			continue;
		}
		if(strcmp(buffer, "\n") == 0) continue;

		//command pre_processing
		int param_cnt = 0;
		int tag = 1;
		char* pstart = buffer;
		while(*pstart != '\n')
		{
			if(*pstart == '&')
			{
				*pstart = 0;
				parament[param_cnt++] = Backgd;
				tag = 1;
			}
			else if(*pstart == '>')
			{
				*pstart = 0;
				parament[param_cnt++] = Redir;
				tag = 1;
			}
			else if(*pstart == '|')
			{
				*pstart = 0;
				parament[param_cnt++] = Pipe;
				tag = 1;
			}
			else if(*pstart == '\t' || *pstart == ' ')
			{
				*pstart = 0;
				tag = 1;
			}
			else if(tag == 1)
			{
				parament[param_cnt++] = pstart;
				if(param_cnt >= MAXPARAM) break;
				tag = 0;
			}
			pstart++;
		}
		*pstart = 0;
		if(param_cnt >= MAXPARAM)
		{
            write(STDERR_FILENO, error_message, strlen(error_message));
			continue;
		}
	   	parament[param_cnt] = NULL;
	   	if(param_cnt == 0) continue;

        //parse commond line
        if(strcmp(parament[0], "wait") == 0)
        {
            if(param_cnt > 1)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            for(; BackProc_cnt > 0; BackProc_cnt--)
            {
                waitpid(BackProc[BackProc_cnt - 1], &status, 0);
            }
            continue;
        }

        //Builtin_command
		if(builtin_command(parament, param_cnt))
			continue;

        //others command, backgrgound, redirection, pipe, common command
		command_info info = parse_command(parament, param_cnt);
		if(info.isValid == FALSE || (info.isPipe == TRUE && info.pipe_cmd_cnt <= 1))
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
			continue;
		}


        if(info.isPipe)
        {
            mypipe(info.pipe_cmd_cnt, info.pipe_cmd, info.outfile);
            continue;
        }

		pid_t chdpid;
		if((chdpid = fork()) < 0)
		{
            write(STDERR_FILENO, error_message, strlen(error_message));
            continue;
		}
		if(chdpid >  0)
		{
			if(info.isBackgd == FALSE)
			{
				waitpid(chdpid, &status, 0);
			}
			else
			{
                BackProc[BackProc_cnt++] = chdpid;
                continue;
			}
		}
		else
		{
			//这是一个最大的问题，因为进程fork的原因导致文件偏移量发生了改变，导致文件除了第一行以外都要重复读入一次
			//因此如果在父进程如果open了一些文件，子进程不用时要注意将这些文件关闭，防止对父进程的文件读入造成干扰
			fclose(instream);
			if(info.isRedir)
			{
				if(info.outfile == NULL)
				{
					write(STDERR_FILENO, error_message, strlen(error_message));
					exit(0);
				}
				else
				{
					int out_fd = open(info.outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
					if(out_fd == -1)
					{
						write(STDERR_FILENO, error_message, strlen(error_message));
						exit(0);
					}
					close(fileno(stdout));
					dup2(out_fd, fileno(stdout));
					close(out_fd);
				}
			}
			if(execvp(parament[0], parament) == -1)
			{
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
			}
			break;
		}
	}
	free(buffer);
	free(parament);
}
