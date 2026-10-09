#ifndef TIO_POLL_H
#define TIO_POLL_H

#include "source.h"
#include "handle.h"

typedef struct tio_poll tio_poll_t;

struct tio_poll {
	tio_source_t source;
	utils_list_node_t node;
	tio_handle_t *handle;
	void *data;
	int flags;
};

#define TIO_POLL_READABLE     0x1
#define TIO_POLL_WRITEABLE    0x2
#define TIO_POLL_DISCONNECTED 0x4

int tio_poll_create(tio_source_t **source, tio_handle_t *handle, int flags);

#endif
