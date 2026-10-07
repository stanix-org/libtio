#ifndef TIO_STREAM_H
#define TIO_STREAM_H

#include "types.h"
#include "error.h"
#include "handle.h"

typedef struct tio_stream tio_stream_t;

struct tio_stream {
	tio_handle_t *handle;
	tio_error_t error;
	tio_bool_t eof;
};

tio_ssize_t tio_stream_read(tio_stream_t *stream, void *buf, tio_size_t count);
tio_ssize_t tio_stream_write(tio_stream_t *stream, const void *buf, tio_size_t count);
int8_t tio_stream_read_i8(tio_stream_t *stream);
static inline uint8_t tio_stream_read_u8(tio_stream_t *stream) {
	return (uint8_t)tio_stream_read_i8(stream);
}
int16_t tio_stream_read_i16(tio_stream_t *stream);
static inline uint16_t tio_stream_read_u16(tio_stream_t *stream) {
	return (uint16_t)tio_stream_read_i16(stream);
}
int32_t tio_stream_read_i32(tio_stream_t *stream);
static inline uint32_t tio_stream_read_u32(tio_stream_t *stream) {
	return (uint32_t)tio_stream_read_i32(stream);
}
int64_t tio_stream_read_i64(tio_stream_t *stream);
static inline uint64_t tio_stream_read_u64(tio_stream_t *stream) {
	return (uint64_t)tio_stream_read_i64(stream);
}

int tio_stream_seek(tio_stream_t *stream, tio_off_t offset, int whence);
tio_off_t tio_stream_tell(tio_stream_t *stream);

#endif
