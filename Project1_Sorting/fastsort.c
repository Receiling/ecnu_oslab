#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <malloc.h>
#include <sys/stat.h>

typedef struct Node
{
	int index;
	unsigned int key;
}node;
void usage(char *prog)
{
	fprintf(stderr, "usage: %s <inputfile outputfile>", prog); 
}
int cmp(const void *a, const void *b)
{
	node p = *(node*)a;
	node q = *(node*)b;
	if(p.key != q.key)
		return p.key - q.key;
	return p.index - q.index;
}

int main(int argc, char* argv[])
{
	//input params
	if(argc != 3) usage(argv[0]);
	char* infile = "no/such/file";
	char* outfile = "no/such/file";
	infile = strdup(argv[1]);
	outfile = strdup(argv[2]);
	//printf("%s\n%s", infile, outfile);
	
	//get inputfile size
	struct stat buf; 
	stat(infile, &buf);

	// open and read input file
	int fd = open(infile, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}

	//create array according to file size
	rec_t* re = (rec_t*)malloc(sizeof(rec_t) * (buf.st_size / 100 + 10));
	if(!re)
	{
		perror("rec_t malloc");
		exit(1);
	}
	node* nodes = (node*)malloc(sizeof(node) * (buf.st_size / 100 + 10));
	if(!nodes)
	{
		perror("nodes malloc");
		exit(1);
	}

	int cnt = 0;
	int rc;
	while(1)
	{
		nodes[cnt].index = cnt;
		rc = read(fd, &re[cnt], sizeof(rec_t));
		nodes[cnt].key = re[cnt].key;
		if (rc == 0) // 0 indicates EOF
			 break;
		 if (rc < 0)
		 {
			 perror("read");
			 exit(1);
		 }
		 cnt += 1;
	}
	//close inputfile
	(void)close(fd);

	//sort
	qsort(nodes, cnt, sizeof(node), cmp);
	
	// open and create output file
	fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
	if (fd < 0)
	{
		perror("open");
		exit(1);
	}
	int i = 0;
	for(i = 0; i < cnt; ++i)
	{
		//printf("%d\n", nodes[i].index);
		rc = write(fd, &re[nodes[i].index], sizeof(rec_t));
		if (rc != sizeof(rec_t)) 
		{
			perror("write");
			exit(1);
		}
	}
	//free struct array
	free(re);
	free(nodes);
	//close outputfile
	(void)close(fd);
	return 0;
}
