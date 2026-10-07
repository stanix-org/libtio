#ifndef TIO_HANDLE_H
#define TIO_HANDLE_H

#include "types.h"
#include "atomic.h"
#include "error.h"

typedef struct tio_handle_ops tio_handle_ops_t;
typedef struct tio_handle tio_handle_t;

struct tio_handle_ops {
	void (*close)(tio_handle_t *handle);
	int (*seek)(tio_handle_t *handle, tio_off_t offset, int whence);
	tio_off_t (*tell)(tio_handle_t *handle);
	tio_ssize_t (*read)(tio_handle_t *handle, void *buf, tio_size_t count);
	tio_ssize_t (*write)(tio_handle_t *handle, const void *buf, tio_size_t count);
};

struct tio_handle {
	TIO_ATOMIC(size_t) ref_count;
	tio_handle_ops_t *ops;
};

tio_ssize_t tio_handle_read(tio_handle_t *handle, void *buf, tio_size_t count);
tio_ssize_t tio_handle_write(tio_handle_t *handle, const void *buf, tio_size_t count);
int tio_handle_seek(tio_handle_t *handle, tio_off_t offset, int whence);
tio_off_t tio_handle_tell(tio_handle_t *handle);

#define TIO_SEEK_SET 0
#define TIO_SEEK_CUR 1
#define TIO_SEEK_END 2

tio_handle_t *tio_handle_init(tio_handle_t *handle, tio_handle_ops_t *ops);

static inline tio_handle_t *tio_handle_ref(tio_handle_t *handle) {
	if (handle) TIO_ATOMIC_FETCH_ADD(&handle->ref_count, 1);
	return handle;
}

void tio_handle_release(tio_handle_t *handle);

int tio_fd_handle_create(tio_handle_t **handle, int fd, tio_bool_t auto_close);

#endif
