#ifndef TIO_LOG_H
#define TIO_LOG_H

#include "ctx.h"
#include <stdarg.h>

void tio_log(tio_t *tio, int level, const char *fmt, ...);
void tio_vlog(tio_t *tio, int level, const char *fmt, va_list args);

#define TIO_DEBUG    0
#define TIO_INFO     1
#define TIO_NOTE     2
#define TIO_WARNING  3
#define TIO_ERROR    4
#define TIO_CRITICAL 5
#define TIO_ALERT    6
#define TIO_PANIC    7

void tio_log_set_level(tio_t *tio, int level);
void tio_log_set_progname(tio_t *tio, const char *progname);

#endif
