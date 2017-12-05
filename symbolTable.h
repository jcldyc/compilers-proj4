#include <stdio.h>
#include <stdlib.h>

struct Rec {
  int index;
  int type;
};

typedef struct {
  int index;
  int type;
  struct Rec fields[100];
} STentry;

struct NodeST {
  STentry  *data;
  struct NodeST *prev;
  struct NodeST *next;
};


void push(struct NodeST** head, STentry* newEntry){
	struct NodeST* newNode = (struct NodeST*)malloc(sizeof(struct NodeST));
	newNode->next = (*head);
	newNode->prev = NULL;
  
	if((*head) != NULL){
		(*head)->prev = newNode;
	}
  
	(*head) = newNode;
  
	newNode->data = newEntry;
  
	return;
}
  
void pop(struct NodeST **head, struct NodeST *removed){
	if (*head == NULL || removed == NULL){ 
		return; 
	}
	if(*head == removed){ 
		*head = removed->next; 
	}
	if(removed->next != NULL){ 
		removed->next->prev = removed->next;
	}
	if(removed->prev != NULL){ 
		removed->prev->next = removed->next;
	}
	
	free(removed);  
	return;
}

extern STentry ST[];
