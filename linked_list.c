#include <stdlib.h>

#include "linked_list.h"

static node_t *node_new()
{
	node_t *new_node = NULL;

	new_node = (node_t *) malloc(sizeof(node_t));
	if (!new_node) {
		return NULL;
	}

	new_node->prev = NULL;
	new_node->next = NULL;
	new_node->user_data = NULL;

	return new_node;
}

static int node_destroy(node_t *node)
{
	if (!node) {
		return -1;
	}

	node->next = NULL;
	node->prev = NULL;
	node->user_data = NULL;

	free(node);

	return 0;
}

static int list_insertNodeBetween(linkedList_t *list, node_t *new_node, node_t *prev, node_t *next)
{
	if (prev) {
		prev->next = new_node;
		new_node->prev = prev;
	}

	if (next) {
		next->prev = new_node;
		new_node->next = next;
	}

	if (next == list->first) {
		list->first = new_node;
	}

	if (prev == list->last) {
		list->last = new_node;
	}

	list->length = list->length + 1;

	return 0;
}

static int list_insertNodeAfter(linkedList_t *list, node_t *new_node, node_t *prev_node)
{
	node_t *prev = NULL;
	node_t *next = NULL;

	if (prev_node) {
		prev = prev_node;
		next = prev_node->next;
	}

	return list_insertNodeBetween(list, new_node, prev, next);
}

static int list_insertNodeBefore(linkedList_t *list, node_t *new_node, node_t *next_node)
{
	node_t *prev = NULL;
	node_t *next = NULL;

	if (next_node) {
		next = next_node;
		prev = next_node->prev;
	}

	return list_insertNodeBetween(list, new_node, prev, next);
}

static int list_removeNode(linkedList_t *list, node_t *node)
{
	if (node->next) {
		node->next->prev = node->prev;
	}

	if (node->prev) {
		node->prev->next = node->next;
	}

	if (node == list->first) {
		list->first = node->next;
	}

	if (node == list->last) {
		list->last = node->prev;
	}

	list->length = list->length - 1;

	return 0;
}

static node_t *list_getNodeAt(linkedList_t *list, int index)
{
	int i;
	node_t *curr = NULL;

	if (index < (list->length / 2)) {
		i = 0;
		curr = list->first;
		while (i != index) {
			i++;
			curr = curr->next;
		}
	} else {
		i = list->length - 1;
		curr = list->last;
		while (i != index) {
			i--;
			curr = curr->prev;
		}
	}

	return curr;
}

int linkedList_append(linkedList_t *list, void *user_data)
{
	node_t *new_node = NULL;
	int error;

	if (!list || !user_data) {
		return -1;
	}

	new_node = node_new();
	if (!new_node) {
		return -1;
	}

	new_node->user_data = user_data;

	list_insertNodeAfter(list, new_node, list->last);

	return 0;
}

void *linkedList_getAt(linkedList_t *list, int index)
{
	node_t *node = NULL;
	int i = 0;

	if (!list || index < 0 || index >= list->length) {
		return NULL;
	}

	node = list_getNodeAt(list, index);

	return node->user_data;
}

void *linkedList_delAt(linkedList_t *list, int index)
{
	node_t *node = NULL;
	void *user_data = NULL;
	int i = 0;

	if (!list || index < 0 || index >= list->length) {
		return NULL;
	}

	node = list_getNodeAt(list, index);

	user_data = node->user_data;

	list_removeNode(list, node);
	node_destroy(node);

	return user_data;
}

int linkedList_addAt(linkedList_t *list, int index, void *user_data)
{
	node_t *node = NULL;
	node_t *new_node = NULL;
	int i = 0;

	if (!list || index < 0 || index >= list->length) {
		return -1;
	}

	node = list_getNodeAt(list, index);

	new_node = node_new();
	if (!new_node) {
		return -1;
	}

	new_node->user_data = user_data;

	return list_insertNodeBefore(list, new_node, node);
}

int linkedList_getLength(linkedList_t *list)
{
	if (!list) {
		return 0;
	}

	return list->length;
}

linkedList_t *linkedList_new()
{
	linkedList_t *list = NULL;

	list = (linkedList_t *) malloc(sizeof(linkedList_t));
	if (!list) {
		return NULL;
	}

	list->first = NULL;
	list->last = NULL;
	list->length = 0;

	return list;
}

int linkedList_destroy(linkedList_t *list)
{
	if (!list) {
		return -1;
	}

	list->first = NULL;
	list->last = NULL;
	list->length = 0;

	free(list);

	return 0;
}
