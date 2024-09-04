#include <stdio.h>
#include "reader.h"
#include "cache.h"
#include "progress.h"

#define BUFFER_BLOCK_SIZE 81920
#define BASS_ERROR 0xFFFFFFFF

size_t get_file_length(FILE* file_handle) {
	size_t length;
	if (fseek(file_handle, 0L, SEEK_END) != 0) {
#if _DEBUG
		char error[256];
		strerror_s(error, sizeof(error), errno);
		printf("Error seeking file: %s\n", error);
#endif
		return -1;
	}
	length = ftell(file_handle);
	if (fseek(file_handle, 0L, SEEK_SET) != 0) {
#if _DEBUG
		char error[256];
		strerror_s(error, sizeof(error), errno);
		printf("Error seeking file: %s\n", error);
#endif
		return -1;
	}
	return length;
}

BOOL populate_file_buffer(const wchar_t* const file, FILE* file_handle, size_t position, const BUFFER* const buffer) {
	size_t length;
	BOOL eof = FALSE;
	BOOL cancel = FALSE;
	BYTE* const file_buffer = malloc(BUFFER_BLOCK_SIZE);
	if (!file_buffer) {
#if _DEBUG
		printf("Could not allocate temp buffer.\n");
#endif
		return FALSE;
	}
	progress_begin(file);
	do {
		length = fread(file_buffer, sizeof(BYTE), BUFFER_BLOCK_SIZE, file_handle);
		if (ferror(file_handle)) {
#if _DEBUG
			char error[256];
			strerror_s(error, sizeof(error), errno);
			printf("Error reading file: %s\n", error);
#endif
			length = 0;
			eof = TRUE;
		}
		if (!length) {
			eof = TRUE;
		}
		if (position + length > buffer->length) {
#if _DEBUG
			printf("Buffer capacity exceeded.");
#endif
			length = (DWORD)(buffer->length - position);
			eof = TRUE;
		}
		if (length > 0) {
			buffer_write(buffer, position, length, file_buffer);
			position += length;
			progress_update(file, position, buffer->length, &cancel);
		}
		if (eof) {
			break;
		}
		if (cancel) {
			break;
		}
	} while (TRUE);
	free(file_buffer);
#if _DEBUG
	if (position < buffer->length) {
		printf("File ended before the buffer was populated.");
	}
#endif
	progress_end(file);
	if (cancel) {
		return FALSE;
	}
	return TRUE;
}

BUFFER* read_file_buffer(const wchar_t* const file, const size_t offset, const size_t length) {
	BUFFER* buffer;
	size_t file_length;
	FILE* file_handle;
	if (cache_acquire(file, &buffer)) {
		return buffer;
	}
	file_handle = _wfopen(file, L"rb");
	if (!file_handle) {
#if _DEBUG
		char error[256];
		strerror_s(error, sizeof(error), errno);
		printf("Error opening file: %s\n", error);
#endif
		return NULL;
	}
	file_length = get_file_length(file_handle);
	if (file_length == -1) {
#if _DEBUG
		printf("Could not determine file length.\n");
#endif
		buffer = NULL;
	}
	else {
		buffer = buffer_create(file_length);
		if (buffer) {
			if (!populate_file_buffer(file, file_handle, 0, buffer)) {
				buffer_free(buffer);
				buffer = NULL;
			}
		}
	}
	fclose(file_handle);
	if (buffer) {
		cache_add(file, buffer);
	}
	return buffer;
}

BOOL populate_stream_buffer(const wchar_t* const file, const HSTREAM handle, size_t position, const BUFFER* const buffer) {
	DWORD length;
	BOOL eof = FALSE;
	BOOL cancel = FALSE;
	BYTE* const stream_buffer = malloc(BUFFER_BLOCK_SIZE);
	if (!stream_buffer) {
#if _DEBUG
		printf("Could not allocate temp buffer.\n");
#endif
		return FALSE;
	}
	progress_begin(file);
	while (BASS_ChannelIsActive(handle) && !eof && !cancel) {
		length = BASS_ChannelGetData(handle, stream_buffer, BUFFER_BLOCK_SIZE);
		if (length == BASS_ERROR) {
#if _DEBUG
			printf("BASS_ChannelGetData error.");
#endif
			length = 0;
		}
		if ((length & BASS_STREAMPROC_END) == BASS_STREAMPROC_END) {
#if _DEBUG
			printf("Stream ended.");
#endif
			length &= ~BASS_STREAMPROC_END;
			eof = TRUE;
		}
		if (position + length > buffer->length) {
#if _DEBUG
			printf("Buffer capacity exceeded.");
#endif
			length = (DWORD)(buffer->length - position);
			eof = TRUE;
		}
		if (length > 0) {
			buffer_write(buffer, position, length, stream_buffer);
			position += length;
			progress_update(file, position, buffer->length, &cancel);
		}
	}
	free(stream_buffer);
#if _DEBUG
	if (position < buffer->length) {
		printf("Stream ended before the buffer was populated.");
	}
#endif
	progress_end(file);
	if (cancel) {
		return FALSE;
	}
	return TRUE;
}

BUFFER* read_stream_buffer(const wchar_t* const file, const WAVE_HEADER* const wave_header, const HSTREAM handle, const size_t offset, const size_t length) {
	BUFFER* buffer;
	size_t stream_length;
	if (cache_acquire(file, &buffer)) {
		return buffer;
	}
	stream_length = (size_t)BASS_ChannelGetLength(handle, BASS_POS_BYTE);
	if (stream_length == -1) {
#if _DEBUG
		printf("Could not determine stream length.\n");
#endif
		buffer = NULL;
	}
	else {
		buffer = buffer_create(sizeof(WAVE_HEADER) + stream_length);
		if (buffer) {
			buffer_write(buffer, 0, sizeof(WAVE_HEADER), (BYTE*)wave_header);
			if (!populate_stream_buffer(file, handle, sizeof(WAVE_HEADER), buffer)) {
				buffer_free(buffer);
				buffer = NULL;
			}
		}
	}
	if (buffer) {
		cache_add(file, buffer);
	}
	return buffer;
}