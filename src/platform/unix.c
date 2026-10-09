#include <tio/platform.h>
#include <tio/error.h>
#include <tio/filehandle.h>
#include <tio/poll.h>
#include <tio/macro.h>
#include <libutils/vector.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

static int tio_platform_get_error(void) {
	switch (errno) {
	case EOPNOTSUPP:
		return TIO_ERROR_NOT_SUPP;
	case ENOENT:
		return TIO_ERROR_NOT_FOUND;
	default:
		return TIO_ERROR_IO;
	}
}

int tio_platform_open(tio_intptr_t *fd, const char *path, int flags, tio_mode_t mode) {
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
	*fd = open(path, open_flags, mode);
	if (*fd < 0) return tio_platform_get_error();
	return 0;
}

tio_ssize_t tio_platform_read(tio_intptr_t fd, void *buf, tio_size_t count) {
	ssize_t r = read(fd, buf, count);
	if (r < 0) return tio_platform_get_error();
	return r;
}

tio_ssize_t tio_platform_write(tio_intptr_t fd, const void *buf, tio_size_t count) {
	ssize_t w = write(fd, buf, count);
	if (w < 0) return tio_platform_get_error();
	return w;
}

tio_off_t tio_platform_seek(tio_intptr_t fd, tio_off_t offset, int whence) {
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
	off_t off = lseek(fd, offset, whence);
	if (off < 0) return tio_platform_get_error();
	return off;
}

void tio_platform_close(tio_intptr_t fd) {
	close(fd);
}

int tio_platform_poll(tio_t *tio) {
	// TODO : rewrite this to handle modifications 
	// of pollfds as we iterate
	utils_vector_t pollfds;
	utils_vector_init(&pollfds, sizeof(struct pollfd));
	utils_list_foreach (node, &tio->file_polls) {
		tio_poll_t *poll = TIO_CONTAINER_OF(node, tio_poll_t, node);
		struct pollfd fd = {
			.fd = TIO_CONTAINER_OF(poll->handle, tio_file_handle_t, handle)->fd,
			.events = 0,
		};
		if (poll->flags & TIO_POLL_READABLE) {
			fd.events |= POLLIN;
		}
		if (poll->flags & TIO_POLL_WRITEABLE) {
			fd.events |= POLLOUT;
		}
		if (poll->flags & TIO_POLL_DISCONNECTED) {
			fd.events |= POLLHUP;
		}
		utils_vector_push_back(&pollfds, &fd);
	}
	int ret = poll(pollfds.data, pollfds.count, -1);
	if (ret < 0) {
		utils_vector_destroy(&pollfds);
		return tio_platform_get_error();
	}
	size_t index = 0;
	utils_list_foreach (node, &tio->file_polls) {
		tio_poll_t *poll = TIO_CONTAINER_OF(node, tio_poll_t, node);
		struct pollfd *fd = utils_vector_at(&pollfds, index++);
		if (fd->revents) {
			tio_source_dispatch(&poll->source);
		}
	}
	utils_vector_destroy(&pollfds);
	return TIO_ERROR_SUCCESS;
}
