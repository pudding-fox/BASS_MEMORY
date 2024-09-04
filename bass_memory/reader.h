#include "buffer.h"
#include "wave_header.h"

BUFFER* read_file_buffer(const wchar_t* const file, const size_t offset, const size_t length);

BUFFER* read_stream_buffer(const wchar_t* const file, const WAVE_HEADER* const wave_header, const HSTREAM handle, const size_t offset, const size_t length);