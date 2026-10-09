#include <tio/poll.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <tio/macro.h>

static void tio_poll_cleanup(tio_source_t *source) {
	tio_poll_t *poll = TIO_CONTAINER_OF(source, tio_poll_t, source);
	tio_handle_release(poll->handle);
	tio_free(poll);
}

static int tio_poll_start(tio_source_t *source) {
	tio_poll_t *poll = TIO_CONTAINER_OF(source, tio_poll_t, source);
	tio_handle_t *handle = poll->handle;
	if (!handle->ops || !handle->ops->start_poll) return TIO_ERROR_NOT_SUPP;
	return handle->ops->start_poll(handle, poll);
}

static int tio_poll_stop(tio_source_t *source) {
	tio_poll_t *poll = TIO_CONTAINER_OF(source, tio_poll_t, source);
	tio_handle_t *handle = poll->handle;
	if (!handle->ops || !handle->ops->stop_poll) return TIO_ERROR_NOT_SUPP;
	return handle->ops->stop_poll(handle, poll);
}

static tio_source_ops_t tio_poll_ops = {
	.cleanup = tio_poll_cleanup,
	.start   = tio_poll_start,
	.stop    = tio_poll_stop,
};

int tio_poll_create(tio_source_t **source, tio_handle_t *handle, int flags) {
	tio_t *tio = handle->tio;
	tio_poll_t *poll = tio_malloc(sizeof(tio_poll_t));
	if (!poll) return TIO_ERROR_NO_MEMORY;
	poll->handle = tio_handle_ref(handle);
	poll->flags  = flags;
	*source = tio_source_init(tio, &poll->source, &tio_poll_ops);
	return 0;
}
