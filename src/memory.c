#include <tio/memory.h>
#include <stdlib.h>

void *tio_malloc(tio_size_t size) {
	return malloc(size);
}

void tio_free(void *ptr) {
	return free(ptr);
}
