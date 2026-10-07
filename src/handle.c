#include <tio/handle.h>
#include <libutils/list.h>
#include <string.h>

tio_ssize_t tio_handle_read(tio_handle_t *handle, void *buf, tio_size_t count) {
	if (!handle->ops || !handle->ops->tell) return TIO_ERROR_NOT_SUPP;
	return handle->ops->read(handle, buf, count);
}

tio_ssize_t tio_handle_write(tio_handle_t *handle, const void *buf, tio_size_t count) {
	if (!handle->ops || !handle->ops->tell) return TIO_ERROR_NOT_SUPP;
	return handle->ops->write(handle, buf, count);
}

int tio_handle_seek(tio_handle_t *handle, tio_off_t offset, int whence) {
	if (!handle->ops || !handle->ops->tell) return TIO_ERROR_NOT_SUPP;
	return handle->ops->seek(handle, offset, whence);
}

tio_off_t tio_handle_tell(tio_handle_t *handle) {
	if (!handle->ops || !handle->ops->tell) return TIO_ERROR_NOT_SUPP;
	return handle->ops->tell(handle);
}

tio_handle_t *tio_handle_init(tio_t *tio, tio_handle_t *handle, tio_handle_ops_t *ops) {
	memset(handle, 0, sizeof(tio_handle_t));
	handle->ops = ops;
	handle->tio = tio;
	utils_list_append(&tio->handles, &handle->node);
	return handle;
}

void tio_handle_release(tio_handle_t *handle) {
	if (!handle) return;
	if (TIO_ATOMIC_FETCH_SUB(&handle->ref_count, 1) > 1) {
		return;
	}
	if (!handle->ops || !handle->ops->close) return;
	utils_list_remove(&handle->tio->handles, &handle->node);
	handle->ops->close(handle);
}
