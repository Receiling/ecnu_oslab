#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "lock.h"

// list_init
void list_init(list_t *list) {
	list->head = NULL;
	lock_init(&list->lock, LOCK_TYPE);
}

// list_insert
void list_insert(list_t *list, unsigned int key) {
	// new a node
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) { // fail to new
		perror("malloc");
		return;
	}
	new->key = key;
	// just lock critical section
	lock_acquire(&list->lock, LOCK_TYPE);
	new->next = list->head;
	list->head = new;
	lock_release(&list->lock, LOCK_TYPE);
}

// list_delete
void list_delete(list_t *list, unsigned int key) {
	lock_acquire(&list->lock, LOCK_TYPE);
	node_t *node1 = list->head;
	node_t *node2 = NULL;
	if(node1 == NULL) // empty list (fail to delete)
		//perror("fail to delete, empty list");
		;
	else if(node1->key == key) { // the node is the head_node
		list->head = list->head->next;
		free(node1);
	}
	else {
		while (node1) {
			node2 = node1->next;
			if(node2 == NULL) { // no such a node (fail to delete)
				//perror("fail to delete, invalid element");
				break;
			}
			else if(node2->key == key) { // delete the node
				node1->next = node2->next;
				free(node2);
				break;
			}
			node1 = node1->next;
		}
	}
	lock_release(&list->lock, LOCK_TYPE);
}

// list_lookup
void *list_lookup(list_t *list, unsigned int key) {
	lock_acquire(&list->lock, LOCK_TYPE);
	node_t *result = list->head;
	while (result) {
		if (result->key == key) break;
		result = result->next;
	}
	lock_release(&list->lock, LOCK_TYPE);
	if(result == NULL)
		perror("lookup");
	return result; // both success and failure
}

void * list_dump(list_t *list){
    lock_acquire(&list->lock,LOCK_TYPE);
    node_t *ptr = list->head;
    printf("List Elements : ");
    int cnt = 0;
    while (ptr!=NULL){
        printf("%d ",ptr->key);
        ptr = ptr->next;
        cnt ++;
    }
    lock_release(&list->lock, LOCK_TYPE);
    printf("\n%d elements\n",cnt);
}
