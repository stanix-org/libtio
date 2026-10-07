#ifndef TIO_ERROR_H
#define TIO_ERROR_H

#include "types.h"

typedef int tio_error_t;

#define TIO_ERROR_SUCCESS    0
#define TIO_ERROR_IO        -1
#define TIO_ERROR_NOT_FOUND -2
#define TIO_ERROR_TIMEOUT   -3
#define TIO_ERROR_NOT_SUPP  -4
#define TIO_ERROR_NO_MEMORY -5
#define TIO_ERROR_INVALID   -6

#define TIO_IS_ERROR(x) ((x) < TIO_ERROR_SUCCESS)

#endif
