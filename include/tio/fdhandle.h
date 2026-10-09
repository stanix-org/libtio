#ifndef TIO_FDHANDLE_H
#define TIO_FDHANDLE_H

#include "handle.h"

typedef struct tio_fd_handle {
	tio_handle_t handle;
	int fd;
	tio_bool_t auto_close;
} tio_fd_handle_t;

int tio_fd_handle_create(tio_t *tio, tio_handle_t **handle, int fd, tio_bool_t auto_close);

#endif
