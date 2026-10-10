#ifndef TIO_FILEHANDLE_H
#define TIO_FILEHANDLE_H

#include "handle.h"
#include "types.h"

typedef struct tio_file_handle {
	tio_handle_t handle;
	tio_intptr_t fd;
	tio_bool_t auto_close;
} tio_file_handle_t;

#define TIO_OPEN_RDONLY 0x00
#define TIO_OPEN_WRONLY 0x01
#define TIO_OPEN_RDWR   0x02
#define TIO_OPEN_CREAT  0x04
#define TIO_OPEN_TRUNC  0x08
#define TIO_OPEN_EXCL   0x10

int tio_file_handle_open(tio_t *tio, tio_handle_t **handle, const char *path, int flags, ...);
int tio_file_handle_from_fd(tio_t *tio, tio_handle_t **handle, int fd, tio_bool_t auto_close);

#endif
