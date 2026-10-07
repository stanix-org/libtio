#ifndef TIO_ATOMIC_H
#define TIO_ATOMIC_H

#if !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)

#include <stdatomic.h>
#define TIO_ATOMIC(type) _Atomic(type)
#define TIO_ATOMIC_FETCH_ADD(ptr, x) atomic_fetch_add((ptr), (x))
#define TIO_ATOMIC_FETCH_SUB(ptr, x) atomic_fetch_sub((ptr), (x))
#define TIO_ATOMIC_FETCH_AND(ptr, x) atomic_fetch_and((ptr), (x))
#define TIO_ATOMIC_FETCH_OR(ptr, x)  atomic_fetch_or((ptr), (x))
#define TIO_ATOMIC_STORE(ptr, x)     atomic_store((ptr), (x))
#define TIO_ATOMIC_LOAD(ptr, x)      atomic_load(ptr)

#elif defined(__cplusplus) && __cplusplus >= 201103L

#include <atomic.h>
#define TIO_ATOMIC(type) std::atomic<type>
#define TIO_ATOMIC_FETCH_ADD(ptr, x) (ptr)->fetch_add(x)
#define TIO_ATOMIC_FETCH_SUB(ptr, x) (ptr)->fetch_sub(x)
#define TIO_ATOMIC_FETCH_AND(ptr, x) (ptr)->fetch_and(x)
#define TIO_ATOMIC_FETCH_OR(ptr, x)  (ptr)->fetch_or(x)
#define TIO_ATOMIC_STORE(ptr, x)     (ptr)->store(x)
#define TIO_ATOMIC_LOAD(ptr, x)      (ptr)->load()

#elif defined(__GNUC__) || defined(__clang__)

#define TIO_ATOMIC(type) volatile type
#define TIO_ATOMIC_FETCH_ADD(ptr, x) __sync_fetch_and_add((ptr), (x))
#define TIO_ATOMIC_FETCH_SUB(ptr, x) __sync_fetch_and_sub((ptr), (x))
#define TIO_ATOMIC_FETCH_AND(ptr, x) __sync_fetch_and_and((ptr), (x))
#define TIO_ATOMIC_FETCH_OR(ptr, x)  __sync_fetch_and_or((ptr), (x))
#define TIO_ATOMIC_STORE(ptr, x)     __sync_store((ptr), (x))
#define TIO_ATOMIC_LOAD(ptr, x)      __sync_load(ptr)

#else
#error "no atomics found"
#endif

#endif
