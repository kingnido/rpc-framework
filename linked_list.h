#ifndef _LINKED_LIST_H_ /* [ */
#define _LINKED_LIST_H_

typedef struct _node_t {
	void *user_data;
	struct _node_t *prev;
	struct _node_t *next;
} node_t;

typedef struct _linkedList_t {
	node_t *first;
	node_t *last;
	int length;
} linkedList_t;

int linkedList_append(linkedList_t *list, void *user_data);

void *linkedList_getAt(linkedList_t *list, int index);
void *linkedList_delAt(linkedList_t *list, int index);
int linkedList_addAt(linkedList_t *list, int index, void *user_data);

int linkedList_getLength(linkedList_t *list);

linkedList_t *linkedList_new();
int linkedList_destroy(linkedList_t *list);

#endif /* ] */
