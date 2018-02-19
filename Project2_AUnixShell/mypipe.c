#include "mysh.h"

void mypipe(int pipe_cmd_cnt, char** pipe_cmd[], char* outfile)
{
    char error_message[30] = "An error has occurred\n";
    int pipe_fd[MAXPIPECOMMAND][2];
    pid_t chdpid;
    int i = 0, j = 0;
    for(i = 0; i < pipe_cmd_cnt - 1; ++i)
    {
        if(pipe(pipe_fd[i]) < 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
    }
    int index = 0;
    for(index = 0; index < pipe_cmd_cnt; ++index)
    {
        if((chdpid = fork()) < 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
        if(chdpid == 0) break;
    }

    if(chdpid == 0)
    {
        if(index == 0)
        {
            close(pipe_fd[index][0]);
            close(fileno(stdout));
            dup2(pipe_fd[index][1], fileno(stdout));
            close(pipe_fd[index][1]);

            for(j = 1; j < pipe_cmd_cnt - 1; ++j)
            {
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }
        }
        else if(index == pipe_cmd_cnt - 1)
        {
            close(fileno(stdin));
            dup2(pipe_fd[index - 1][0], fileno(stdin));
            close(pipe_fd[index - 1][0]);

            //pipe contains redirection
            if(outfile != NULL)
            {
                int out_fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                if(out_fd == -1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
                close(fileno(stdout));
                dup2(out_fd, fileno(stdout));
                close(out_fd);
            }

            for(j = 0; j < pipe_cmd_cnt - 1; ++j)
            {
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }
        }
        else
        {
            close(fileno(stdin));
            dup2(pipe_fd[index - 1][0], fileno(stdin));
            close(pipe_fd[index - 1][0]);

            close(pipe_fd[index][0]);
            close(fileno(stdout));
            dup2(pipe_fd[index][1], fileno(stdout));
            close(pipe_fd[index][1]);

            for(j = 0; j < pipe_cmd_cnt - 1; ++j)
            {
                if(j == index) continue;
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }
        }

        if(execvp(pipe_cmd[index][0], pipe_cmd[index]) == -1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
    }
    else
    {
        for(j = 0; j < pipe_cmd_cnt - 1; ++j)
        {
            close(pipe_fd[j][0]);
            close(pipe_fd[j][1]);
        }
        for(j = 0; j < pipe_cmd_cnt; ++j)
        {
            wait(NULL);
        }
    }
}
