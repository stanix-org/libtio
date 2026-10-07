#ifndef TIO_CTX_H
#define TIO_CTX_H

#include "types.h"
#include <libutils/list.h>

typedef struct tio tio_t;

struct tio {
	utils_list_t handles;
	tio_bool_t quit;
	int exit_code;
};

int tio_init(tio_t **tio);
int tio_run(tio_t *tio);
void tio_fini(tio_t *tio);
void tio_quit(tio_t *tio, int exit_code);

#endif
