#include <tio/ctx.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <tio/handle.h>
#include <tio/macro.h>
#include <string.h>

int tio_init(tio_t **tio) {
	*tio = tio_malloc(sizeof(tio_t));
	if (!*tio) return TIO_ERROR_NO_MEMORY;
	memset(*tio, 0, sizeof(tio_t));
	return TIO_ERROR_SUCCESS;
}

int tio_run(tio_t *tio) {
	while (!tio->quit) {
		// TODO : poll fds
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
