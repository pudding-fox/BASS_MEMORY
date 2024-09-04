#ifndef MEMORY_BUFFER_H
#define MEMORY_BUFFER_H

#include "../bass/bass.h"

typedef struct {
	BYTE* data;
	size_t length;
} BUFFER_SEGMENT;

typedef struct {
	BUFFER_SEGMENT* segments;
	size_t length;
} BUFFER;

BUFFER* buffer_create(const size_t length);

void buffer_read(const BUFFER* const buffer, size_t position, const size_t length, BYTE* const data);

void buffer_write(const BUFFER* const buffer, size_t position, const size_t length, const BYTE* const data);

void buffer_free(BUFFER* buffer);

#endif