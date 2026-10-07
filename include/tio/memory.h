#ifndef TIO_MEMORY_H
#define TIO_MEMORY_H

#include "types.h"

void *tio_malloc(tio_size_t size);
void tio_free(void *ptr);

#endif
