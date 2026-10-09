#include <tio/filehandle.h>
#include <tio/platform.h>
#include <tio/memory.h>
#include <tio/macro.h>
#include <tio/poll.h>
#include <stdarg.h>

static void tio_file_handle_close(tio_handle_t *handle) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	if (file_handle->auto_close) {
		tio_platform_close(file_handle->fd);
	}
	tio_free(file_handle);
}

static tio_ssize_t tio_file_handle_read(tio_handle_t *handle, void *buf, tio_size_t count) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	return tio_platform_read(file_handle->fd, buf, count);
}

static tio_ssize_t tio_file_handle_write(tio_handle_t *handle, const void *buf, tio_size_t count) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	return tio_platform_write(file_handle->fd, buf, count);
}

static int tio_file_handle_seek(tio_handle_t *handle, tio_off_t offset, int whence) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	off_t ret = tio_platform_seek(file_handle->fd, offset, whence);
	if (ret < 0) return ret;
	return TIO_ERROR_SUCCESS;
}

static tio_off_t tio_file_handle_tell(tio_handle_t *handle) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	return tio_platform_seek(file_handle->fd, 0, TIO_SEEK_CUR);
}

static int tio_file_handle_start_poll(tio_handle_t *handle, tio_poll_t *poll) {
	utils_list_append(&handle->tio->file_polls, &poll->node);
	return TIO_ERROR_SUCCESS;
}

static int tio_file_handle_stop_poll(tio_handle_t *handle, tio_poll_t *poll) {
	utils_list_remove(&handle->tio->file_polls, &poll->node);
	return TIO_ERROR_SUCCESS;
}

static tio_handle_ops_t tio_file_handle_ops = {
	.close       = tio_file_handle_close,
	.read        = tio_file_handle_read,
	.write       = tio_file_handle_write,
	.seek        = tio_file_handle_seek,
	.tell        = tio_file_handle_tell,
	.start_poll  = tio_file_handle_start_poll,
	.stop_poll   = tio_file_handle_stop_poll,
};

int tio_file_handle_from_fd(tio_t *tio, tio_handle_t **handle, int fd, tio_bool_t auto_close) {
	tio_file_handle_t *file_handle = tio_malloc(sizeof(tio_file_handle_t));
	if (!file_handle) return TIO_ERROR_NO_MEMORY;
	file_handle->fd = fd;
	file_handle->auto_close = auto_close;
	*handle = tio_handle_init(tio, &file_handle->handle, &tio_file_handle_ops);
	return TIO_ERROR_SUCCESS;
}

int tio_file_handle_open(tio_t *tio, tio_handle_t **handle, const char *path, int flags, ...) {
	tio_mode_t mode = 0666;
	if (flags & TIO_OPEN_CREAT) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}

	tio_intptr_t fd;
	int ret = tio_platform_open(&fd, path, flags, mode);
	if (ret < 0) return ret;

	ret = tio_file_handle_from_fd(tio, handle, fd, TIO_TRUE);
	if (ret < 0) tio_platform_close(fd);
	return ret;
}
