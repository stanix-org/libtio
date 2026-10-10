#ifndef TIO_STREAM_H
#define TIO_STREAM_H

#include "types.h"
#include "atomic.h"
#include "error.h"
#include "filehandle.h"

typedef struct tio_stream tio_stream_t;

struct tio_stream {
	TIO_ATOMIC(tio_size_t) ref_count;
	tio_handle_t *handle;
	char *buf;
	tio_size_t buf_size;
	char *write_base;
	char *write_pos;
	char *write_end;
	char *read_pos;
	char *read_end;
	tio_error_t error;
	int buf_type;
	tio_bool_t eof;
};

tio_ssize_t tio_stream_read(tio_stream_t *stream, void *buf, tio_size_t count);
tio_ssize_t tio_stream_write(tio_stream_t *stream, const void *buf, tio_size_t count);
tio_ssize_t tio_stream_write_string(tio_stream_t *stream, const char *string);
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
void tio_stream_write_i8(tio_stream_t *stream, int8_t i8);
static inline void tio_stream_write_u8(tio_stream_t *stream,uint8_t u8) {
	tio_stream_write_i8(stream, (int8_t)u8);
}
void tio_stream_write_i16(tio_stream_t *stream, int16_t i16);
static inline void tio_stream_write_u16(tio_stream_t *stream,uint16_t u16) {
	tio_stream_write_i16(stream, (int16_t)u16);
}
void tio_stream_write_i32(tio_stream_t *stream, int32_t i32);
static inline void tio_stream_write_u32(tio_stream_t *stream,uint32_t u32) {
	tio_stream_write_i32(stream, (int32_t)u32);
}
void tio_stream_write_i64(tio_stream_t *stream, int64_t i64);
static inline void tio_stream_write_u64(tio_stream_t *stream,uint64_t u64) {
	tio_stream_write_i64(stream, (int64_t)u64);
}

int tio_stream_seek(tio_stream_t *stream, tio_off_t offset, int whence);
tio_off_t tio_stream_tell(tio_stream_t *stream);
int tio_stream_flush(tio_stream_t *stream);

int tio_stream_from_handle(tio_stream_t **stream, tio_handle_t *handle);
int tio_stream_open(tio_t *tio, tio_stream_t **stream, const char *path, int flags, ...);
static inline tio_stream_t *tio_stream_ref(tio_stream_t *stream) {
	if (stream) TIO_ATOMIC_FETCH_ADD(&stream->ref_count, 1);
	return stream;
}
void tio_stream_release(tio_stream_t *stream);
tio_size_t tio_stream_pending(tio_stream_t *stream);
tio_size_t tio_stream_readahead(tio_stream_t *stream);

#define TIO_STREAM_NO_BUF   0
#define TIO_STREAM_LINE_BUF 1
#define TIO_STREAM_FULL_BUF 2

int tio_stream_set_buf(tio_stream_t *stream, int type, tio_size_t size);

void tio_stream_set_error(tio_stream_t *stream, tio_error_t error);

static inline void tio_stream_clear_error(tio_stream_t *stream) {
	tio_stream_set_error(stream, TIO_ERROR_SUCCESS);
}

tio_error_t tio_stream_get_error(tio_stream_t *stream);

static inline tio_bool_t tio_stream_have_error(tio_stream_t *stream) {
	return TIO_IS_ERROR(tio_stream_get_error(stream));
}

#endif
