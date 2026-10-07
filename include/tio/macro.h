#ifndef TIO_MACRO_H
#define TIO_MACRO_H

#include <stddef.h>

#define TIO_CONTAINER_OF(ptr, type, member) ((type *)((char*)(ptr) - offsetof(type, member)))

#endif
