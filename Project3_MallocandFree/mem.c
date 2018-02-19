#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "mem.h"


int m_error;
// allocated node
typedef struct HEAD_T
{
    long long size;
    long long magic;
}head_t;

//free node
typedef struct NODE_T
{
    long long size;
    struct NODE_T* next;
}node_t;
node_t* free_head = NULL;

//count init
int init_cnt = 0;

//initialize a space for allocating
int mem_init(int size_of_region)
{
    init_cnt += 1;
    if(size_of_region <= 16 || init_cnt > 1)
    {
        m_error = E_BAD_ARGS;
        return -1;
    }

    // open the /dev/zero device
    int fd = open("/dev/zero", O_RDWR);

    // size_of_region (in bytes) needs to be evenly divisible by the page size
    void *ptr = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED)
    {
        m_error = E_BAD_ARGS;
        return -1;
    }

    //initialize free list head
    free_head = (node_t*)ptr;
    free_head -> size = size_of_region - sizeof(node_t);
    free_head -> next = NULL;

    // close the device (don't worry, mapping should be unaffected)
    close(fd);
    return 0;
}

/*
return: free space node pointer; if not found, return NULL
param: size is size of allocating space, style is way to find:
0 ---- best find, 1 ------- worst find, 2 -------- first find
pre_node should be returned, pre_node is pre node to found node, pre -> next will be used,
if node is the first node, pre = NULL
*/
node_t* free_node_find(int size, int style, node_t** pre_node)
{
    node_t *target = NULL;
    node_t *head = free_head;
    node_t *pre = NULL;
    int space = -1;
    if(style == M_BESTFIT)
    {
        while(head != NULL)
        {
            if(head -> size >= size)
            {
                if(space == -1 || space > head -> size)
                {
                    space = head -> size;
                    target = head;
                    *pre_node = pre;
                }
            }
            pre = head;
            head = head -> next;
        }
    }
    else if(style == M_WORSTFIT)
    {
        while(head != NULL)
        {
            if(head -> size >= size)
            {
                if(space == -1 || space < head -> size)
                {
                    space = head -> size;
                    target = head;
                    *pre_node = pre;
                }
            }
            pre = head;
            head = head -> next;
        }
    }
    else if(style == M_FIRSTFIT)
    {
        while(head != NULL)
        {
            if(head -> size >= size)
            {
                target = head;
                *pre_node = pre;
                break;
            }
            pre = head;
            head = head -> next;
        }
    }
    return target;
}

void* mem_alloc(int size, int style)
{
    node_t* pre_node = NULL;
    if(size <= 0 || style < 0 || style > 2)
    {
        m_error = E_BAD_ARGS;
        return pre_node;
    }
    //8 byte align
    if(size % 8 != 0) size = size + (8 - size % 8);

    node_t* free_node  = free_node_find(size, style, &pre_node);
    if(free_node == NULL)
    {
        m_error = E_NO_SPACE;
        return free_node;
    }

    long long space = free_node -> size;
    node_t *next = free_node -> next;
    //create allocated node
    head_t* allocated_node = (head_t*)free_node;
    allocated_node -> size = size;
    allocated_node -> magic = 12345678;

    void* st_node = (void*)(allocated_node + 1);
    if(space - size <= sizeof(node_t))
    {
        allocated_node -> size = space;
        if(pre_node == NULL)
            free_head = next;
        else pre_node -> next = next;
        return st_node;
    }
    // this is not test, node_t* new_node = (node_t*)((char*)st_node + size);
    node_t* new_node = (node_t*)(st_node + size);
    new_node -> next = next;
    new_node -> size = space - size - sizeof(node_t);
    if(pre_node == NULL)
        free_head = new_node;
    else pre_node -> next = new_node;
    return st_node;
}

/*
according request in problem
*/
int mem_free(void * ptr)
{
    if (ptr == NULL) return 0;

    head_t *target = ptr - sizeof(head_t);
    if (target -> magic != 12345678){
        m_error = E_BAD_POINTER;
        return -1;
    }

    void* free_add = (void*)target;
    long long size = target -> size;

    node_t *head = free_head;
    node_t *pre = NULL;
    while (head != NULL && (void*)head < free_add) {
        pre = head;
        head = head->next;
    }

    int tag = 0;
    if (pre == NULL) {  //插到头
        node_t *new_node = (node_t *) free_add;
        new_node -> size = size;
        new_node -> next = head;
        free_head = new_node;
        tag = 1;
    }
    else if ((char *)pre + pre->size + sizeof(node_t) == free_add){ //和前面一个节点合并
        pre->size = pre->size + size + sizeof(head_t);
        free_add = (void*)pre;
        size = pre -> size;
        tag = 1;
    }

    if ((char *) free_add + size + sizeof(node_t) == (char*)head) //和后面一个节点合并
    {
        node_t *new_node  = (node_t *)free_add;
        new_node -> size = size + sizeof(node_t) + head -> size;
        if(pre == NULL)
            free_head = new_node;
        else pre->next = new_node;
        new_node->next = head->next;
        tag = 1;
    }

    if(tag == 0)
    {  //插到中间
        node_t *new_node = (node_t *) free_add;
        pre->next = new_node;
        new_node -> next = head;
        new_node -> size = size;
    }
    return 0;
}


/*
for test
*/
void mem_dump()
{
    node_t* head = free_head;
    printf("--------------------------------------\n");
    while(head != NULL)
    {
        printf("address: %p, size: %lld\n", head, head -> size);
        head = head -> next;
    }
    printf("--------------------------------------\n\n");
    return;
}

/*int main()
{
    //printf("%d %d\n", sizeof(node_t), sizeof(head_t));
    int mem = mem_init(200);
    char* m1 = (char*)mem_alloc(10, 0);
    head_t* hm1 = m1 - sizeof(head_t);
    printf("allocated adress: %p, size: %d\n", m1, hm1 -> size);
    mem_dump();
    char* m2 = (char*)mem_alloc(10, 1);
    head_t* hm2 = m2 - sizeof(head_t);
    printf("allocated adress: %p, size: %d\n", m2, hm2 -> size);
    mem_dump();
    char* m3 = (char*)mem_alloc(10, 2);
    head_t* hm3 = m3 - sizeof(head_t);
    printf("allocated adress: %p, size: %d\n", m3, hm3 -> size);
    mem_dump();
    //printf("------------------free space test---------------------\n");
    mem_free(m2);
    mem_dump();
    m2 = (char*)mem_alloc(10, 1);
    hm2 = m2 - sizeof(head_t);
    printf("allocated adress: %p, size: %d\n", m2, hm2 -> size);
    mem_dump();
    mem_free(m3);
    mem_dump();
    m3 = (char*)mem_alloc(10, 1);
    hm3 = m3 - sizeof(head_t);
    printf("allocated adress: %p, size: %d\n", m3, hm3 -> size);
    mem_dump();
    mem_free(m1);
    mem_dump();
    mem_free(m2);
    mem_dump();
    mem_free(m3);
    mem_dump();
    return 0;
}*/
