#include <tio/log.h>
#include <stdio.h>

void tio_log(tio_t *tio, int level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	tio_vlog(tio, level, fmt, args);
	va_end(args);
}

static const char *tio_level2str(int level) {
	switch (level) {
	case TIO_DEBUG:
		return "debug";
	case TIO_INFO:
		return "info";
	case TIO_NOTE:
		return "note";
	case TIO_WARNING:
		return "warning";
	case TIO_ERROR:
		return "error";
	case TIO_CRITICAL:
		return "critical";
	case TIO_ALERT:
		return "alert";
	case TIO_PANIC:
		return "panic";
	default:
		return "unknown";
	}
}

void tio_vlog(tio_t *tio, int level, const char *fmt, va_list args) {
	if (level < tio->log_level) return;
	fprintf(stderr, "%s : %s : ", tio->progname, tio_level2str(level));
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
}

void tio_log_set_level(tio_t *tio, int level) {
	tio->log_level = level;
}

void tio_log_set_progname(tio_t *tio, const char *progname) {
	tio->progname = progname;
}
