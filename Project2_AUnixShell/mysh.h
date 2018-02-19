#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAXSIZE 4096
#define MAXPARAM 1000
#define MAXNBGPROC 1000
#define TRUE 1
#define FALSE 0
#define MAXPIPECOMMAND 100

typedef struct Command_info
{
	int isValid;
	int isRedir, isBackgd, isPipe;
	int pipe_cmd_cnt;
	char** pipe_cmd[MAXPARAM];
	char* outfile;
	char* command[MAXPARAM];
}command_info;

command_info parse_command(char* parament[], int param_cnt);
void mypipe(int pipe_cmd_cnt, char** pipe_cmd[], char* outfile);
int builtin_command(char* parament[], int parament_cnt);
void myproc(FILE* instream, int model);
