#ifndef TIO_PLATFORM_H
#define TIO_PLATFORM_H

#include "types.h"
#include "ctx.h"

int tio_platform_init(tio_t *tio);
void tio_platform_fini(tio_t *tio);
int tio_platform_open(tio_intptr_t *fd, const char *path, int flags, tio_mode_t mode);
tio_ssize_t tio_platform_read(tio_intptr_t fd, void *buf, tio_size_t count);
tio_ssize_t tio_platform_write(tio_intptr_t fd, const void *buf, tio_size_t count);
tio_off_t tio_platform_seek(tio_intptr_t fd, tio_off_t offset, int whence);
void tio_platform_close(tio_intptr_t fd);
int tio_platform_poll(tio_t *tio);
tio_time_t tio_platform_get_current_time(void);
int tio_platform_setup_signal_handler(tio_t *tio, int signum);

#endif
