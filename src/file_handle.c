#include <tio/filehandle.h>
#include <tio/memory.h>
#include <tio/macro.h>
#include <tio/poll.h>
#include <stdarg.h>

#ifdef HAVE_OPEN
#include <fcntl.h>
#endif

#if defined(HAVE_UNISTD_H) && defined(HAVE_CLOSE)
#include <unistd.h>
#include <errno.h>

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

static void tio_file_handle_close(tio_handle_t *handle) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	if (file_handle->auto_close) {
		close(file_handle->fd);
	}
	tio_free(file_handle);
}

static tio_ssize_t tio_file_handle_read(tio_handle_t *handle, void *buf, tio_size_t count) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	return tio_convert_error(read(file_handle->fd, buf, count));
}

static tio_ssize_t tio_file_handle_write(tio_handle_t *handle, const void *buf, tio_size_t count) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	return tio_convert_error(write(file_handle->fd, buf, count));
}

static int tio_file_handle_seek(tio_handle_t *handle, tio_off_t offset, int whence) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
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
	off_t ret = lseek(file_handle->fd, offset, whence);
	if (ret >= 0) return 0;
	return tio_convert_error(ret);
}

static tio_off_t tio_file_handle_tell(tio_handle_t *handle) {
	tio_file_handle_t *file_handle = TIO_CONTAINER_OF(handle, tio_file_handle_t, handle);
	off_t ret = lseek(file_handle->fd, 0, SEEK_CUR);
	if (ret >= 0) return ret;
	return tio_convert_error(ret);
}

static int tio_file_handle_start_poll(tio_handle_t *handle, tio_poll_t *poll) {
	utils_list_append(&handle->tio->file_polls, &poll->node);
	return 0;
}

static int tio_file_handle_stop_poll(tio_handle_t *handle, tio_poll_t *poll) {
	utils_list_remove(&handle->tio->file_polls, &poll->node);
	return 0;
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

#else

int tio_file_handle_create(tio_t *tio, tio_handle_t **handle, int fd, tio_bool_t auto_close) {
	(void)tio;
	(void)handle;
	(void)fd;
	(void)auto_close;
	return TIO_ERROR_NOT_SUPP;
}

#endif

int tio_file_handle_open(tio_t *tio, tio_handle_t **handle, const char *path, int flags, ...) {
#ifdef HAVE_OPEN
	int open_flags = 0;
	if (flags & TIO_OPEN_RDWR) {
		open_flags |= O_RDWR;
	} else if (flags & TIO_OPEN_WRONLY) {
		open_flags |= O_WRONLY;
	} else {
		open_flags |= O_RDONLY;
	}
	if (flags & TIO_OPEN_CREAT) {
		open_flags |= O_CREAT;
	}
	if (flags & TIO_OPEN_TRUNC) {
		open_flags |= O_TRUNC;
	}
	if (flags & TIO_OPEN_EXCL) {
		open_flags |= O_EXCL;
	}

	mode_t mode = 0666;
	if (flags & TIO_OPEN_CREAT) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	int fd = open(path, open_flags, mode);

	// TODO : convert errors
	if (fd < 0) return TIO_ERROR_IO;

	int ret = tio_file_handle_from_fd(tio, handle, fd, TIO_TRUE);
	if (ret < 0) close(fd);
	return ret;
#else
	(void)tio;
	(void)handle;
	(void)path;
	(void)flags;
	return TIO_ERROR_NOT_SUPP;
#endif
}
