#include <tio/stream.h>
#include <tio/memory.h>
#include <tio/error.h>
#include <stdarg.h>
#include <string.h>

tio_ssize_t tio_stream_read(tio_stream_t *stream, void *buffer, tio_size_t count) {
	if (tio_stream_have_error(stream)) return stream->error;
	if (stream->buf_type == TIO_STREAM_NO_BUF) {
		tio_ssize_t r = tio_handle_read(stream->handle, buffer, count);
		if (TIO_IS_ERROR(r)) tio_stream_set_error(stream, r);
		return r;
	}
	if (stream->write_pos) {
		int ret = tio_stream_flush(stream);
		if (TIO_IS_ERROR(ret)) return ret;
	}

	char *buf = buffer;
	tio_ssize_t total = 0;
	tio_ssize_t ret = 0;
	while (count > 0) {
		if (stream->read_pos >= stream->read_end) {
			ret = tio_handle_write(stream->handle, stream->buf, stream->buf_size);
			if (TIO_IS_ERROR(ret)) break;
			stream->read_pos = stream->buf;
			stream->read_end = stream->buf + ret;
			if (ret == 0) {
				stream->eof = TIO_TRUE;
				break;
			}
		}
		tio_size_t readahead = tio_stream_readahead(stream);
		tio_size_t chunk_size = count < readahead ? count : readahead;
		memcpy(buf, stream->read_pos, chunk_size);
		stream->read_pos += chunk_size;
		count -= chunk_size;
		buf += chunk_size;
		total += chunk_size;
	}
	if (total == 0 && TIO_IS_ERROR(ret)) return ret;
	return total;
}

tio_ssize_t tio_stream_write(tio_stream_t *stream, const void *buffer, tio_size_t count) {
	if (tio_stream_have_error(stream)) return stream->error;
	if (stream->buf_type == TIO_STREAM_NO_BUF) {
		tio_ssize_t w = tio_handle_write(stream->handle, buffer, count);
		if (TIO_IS_ERROR(w)) tio_stream_set_error(stream, w);
		return w;
	}
	if (stream->read_pos) {
		int ret = tio_stream_flush(stream);
		if (TIO_IS_ERROR(ret)) return ret;
	}

	// TODO : handle line buffering

	const char *buf = buffer;
	tio_ssize_t total = 0;
	while (count > 0) {
		if (stream->write_pos >= stream->write_end) {
			tio_size_t pending = tio_stream_pending(stream);
			if (pending > 0) {
				tio_ssize_t w = tio_handle_write(stream->handle, stream->write_base, pending);
				if (TIO_IS_ERROR(w)) return w;
			}
			stream->write_base = stream->write_pos = stream->buf;
			stream->write_end = stream->buf + stream->buf_size;
		}
		tio_size_t remaining = stream->write_end - stream->write_pos;
		tio_size_t chunk_size = count < remaining ? count : remaining;
		memcpy(stream->write_pos, buf, chunk_size);
		stream->write_pos += chunk_size;
		count -= chunk_size;
		buf += chunk_size;
		total += chunk_size;
	}
	return total;
}

tio_ssize_t tio_stream_write_string(tio_stream_t *stream, const char *string) {
	return tio_stream_write(stream, string, strlen(string));
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

void tio_stream_write_i8(tio_stream_t *stream, int8_t i8) {
	tio_stream_write(stream, &i8, sizeof(i8));
}

void tio_stream_write_i16(tio_stream_t *stream, int16_t i16) {
	tio_stream_write(stream, &i16, sizeof(i16));
}

void tio_stream_write_i32(tio_stream_t *stream, int32_t i32) {
	tio_stream_write(stream, &i32, sizeof(i32));
}

void tio_stream_write_i64(tio_stream_t *stream, int64_t i64) {
	tio_stream_write(stream, &i64, sizeof(i64));
}

int tio_stream_seek(tio_stream_t *stream, tio_off_t offset, int whence) {
	int ret = tio_stream_flush(stream);
	if (TIO_IS_ERROR(ret)) return ret;
	stream->eof = TIO_FALSE;
	return tio_handle_seek(stream->handle, offset, whence);
}

tio_off_t tio_stream_tell(tio_stream_t *stream) {
	tio_off_t ret = tio_handle_tell(stream->handle);
	if (TIO_IS_ERROR(ret)) return ret;
	return ret - tio_stream_readahead(stream) + tio_stream_pending(stream);
}

tio_size_t tio_stream_pending(tio_stream_t *stream) {
	return stream->write_pos - stream->write_base;
}

tio_size_t tio_stream_readahead(tio_stream_t *stream) {
	return stream->read_end - stream->read_pos;
}

int tio_stream_set_buf(tio_stream_t *stream, int type, tio_size_t size) {
	int ret = tio_stream_flush(stream);
	if (TIO_IS_ERROR(ret)) return ret;

	if (type == TIO_STREAM_NO_BUF) {
		size = 0;
	} else if (size == 0) {
		type = TIO_STREAM_NO_BUF;
	}

	if (stream->buf_size != size) {
		char *new_buf = tio_malloc(size);
		if (!new_buf) return TIO_ERROR_NO_MEMORY;
		tio_free(stream->buf);
		stream->buf = new_buf;
		stream->buf_size = size;
	}
	stream->buf_type = type;
	return TIO_ERROR_SUCCESS;
}

int tio_stream_flush(tio_stream_t *stream) {
	if (stream->write_pos) {
		// write pending data
		char *ptr = stream->write_base;
		while (ptr < stream->write_pos) {
			tio_ssize_t w = tio_handle_write(stream->handle, ptr, stream->write_pos - ptr);
			if (TIO_IS_ERROR(w)) {
				tio_stream_set_error(stream, w);
				return w;
			}
			if (w == 0) break;
			ptr += w;
		}
	} else if (stream->read_pos) {
		// seek back unread data
		int ret = tio_handle_seek(stream->handle, TIO_SEEK_CUR, -(tio_off_t)tio_stream_readahead(stream));
		if (TIO_IS_ERROR(ret)) {
			tio_stream_set_error(stream, ret);
			return ret;
		}
	}
	stream->write_base = stream->write_pos = stream->write_end = NULL;
	stream->read_pos = stream->read_end = NULL;
	return 0;
}

int tio_stream_from_handle(tio_stream_t **stream, tio_handle_t *handle) {
	tio_stream_t *new_stream = tio_malloc(sizeof(tio_stream_t));
	if (!new_stream) return TIO_ERROR_NO_MEMORY;
	memset(new_stream, 0, sizeof(tio_stream_t));
	new_stream->buf_type = TIO_STREAM_FULL_BUF;
	new_stream->buf_size = 4096;
	new_stream->buf = tio_malloc(new_stream->buf_size);
	if (!new_stream->buf) {
		tio_free(new_stream);
		return TIO_ERROR_NO_MEMORY;
	}
	new_stream->handle = tio_handle_ref(handle);
	*stream = new_stream;
	return TIO_ERROR_SUCCESS;
}

int tio_stream_open(tio_t *tio, tio_stream_t **stream, const char *path, int flags, ...) {
	tio_mode_t mode = 0;
	if (flags & TIO_OPEN_CREAT) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, tio_mode_t);
		va_end(args);
	}

	tio_handle_t *handle;
	int ret = tio_file_handle_open(tio, &handle, path, flags, mode);
	if (TIO_IS_ERROR(ret)) return ret;

	ret = tio_stream_from_handle(stream, handle);
	if (TIO_IS_ERROR(ret)) tio_handle_release(handle);
	return ret;
}

void tio_stream_release(tio_stream_t *stream) {
	if (TIO_ATOMIC_FETCH_SUB(&stream->ref_count, 1) > 1) return;
	tio_stream_flush(stream);
	tio_handle_release(stream->handle);
	tio_free(stream->buf);
	tio_free(stream);
}

void tio_stream_set_error(tio_stream_t *stream, tio_error_t error) {
	stream->error = error;
}

tio_error_t tio_stream_get_error(tio_stream_t *stream) {
	return stream->error;
}
