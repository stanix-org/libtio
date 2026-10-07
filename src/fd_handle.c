#include <tio/handle.h>
#include <tio/memory.h>
#include <tio/macro.h>

#if defined(HAVE_UNISTD_H) && defined(HAVE_CLOSE)
#include <unistd.h>
#include <errno.h>

typedef struct tio_fd_handle {
	tio_handle_t handle;
	int fd;
	tio_bool_t auto_close;
} tio_fd_handle_t;

static tio_ssize_t tio_convert_error(tio_ssize_t ret) {
	if (ret >= 0) return ret;
	switch (errno) {
	case EOPNOTSUPP:
		return TIO_ERROR_NOT_SUPP;
	case ENOENT:
		return TIO_ERROR_NOT_FOUND;
	default:
		return TIO_ERROR_IO;
	}
}

static void tio_fd_handle_close(tio_handle_t *handle) {
	tio_fd_handle_t *fd_handle = TIO_CONTAINER_OF(handle, tio_fd_handle_t, handle);
	if (fd_handle->auto_close) {
		close(fd_handle->fd);
	}
	tio_free(fd_handle);
}

static tio_ssize_t tio_fd_handle_read(tio_handle_t *handle, void *buf, tio_size_t count) {
	tio_fd_handle_t *fd_handle = TIO_CONTAINER_OF(handle, tio_fd_handle_t, handle);
	return tio_convert_error(read(fd_handle->fd, buf, count));
}

static tio_ssize_t tio_fd_handle_write(tio_handle_t *handle, const void *buf, tio_size_t count) {
	tio_fd_handle_t *fd_handle = TIO_CONTAINER_OF(handle, tio_fd_handle_t, handle);
	return tio_convert_error(write(fd_handle->fd, buf, count));
}

static int tio_fd_handle_seek(tio_handle_t *handle, tio_off_t offset, int whence) {
	tio_fd_handle_t *fd_handle = TIO_CONTAINER_OF(handle, tio_fd_handle_t, handle);
	switch (whence) {
	case TIO_SEEK_SET:
		whence = SEEK_SET;
		break;
	case TIO_SEEK_CUR:
		whence = SEEK_CUR;
		break;
	case TIO_SEEK_END:
		whence = SEEK_END;
		break;
	default:
		return TIO_ERROR_INVALID;
	}
	off_t ret = lseek(fd_handle->fd, offset, whence);
	if (ret >= 0) return 0;
	return tio_convert_error(ret);
}

static tio_off_t tio_fd_handle_tell(tio_handle_t *handle) {
	tio_fd_handle_t *fd_handle = TIO_CONTAINER_OF(handle, tio_fd_handle_t, handle);
	off_t ret = lseek(fd_handle->fd, 0, SEEK_CUR);
	if (ret >= 0) return ret;
	return tio_convert_error(ret);
}

static tio_handle_ops_t tio_fd_handle_ops = {
	.close = tio_fd_handle_close,
	.read  = tio_fd_handle_read,
	.write = tio_fd_handle_write,
	.seek  = tio_fd_handle_seek,
	.tell  = tio_fd_handle_tell,
};

int tio_fd_handle_create(tio_handle_t **handle, int fd, tio_bool_t auto_close) {
	tio_fd_handle_t *fd_handle = tio_malloc(sizeof(tio_fd_handle_t));
	if (!fd_handle) return TIO_ERROR_NO_MEMORY;
	fd_handle->fd = fd;
	fd_handle->auto_close = auto_close;
	*handle = tio_handle_init(&fd_handle->handle, &tio_fd_handle_ops);
	return TIO_ERROR_SUCCESS;
}

#else

int tio_fd_handle_create(tio_handle_t **handle, int fd, tio_bool_t auto_close) {
	(void)handle;
	(void)fd;
	(void)auto_close;
	return TIO_ERROR_NOT_SUPP;
}

#endif
