#ifndef TIO_SIGNAL_H
#define TIO_SIGNAL_H

#include "source.h"
#include <libutils/list.h>
#include <signal.h>

typedef struct tio_signal {
	tio_source_t source;
	utils_list_node_t node;
	int signum;
} tio_signal_t;

int tio_signal_create(tio_t *tio, tio_source_t **source, int signum);

#endif
