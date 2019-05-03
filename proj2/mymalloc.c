#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// USE THIS GODDAMN MACRO OKAY
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8

// Header will be 16 bytes
typedef struct Node {
	unsigned int data_size;
	int in_use;
	struct Node* previous;
	struct Node* next;
} Node;

Node* heap_head = NULL;
Node* heap_tail = NULL;

unsigned int round_up_size(unsigned int data_size) {
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

// Appends new node to end of the list
Node* create_header(unsigned int size) {
	Node* header = sbrk(sizeof(Node));
	header->data_size = size;
	header->in_use = 1;

	// Set first node if no nodes
	if(heap_head == NULL) {
		heap_head = header;
		heap_tail = header;
		header->previous = NULL;
		header->next = NULL;
	} else {
		// Set node at last pos if head node is set
		header->previous = heap_tail;
		header->next = NULL;
		heap_tail->next = header;
		heap_tail = header;
	}

	return header;
}

// splits free blocks if too large
void split_blocks(Node* node, int size) {
	Node* next = PTR_ADD_BYTES(node, sizeof(Node)+size);
	next->in_use = 0;
	next->data_size = node->data_size - size;
	next->previous = node;
	next->next = node->next;
	node->data_size = size;
	node->next = next;
	next->next->previous = next;
}

// Reuses freed nodes
void* first_fit(unsigned int size) {
	for(Node* curr = heap_head; curr != NULL; curr = curr->next) {
		if(curr->in_use == 0 && curr->data_size >= size) {
			if(curr->data_size > size) {
				split_blocks(curr, size);
			}
			curr->in_use = 1;
			return PTR_ADD_BYTES(curr, sizeof(Node));
		}
	}
	return NULL;
}

void* my_malloc(unsigned int size) {
	if(size == 0)
		return NULL;

	size = round_up_size(size);

	// ------- Don't remove anything above this line. -------

	void* ptr = first_fit(size);

	if(ptr == NULL) {
		create_header(size);
		ptr = sbrk(size);
	}

	return ptr;
}

// Coalesces neighboring free nodes
void coalesce_blocks(Node* header) {
	Node* prev = header->previous;
	Node* next = header->next;
	if(header->in_use == 0) {
		if(prev != NULL && next != NULL && prev->in_use == 0 && next->in_use == 0) {
			// Both are free
			prev->data_size += header->data_size + next->data_size + (2*sizeof(Node));
			prev->next = next->next;
			if(next->next != NULL) {
				next->next->previous = prev;
			} else {
				heap_tail = prev;
			}
		} else if(prev != NULL && prev->in_use == 0) {
			// Prev is free
			prev->data_size += header->data_size + sizeof(Node);
			prev->next = header->next;
			if(header->next != NULL) {
				header->next->previous = prev;
			} else {
				heap_tail = prev;
			}
			header = prev;
		} else if(next != NULL && next->in_use == 0) {
			// Next is free
			header->data_size += next->data_size + sizeof(Node);
			header->next = next->next;
			if(next->next != NULL) {
				next->next->previous = header;
			} else {
				heap_tail = header;
			}
		}
	}
}

void my_free(void* ptr) {
	if(ptr == NULL)
		return;

	Node* header = PTR_ADD_BYTES(ptr, 0-sizeof(Node));
	header->in_use = 0;
	
	if(header->previous == NULL)
		heap_head = header;
	if(header->next == NULL)
		heap_tail = header;
	
	coalesce_blocks(header);

	if(heap_head == heap_tail) {
		// Removing last node
		if(heap_head != NULL) {
			if(heap_head->next == NULL && heap_head->in_use == 0) {
				brk(heap_head);
				heap_head = NULL;
				heap_tail = NULL;
			} else {
				brk(header);
				heap_tail = header->previous;
				heap_tail->next = NULL;
			}
		}		
	}
}
