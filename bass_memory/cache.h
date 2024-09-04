#include "buffer.h"

BOOL cache_add(const wchar_t* const file, const BUFFER* const buffer);

BOOL cache_acquire(const wchar_t* const file, const BUFFER** buffer);

BOOL cache_release(const wchar_t* const file);

QWORD cache_size();