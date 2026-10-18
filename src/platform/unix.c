#include <tio/platform.h>
#include <tio/error.h>
#include <tio/filehandle.h>
#include <tio/poll.h>
#include <tio/macro.h>
#include <tio/timer.h>
#include <libutils/vector.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
#endif
#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

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
	utils_vector_t pollfds;
	utils_vector_init(&pollfds, sizeof(struct pollfd));
	utils_vector_t polls;
	utils_vector_init(&polls, sizeof(tio_poll_t *));

	utils_list_foreach (node, &tio->file_polls) {
		tio_poll_t *poll = TIO_CONTAINER_OF(node, tio_poll_t, node);
		
		// we keep a ref to the poll source
		// to prevent it from being freed while we poll
		tio_source_ref(&poll->source);

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
		utils_vector_push_back(&polls, &poll);
	}
	int ret = poll(pollfds.data, pollfds.count, tio_timer_get_before_next_trigger(tio));
	if (ret < 0) {
		utils_vector_destroy(&pollfds);
		utils_vector_destroy(&polls);
		return tio_platform_get_error();
	}
	utils_vector_typed_indexed_foreach (tio_poll_t *, poll_ptr, index, &polls) {
		tio_poll_t *poll = *poll_ptr;
		struct pollfd *fd = utils_vector_at(&pollfds, index);
		if (fd->revents) {
			tio_source_dispatch(&poll->source);
		}
	
		tio_source_release(&poll->source);
	}
	utils_vector_destroy(&pollfds);
	utils_vector_destroy(&polls);
	return TIO_ERROR_SUCCESS;
}

tio_time_t tio_platform_get_current_time(void) {
#ifdef HAVE_CLOCK_GETTIME
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#elif defined(HAVE_GETTIMEOFDAY)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_nsec / 1000;
#else
#error "no supported way of getting time"
#endif
}
