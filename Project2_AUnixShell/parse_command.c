#include "mysh.h"

//parse pre_processing commond
command_info parse_command(char* parament[], int param_cnt)
{
    char error_message[30] = "An error has occurred\n";
	command_info info;
	info.isValid = TRUE;
	info.isRedir = info.isBackgd = info.isPipe = FALSE;
	info.outfile = NULL;
	info.pipe_cmd_cnt = 0;
	int pos = 0;
	int tag = 1;
	for(; pos < param_cnt; ++pos)
	{
		if(strcmp(parament[pos], ">") == 0)
		{
			if(info.isRedir || pos == 0)
			{
				info.isValid = FALSE;
				break;
			}
			else
			{
				parament[pos] = NULL;
				info.isRedir = TRUE;
			}
		}
		else if(strcmp(parament[pos], "&") == 0)
		{
			if(info.isBackgd || pos == 0)
			{
				info.isValid = FALSE;
				break;
			}
			else
			{
				parament[pos] = NULL;
				info.isBackgd = TRUE;
			}
		}
		else if(strcmp(parament[pos], "|") == 0)
		{
			if(pos == 0 || (pos + 1 < param_cnt && strcmp(parament[pos + 1], "|") == 0))
			{
				info.isValid = FALSE;
				break;
			}
			else
			{
				parament[pos] = NULL;
				info.isPipe = TRUE;
				tag = 1;
			}
		}
		else
		{
            if(tag)
            {
                info.pipe_cmd[info.pipe_cmd_cnt++] = &parament[pos];
                tag = 0;
            }
			if(info.isRedir)
			{
				if(info.outfile != NULL)
				{
					info.isValid = FALSE;
					break;
				}
				info.outfile = parament[pos];
			}
			else if(info.isBackgd)
			{
				info.isValid = FALSE;
				break;
			}
		}
	}
	return info;
}
