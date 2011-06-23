#include <assert.h>
#include "CDLList.h"


/* Matt Atwood 6/22/11
 * Function initializes the list.
 * Param: list is the list
 * Pre: list is not null
 * Post: list->size is 0 and list->lock is 0
 */

void initList(struct List *list){
	assert(list != NULL);
	struct Node *fSent = (struct Node *)malloc(sizeof(struct Node));
	struct Node *bSent = (struct Node *)malloc(sizeof(struct Node));
	list->size = 0;
	list->lock = 0;
	list->head = fSent;
	list->tail = bSent;
	list->tail->value = -2;
	list->head->value = -1;
	list->head->next = list->tail;
	list->tail->prev = list->head;
}

/* Matt Atwood 6/22/11
 * Function adds the node in order
 * Param: list is the List, node is the node to add
 * Pre: list and node != NULL
 * Post: node is in list in order and size is incremented
 */
void addList(struct List *list, struct Node *node){
	struct Node *place;

	assert(list != NULL);
	assert(node != NULL);

	while(list->lock == 1) ;
	list->lock = 1;
	
	if(list->size < 1){
		list->tail->prev = node;
		node->next = list->tail;
		list->head->next = node;
		node->prev = list->head;	
	}

	else{
		/* find the point of entry */
		place = list->tail->prev;
		while(place->value > node->value){
			place = place->prev;
			if(place == list->head) break;	
		}
		/*change the ptrs */
		node->prev = place;		
		node->next = place->next;
		place->next = node;
		node->next->prev = node;
	}
	list->size++;
	list->lock = 0;	
}


