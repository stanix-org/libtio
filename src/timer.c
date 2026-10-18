#include <tio/timer.h>
#include <tio/macro.h>
#include <tio/platform.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <string.h>

static void tio_timer_insert(tio_timer_t *timer) {
	if (timer->trigger == -1 || !timer->source.started) return;
	tio_t *tio = timer->source.tio;
	utils_list_node_t *prev = NULL;
	utils_list_foreach (node, &tio->timers) {
		tio_timer_t *current = TIO_CONTAINER_OF(node, tio_timer_t, node);
		if (current->trigger > timer->trigger) {
			break;
		}
		prev = node;
	}
	utils_list_add_after(&tio->timers, prev, &timer->node);
}

static int tio_timer_start(tio_source_t *source) {
	tio_timer_t *timer = TIO_CONTAINER_OF(source, tio_timer_t, source);
	if (timer->trigger == -1) {
		return TIO_ERROR_INVALID;
	}
	tio_timer_insert(timer);
	return TIO_ERROR_SUCCESS;
}

static int tio_timer_stop(tio_source_t *source) {
	tio_timer_t *timer = TIO_CONTAINER_OF(source, tio_timer_t, source);
	utils_list_remove(&source->tio->timers, &timer->node);
	return TIO_ERROR_SUCCESS;
}

static tio_source_ops_t tio_timer_ops = {
	.start = tio_timer_start,
	.stop  = tio_timer_stop,
};

int tio_timer_create(tio_t *tio, tio_source_t **source, tio_time_t duration) {
	tio_timer_t *timer = tio_malloc(sizeof(tio_timer_t));
	if (!timer) return TIO_ERROR_NO_MEMORY;
	memset(timer, 0, sizeof(tio_timer_t));
	tio_source_init(tio, &timer->source, &tio_timer_ops);
	timer->duration = duration;
	tio_timer_reset(&timer->source);
	*source = &timer->source;
	return TIO_ERROR_SUCCESS;
}

int tio_timer_create_from_trigger(tio_t *tio, tio_source_t **source, tio_time_t trigger) {
	tio_timer_t *timer = tio_malloc(sizeof(tio_timer_t));
	if (!timer) return TIO_ERROR_NO_MEMORY;
	memset(timer, 0, sizeof(tio_timer_t));
	tio_source_init(tio, &timer->source, &tio_timer_ops);
	timer->duration = -1;
	timer->trigger = trigger;
	*source = &timer->source;
	return TIO_ERROR_SUCCESS;
}

void tio_timer_set_trigger(tio_source_t *source, tio_time_t trigger) {
	tio_timer_t *timer = TIO_CONTAINER_OF(source, tio_timer_t, source);
	if (timer->trigger != -1 && timer->source.started) {
		// already in the list
		utils_list_remove(&source->tio->timers, &timer->node);
	}
	timer->trigger = trigger;
	tio_timer_insert(timer);
}

void tio_timer_reset(tio_source_t *source) {
	tio_timer_t *timer = TIO_CONTAINER_OF(source, tio_timer_t, source);
	if (timer->duration == -1) return;
	tio_timer_set_trigger(source, timer->duration + tio_timer_get_current_time());
}

tio_time_t tio_timer_get_current_time(void) {
	return tio_platform_get_current_time();
}

tio_time_t tio_timer_get_next_trigger(tio_t *tio) {
	if (utils_list_is_empty(&tio->timers)) {
		return -1;
	} else {
		tio_timer_t *timer = TIO_CONTAINER_OF(tio->timers.first, tio_timer_t, node);
		return timer->trigger;
	}
}

tio_time_t tio_timer_get_before_next_trigger(tio_t *tio) {
	tio_time_t next_trigger = tio_timer_get_next_trigger(tio);
	if (next_trigger != -1) {
		return next_trigger - tio_timer_get_current_time();
	} else {
		return -1;
	}
}

void tio_timer_dispatch(tio_t *tio) {
	while (!utils_list_is_empty(&tio->timers)) {
		tio_timer_t *timer = TIO_CONTAINER_OF(tio->timers.first, tio_timer_t, node);
		if (timer->trigger > tio_timer_get_current_time()) {
			break;
		}
		utils_list_remove(&tio->timers, &timer->node);
		timer->trigger = -1;
		tio_source_dispatch(&timer->source);
	}
}
