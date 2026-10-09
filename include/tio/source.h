#ifndef TIO_SOURCE_H
#define TIO_SOURCE_H

#include <libutils/list.h>
#include "ctx.h"
#include "types.h"
#include "atomic.h"

typedef struct tio_source_ops tio_source_ops_t;
typedef struct tio_source tio_source_t;

typedef void (*tio_callback_t)(tio_source_t *source, void *data);

struct tio_source_ops {
	int (*start)(tio_source_t *source);
	int (*stop)(tio_source_t *source);
	void (*cleanup)(tio_source_t *source);
};

struct tio_source {
	TIO_ATOMIC(size_t) ref_count;
	utils_list_node_t node;
	tio_source_ops_t *ops;
	tio_t *tio;
	tio_callback_t callback;
	void *data;
};

/**
 * @brief start receive events from this source
 * @param source the source to receive events from
 * @param callback the callback to trigger when the source fire
 * @param data user specified data to pass to the callback
 */
int tio_source_start(tio_source_t *source, tio_callback_t callback, void *data);

/**
 * @brief stop receive events from this source
 * @param source the source to stop receiving events from
 */
int tio_source_stop(tio_source_t *source);

tio_source_t *tio_source_init(tio_t *tio, tio_source_t *source, tio_source_ops_t *ops);

static inline tio_source_t *tio_source_ref(tio_source_t *source) {
	if (source) TIO_ATOMIC_FETCH_ADD(&source->ref_count, 1);
	return source;
}

void tio_source_release(tio_source_t *source);
void tio_source_dispatch(tio_source_t *source);

#endif
