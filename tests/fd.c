#include <tio/tio.h>
#include <unistd.h>
#include <string.h>

int main() {
	tio_handle_t *handle;
	if (TIO_IS_ERROR(tio_fd_handle_create(&handle, STDOUT_FILENO, false))) {
		return 1;
	}
	const char *string = "hello world!\n";
	tio_handle_write(handle, string, strlen(string));
}
