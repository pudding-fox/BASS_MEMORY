#include "buffer.h"

typedef struct {
	const wchar_t file[MAX_PATH + 1];
	size_t position;
	BUFFER* buffer;
	HSTREAM handle;
} MEMORY_STREAM;

typedef HSTREAM(CALLBACK MEMORY_STREAM_HANDLER)(DWORD system, DWORD flags, const BASS_FILEPROCS* proc, void* user);

MEMORY_STREAM* memory_stream_create(const wchar_t* const file, BUFFER* const buffer, MEMORY_STREAM_HANDLER* handler, const DWORD flags);

void memory_stream_free(MEMORY_STREAM* stream);