#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defrag.h"

char outfile[100];//输出文件的文件名
FILE *fin = NULL, *fout = NULL;
int size_of_file = 0;//总的文件的大小
int size_of_block = 0;//块的大小
int size_of_inode = 0;//结构体inode的size
int num_of_inodes = 0;//inode的个数
int num_of_iblock = 0;//每一个间接索引块所包含的下一级索引块的个数
int id_of_inode = 0;//当前处理的inode
int id_of_block = 0;//当前处理的block
int data_offset = 0;//数据区的偏移量，即数据区的开始地址
inode* inode_blocks = NULL;//用于记录新产生的inode

int min(int x, int y)
{
    if(x < y) return x;
    return y;
}
int frag_iblocks(int iblocks, int num_of_block)
{
    int* iblock = (int*)calloc(size_of_block / 4, 4);
    int* dblock = (int*)calloc(size_of_block / 4, 4);
    int sz = min(num_of_block, num_of_iblock);
    id_of_block += 1;
    int i = 0;
    for(i = 0; i < sz; ++i)
    {
        iblock[i] = id_of_block++;
    }
    fwrite(iblock, size_of_block, 1, fout);
    fseek(fin, 1024 + (data_offset + iblocks) * size_of_block, SEEK_SET);
    fread(iblock, size_of_block, 1, fin);
    for(int i = 0; i < sz; ++i)
    {
        fseek(fin, 1024 + (data_offset + iblock[i]) * size_of_block, SEEK_SET);
        fread(dblock, size_of_block, 1, fin);
        fwrite(dblock, size_of_block, 1, fout);
    }
    free(iblock);
    free(dblock);
    return num_of_block - sz;
}

int frag_i2block(int i2block, int num_of_block)
{
    int* iblock = (int*)calloc(size_of_block / 4, 4);
    int sz = min(num_of_block, num_of_iblock * num_of_iblock);
    id_of_block += 1;
    int num_use_block = (sz - 1) / num_of_block + 1;
    int i = 0;
    for(i = 0; i < num_use_block; ++i)
    {
        iblock[i] = id_of_block + (num_of_iblock + 1) * i;
    }
    fwrite(iblock, size_of_block, 1, fout);
    fseek(fin, 1024 + (data_offset + i2block) * size_of_block, SEEK_SET);
    fread(iblock, size_of_block, 1, fin);
    for(int i = 0; i < num_use_block; ++i)
    {
        num_of_block = frag_iblocks(iblock[i], num_of_block);
    }
    free(iblock);
    return num_of_block;
}

int frag_i3block(int i3block, int num_of_block)
{
    int* iblock = (int*)calloc(size_of_block / 4, 4);
    int sz = min(num_of_block, num_of_iblock * num_of_iblock * num_of_iblock);
    id_of_block += 1;
    int num_use_block = (sz - 1) / (num_of_iblock * num_of_iblock) + 1;
    int i = 0;
    for(i = 0; i < num_use_block; ++i)
    {
        iblock[i] = id_of_block + (num_of_iblock * (num_of_iblock + 1) + 1) * i;
    }
    fwrite(iblock, size_of_block, 1, fout);
    fseek(fin, 1024 + (data_offset + i3block) * size_of_block, SEEK_SET);
    fread(iblock, size_of_block, 1, fin);
    for(int i = 0; i < num_use_block; ++i)
    {
        num_of_block = frag_i2block(iblock[i], num_of_block);
    }
    free(iblock);
    return num_of_block;
}

void frag_block(inode* in, int* dblock)
{
    if(in->nlink == 0) return;
    id_of_inode += 1;
    memcpy(&inode_blocks[id_of_inode], in, sizeof(inode));
    int num_of_block = (in->size - 1) / size_of_block + 1;
    int i = 0;
    for(i = 0; i < N_DBLOCKS; ++i)
    {
        if(num_of_block == 0) return;
        inode_blocks[id_of_inode].dblocks[i] = id_of_block++;
        fseek(fin, 1024 + (data_offset + in->dblocks[i]) * size_of_block, SEEK_SET);
        fread(dblock, size_of_block, 1, fin);
        fwrite(dblock, size_of_block, 1, fout);
        num_of_block -= 1;
    }

    //一级索引
    for(i = 0; i < N_IBLOCKS; ++i)
    {
        if(num_of_block == 0) return;
        inode_blocks[id_of_inode].iblocks[i] = id_of_block;
        num_of_block = frag_iblocks(in->iblocks[i], num_of_block);
    }
    if(num_of_block == 0) return;

    //二级索引
    inode_blocks[id_of_inode].i2block = id_of_block;
    num_of_block = frag_i2block(in->i2block, num_of_block);
    if(num_of_block == 0) return;

    //三级索引
    inode_blocks[id_of_inode].i3block = id_of_block;
    num_of_block = frag_i3block(in->i3block, num_of_block);
    
    if(num_of_block > 0)
    {
        perror("file is too large!\n");
        exit(1);
    }
    return;
}

void modify_and_fill(superblock* sblock, inode* in)
{
    //填补数据区的空闲块，同时设置空闲块的next指针
    int num_of_data_region = (size_of_file - 1024 - data_offset * size_of_block - 1) / size_of_block + 1;
    int* dblock = (int*)calloc(size_of_block / 4, 4);
    int i = 0;
    for(i = id_of_block; i < num_of_data_region - 1; ++i)
    {
        dblock[0] = i + 1;
        fwrite(dblock, size_of_block, 1, fout);
    }
    dblock[0] = -1;
    fwrite(dblock, size_of_block, 1, fout);
    free(dblock);

    fseek(fout, 512, SEEK_SET);
    //修改superblock
    sblock->free_inode = id_of_inode + 1;
    sblock->free_iblock = id_of_block;
    fwrite(sblock, 512, 1, fout);

    //修改inode
    for(i = 0; i <= id_of_inode; ++i)
    {
        fwrite(&inode_blocks[i], sizeof(inode), 1, fout);
    }
    in->nlink = 0;
    for(; i < num_of_inodes - 1; ++i)
    {
        in->next_inode = i + 1;
        fwrite(in, sizeof(inode), 1, fout);
    }
    in->next_inode = -1;
    fwrite(in, sizeof(inode), 1, fout);
}

int main(int argc, char* argv[])
{
    int len = strlen(argv[1]);
    int dot = 0;
    for(dot = 0; dot < len; ++dot)
    {
    	if(argv[1][dot] == '.') break;
    }
    if(dot == len)
    {
    	sprintf(outfile, "%s-defrag", argv[1]);
    }
    else
    {
    	int i = 0;
    	for(i = 0; i < dot; ++i) outfile[i] = argv[1][i];
    	sprintf(outfile, "%s-defrag%s", outfile, argv[1] + dot);
    }

    //打开数据文件
    fin = fopen(argv[1], "r");
   	//fin = fopen("datafile-frag.txt", "r");
    if(fin == NULL)
    {
        perror("datafile open fail!\n");
        exit(1);
    }
    fseek(fin, 0, SEEK_END);
    size_of_file = ftell(fin);


    //创建输出文件
    fout = fopen(outfile, "w");
    //fout = fopen("datafile-frag-defrag.txt", "w");
    if(fout == NULL)
    {
        perror("create output file fail!\n");
        exit(1);
    }
    fseek(fout, 0, SEEK_SET);

    //读取superblock并写入，获取文件的主要信息
    fseek(fin, 0, SEEK_SET);
    superblock* sblock = (superblock*)malloc(512);
    fread(sblock, 512, 1, fin);
    fwrite(sblock, 512, 1, fout);
    fread(sblock, 512, 1, fin);
    fwrite(sblock, 512, 1, fout);
    size_of_block = sblock->size;
    data_offset = sblock->data_offset;

    //读取inode并写入，此次写入的iNode之后是要修改的，只是为了占位
    int inode_space = 512 * (sblock->data_offset - sblock->inode_offset);
    size_of_inode = sizeof(inode);
    inode* in = (inode*)malloc(size_of_inode);
    num_of_inodes = 0;
    while(inode_space >= size_of_inode)
    {
        fread(in, size_of_inode, 1, fin);
        fwrite(in, size_of_inode, 1, fout);
        inode_space -= size_of_inode;
        num_of_inodes += 1;
    }
    fwrite(in, inode_space, 1, fout);

    //重新读取每一个inode，然后逐个处理
    inode_blocks = (inode*)malloc(num_of_inodes * sizeof(inode));
    num_of_iblock = size_of_block / sizeof(int);
    int* dblock = (int*)calloc(size_of_block / 4, 4);
    id_of_inode = -1;
    id_of_block = 0;
    int i = 0;
    for(i = 0; i < num_of_inodes; ++i)
    {
        fseek(fin, 1024 + size_of_inode * i, SEEK_SET);
        fread(in, size_of_inode, 1, fin);
        frag_block(in, dblock);
    }

    //填充空闲数据块，修改inode,superlock

    modify_and_fill(sblock, in);

    free(sblock);
    free(in);
    free(inode_blocks);
    free(dblock);
    fclose(fin);
    fclose(fout);
    return 0;
}
