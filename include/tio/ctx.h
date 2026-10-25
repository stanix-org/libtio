#ifndef TIO_CTX_H
#define TIO_CTX_H

#include "types.h"
#include <libutils/list.h>

typedef struct tio tio_t;

struct tio {
	utils_list_t handles;
	utils_list_t sources;
	utils_list_t file_polls;
	utils_list_t timers;
	utils_list_t signals;
	const char *progname;
	void *private;
	int log_level;
	int exit_code;
	tio_bool_t quit;
};

int tio_init(tio_t **tio);
int tio_run(tio_t *tio);
void tio_fini(tio_t *tio);
void tio_quit(tio_t *tio, int exit_code);

#endif
