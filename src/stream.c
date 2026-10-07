#include <tio/stream.h>

tio_ssize_t tio_stream_read(tio_stream_t *stream, void *buf, tio_size_t count) {
}

tio_ssize_t tio_stream_write(tio_stream_t *stream, const void *buf, tio_size_t count) {
}

int8_t tio_stream_read_i8(tio_stream_t *stream) {
	int8_t i8 = 0;
	tio_stream_read(stream, &i8, sizeof(i8));
	return i8;
}

int16_t tio_stream_read_i16(tio_stream_t *stream) {
	int16_t i16 = 0;
	tio_stream_read(stream, &i16, sizeof(i16));
	return i16;
}

int32_t tio_stream_read_i32(tio_stream_t *stream) {
	int32_t i32 = 0;
	tio_stream_read(stream, &i32, sizeof(i32));
	return i32;
}

int64_t tio_stream_read_i64(tio_stream_t *stream) {
	int64_t i64 = 0;
	tio_stream_read(stream, &i64, sizeof(i64));
	return i64;
}
