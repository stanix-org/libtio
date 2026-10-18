#ifndef TIO_TIMER_H
#define TIO_TIMER_H

#include "source.h"
#include <libutils/list.h>

typedef struct tio_timer tio_timer_t;

struct tio_timer {
	tio_source_t source;
	utils_list_node_t node;
	tio_time_t duration;
	tio_time_t trigger;
};

int tio_timer_create(tio_t *tio, tio_source_t **source, tio_time_t duration);
int tio_timer_create_from_trigger(tio_t *tio, tio_source_t **source, tio_time_t trigger);
void tio_timer_set_trigger(tio_source_t *source, tio_time_t trigger);
void tio_timer_reset(tio_source_t *source);

tio_time_t tio_timer_get_current_time(void);
tio_time_t tio_timer_get_next_trigger(tio_t *tio);
tio_time_t tio_timer_get_before_next_trigger(tio_t *tio);
void tio_timer_dispatch(tio_t *tio);

#endif
