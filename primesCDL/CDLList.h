#include <stdlib.h>
#include <stdio.h>

#define TYPE  long

/* Matthew Atwood 6/22/11
 * A Circularly Double Linked List with a Lock 
 * for a multithreaded programs. The list is kept
 * in order.
 */

struct Node{
	TYPE value;
	struct Node *next;
	struct Node *prev;
};

struct List{
	struct Node *head;
	struct Node *tail;
	int size;
	int lock;
};

/* List interface */
void initList(struct List *list);
void addList(struct List *list, struct Node *node);
