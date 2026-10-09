#include <tio/ctx.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <tio/fdhandle.h>
#include <tio/macro.h>
#include <tio/poll.h>
#include <libutils/vector.h>
#include <string.h>
#ifdef HAVE_POLL
#include <poll.h>
#endif

int tio_init(tio_t **tio) {
	*tio = tio_malloc(sizeof(tio_t));
	if (!*tio) return TIO_ERROR_NO_MEMORY;
	memset(*tio, 0, sizeof(tio_t));
	return TIO_ERROR_SUCCESS;
}

int tio_run(tio_t *tio) {
	while (!tio->quit) {
#ifdef HAVE_POLL
		// TODO : poll fds
		utils_vector_t pollfds;
		utils_vector_init(&pollfds, sizeof(struct pollfd));
		utils_list_foreach (node, &tio->fd_polls) {
			tio_poll_t *poll = TIO_CONTAINER_OF(node, tio_poll_t, node);
			struct pollfd fd = {
				.fd = TIO_CONTAINER_OF(poll->handle, tio_fd_handle_t, handle)->fd,
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
			// TODO : convert
			return TIO_ERROR_IO;
		}
		size_t index = 0;
		utils_list_foreach (node, &tio->fd_polls) {
			tio_poll_t *poll = TIO_CONTAINER_OF(node, tio_poll_t, node);
			struct pollfd *fd = utils_vector_at(&pollfds, index++);
			if (fd->revents) {
				tio_source_dispatch(&poll->source);
			}
		}
		utils_vector_destroy(&pollfds);
#endif
	}
	return tio->exit_code;
}

void tio_fini(tio_t *tio) {
	utils_list_node_t *node = tio->handles.first;
	while (node) {
		utils_list_node_t *next = node->next;
		tio_handle_release(TIO_CONTAINER_OF(node, tio_handle_t, node));
		node = next;
	}
}

void tio_quit(tio_t *tio, int exit_code) {
	if (tio->quit) return;
	tio->quit = TIO_TRUE;
	tio->exit_code = exit_code;
}
