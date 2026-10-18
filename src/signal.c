#include <tio/signal.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <tio/macro.h>
#include <tio/platform.h>

static int tio_signal_start(tio_source_t *source) {
	tio_signal_t *signal = TIO_CONTAINER_OF(source, tio_signal_t, source);
	int ret = tio_platform_setup_signal_handler(source->tio, signal->signum);
	if (TIO_IS_ERROR(ret)) return ret;
	utils_list_append(&source->tio->signals, &signal->node);
	return TIO_ERROR_SUCCESS;
}

static int tio_signal_stop(tio_source_t *source) {
	tio_signal_t *signal = TIO_CONTAINER_OF(source, tio_signal_t, source);
	utils_list_remove(&source->tio->signals, &signal->node);
	return TIO_ERROR_SUCCESS;
}

static tio_source_ops_t tio_signal_ops = {
	.start = tio_signal_start,
	.stop  = tio_signal_stop,
};

int tio_signal_create(tio_t *tio, tio_source_t **source, int signum) {
	tio_signal_t *signal = tio_malloc(sizeof(tio_signal_t));
	if (!signal) return TIO_ERROR_NO_MEMORY;
	signal->signum = signum;
	*source = tio_source_init(tio, &signal->source, &tio_signal_ops);
	return TIO_ERROR_SUCCESS;
}
