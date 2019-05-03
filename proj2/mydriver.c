#include <stdio.h>
#include <unistd.h>

#include "mymalloc.h"

void base_test() {
	// You can use sbrk(0) to get the current position of the break.
	// This is nice for testing cause you can see if the heap is the same size
	// before and after your tests, like here.
	void* heap_at_start = sbrk(0);

	void* block = my_malloc(100);
	my_free(block);

	void* heap_at_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_at_end - heap_at_start);

	if(heap_size_diff)
		printf("Hmm, the heap got bigger by %u (0x%X) bytes...\n", heap_size_diff, heap_size_diff);
}

void any_order_test() {
	// ADD MORE TESTS HERE.
	void* a = my_malloc(100);
	void* b = my_malloc(27);
	void* c = my_malloc(40);

	my_free(b);
	my_free(a);
	my_free(c);
}

void coalesce_test() {
	void* heap_start = sbrk(0);
	void* a = my_malloc(40);
	void* b = my_malloc(40);
	void* c = my_malloc(40);
	void* d = my_malloc(40);
	// void* e = my_malloc(40);

	my_free(a); // 40B
	my_free(b); // 96B
	my_free(c); // 152B
	my_free(d); // 208B

	void* heap_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_end - heap_start);

	if(heap_size_diff)
		printf("The heap got bigger by %u (0x%X) bytes...\n",
			heap_size_diff, heap_size_diff);
	else
		printf("Everything seems fine...");
	
}

int main() {
	// base_test();
	// any_order_test();
	coalesce_test();

	return 0;
}
