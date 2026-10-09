#ifndef TIO_FILEHANDLE_H
#define TIO_FILEHANDLE_H

#include "handle.h"
#include "types.h"

typedef struct tio_file_handle {
	tio_handle_t handle;
	tio_intptr_t fd;
	tio_bool_t auto_close;
} tio_file_handle_t;

#define TIO_OPEN_RDONLY 0
#define TIO_OPEN_WRONLY 1
#define TIO_OPEN_RDWR   2
#define TIO_OPEN_CREAT  3
#define TIO_OPEN_TRUNC  4
#define TIO_OPEN_EXCL   5

int tio_file_handle_open(tio_t *tio, tio_handle_t **handle, const char *path, int flags, ...);
int tio_file_handle_from_fd(tio_t *tio, tio_handle_t **handle, int fd, tio_bool_t auto_close);

#endif
