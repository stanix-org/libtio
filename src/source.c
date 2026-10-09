#include <tio/source.h>
#include <tio/error.h>
#include <string.h>

tio_source_t *tio_source_init(tio_t *tio, tio_source_t *source, tio_source_ops_t *ops) {
	memset(source, 0, sizeof(tio_source_t));
	source->tio = tio;
	source->ops = ops;
	utils_list_append(&tio->sources, &source->node);
	return source;
}

int tio_source_start(tio_source_t *source, tio_callback_t callback, void *data) {
	if (!source->ops || !source->ops->start) return TIO_ERROR_NOT_SUPP;
	source->callback = callback;
	source->data = data;
	return source->ops->start(source);
}

int tio_source_stop(tio_source_t *source) {
	if (!source->ops || !source->ops->stop) return TIO_ERROR_NOT_SUPP;
	return source->ops->stop(source);
}

void tio_source_release(tio_source_t *source) {
	if (TIO_ATOMIC_FETCH_SUB(&source->ref_count, 1) > 1) return;
	utils_list_remove(&source->tio->sources, &source->node);
	if (!source->ops || !source->ops->cleanup) return;
	source->ops->cleanup(source);
}

void tio_source_dispatch(tio_source_t *source) {
	if (source->callback) {
		source->callback(source, source->data);
	}
}
