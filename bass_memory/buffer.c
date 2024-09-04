#if _DEBUG
#include <stdio.h>
#endif

#include <math.h>
#include "buffer.h"

#define BUFFER_BLOCK_SIZE 10000000

void buffer_size(const size_t size, size_t* const segment_count) {
	*segment_count = (size_t)ceil(
		(double)size / BUFFER_BLOCK_SIZE
	);
}

void buffer_position(const size_t position, size_t* const segment_position, size_t* const segment_offset) {
	*segment_position = position / BUFFER_BLOCK_SIZE;
	*segment_offset = position % BUFFER_BLOCK_SIZE;
}

BOOL buffer_alloc(BUFFER* const buffer, const size_t size) {
	size_t remaining = size;
	size_t segment_position;
	size_t segment_count;
	size_t segment_length;
	buffer_size(size, &segment_count);
	buffer->segments = calloc(sizeof(BUFFER_SEGMENT), segment_count);
	if (!buffer->segments) {
#if _DEBUG
		printf("Could not allocate buffer segments.\n");
#endif
		return FALSE;
	}
	for (segment_position = 0; segment_position < segment_count; segment_position++) {
		if (remaining > BUFFER_BLOCK_SIZE) {
			segment_length = BUFFER_BLOCK_SIZE;
		}
		else {
			segment_length = remaining;
		}
		buffer->segments[segment_position].data = malloc(segment_length);
		if (!buffer->segments[segment_position].data) {
#if _DEBUG
			printf("Could not allocate buffer segment.\n");
#endif
			break;
		}
		buffer->segments[segment_position].length = segment_length;
		remaining -= segment_length;
	}
	if (remaining) {
#if _DEBUG
		printf("Buffer was partially allocated.\n");
#endif
		buffer->length = size - remaining;
		return FALSE;
	}
	buffer->length = size;
	return TRUE;
}

BUFFER* buffer_create(const size_t size) {
	BUFFER* const buffer = calloc(sizeof(BUFFER), 1);
	if (buffer) {
		if (!buffer_alloc(buffer, size)) {
			buffer_free(buffer);
			return NULL;
		}
	}
	return buffer;
}

size_t buffer_read_segment(const BUFFER* const buffer, const size_t position, const size_t length, size_t* const segment_position, size_t* const segment_offset, BYTE* const data) {
	size_t remaining = length - position;
	size_t segment_remaining = buffer->segments[*segment_position].length - *segment_offset;
	if (segment_remaining > remaining) {
		memcpy(
			(BYTE*)data + position,
			(BYTE*)buffer->segments[*segment_position].data + *segment_offset,
			remaining
		);
		*segment_offset += remaining;
		return remaining;
	}
	else {
		memcpy(
			(BYTE*)data + position,
			(BYTE*)buffer->segments[*segment_position].data + *segment_offset,
			segment_remaining
		);
		(*segment_position)++;
		*segment_offset = 0;
		return segment_remaining;
	}
}

void buffer_read(const BUFFER* const buffer, size_t position, const size_t length, BYTE* const data) {
	if (position + length > buffer->length) {
#if _DEBUG
		printf("Buffer capacity exceeded.");
#endif
		return;
	}
	size_t segment_position;
	size_t segment_offset;
	buffer_position(position, &segment_position, &segment_offset);
	for (position = 0; position < length; ) {
		position += buffer_read_segment(buffer, position, length, &segment_position, &segment_offset, data);
	}
}

size_t buffer_write_segment(const BUFFER* const buffer, const size_t position, const size_t length, size_t* const segment_position, size_t* const segment_offset, const BYTE* const data) {
	size_t remaining = length - position;
	size_t segment_remaining = buffer->segments[*segment_position].length - *segment_offset;
	if (segment_remaining > remaining) {
		memcpy(
			buffer->segments[*segment_position].data + *segment_offset,
			data + position,
			remaining
		);
		*segment_offset += remaining;
		return remaining;
	}
	else {
		memcpy(
			buffer->segments[*segment_position].data + *segment_offset,
			data + position,
			segment_remaining
		);
		(*segment_position)++;
		*segment_offset = 0;
		return segment_remaining;
	}
}

void buffer_write(const BUFFER* const buffer, size_t position, const size_t length, const BYTE* const data) {
	if (position + length > buffer->length) {
#if _DEBUG
		printf("Buffer capacity exceeded.");
#endif
		return;
	}
	size_t segment_position;
	size_t segment_offset;
	buffer_position(position, &segment_position, &segment_offset);
	for (position = 0; position < length; ) {
		position += buffer_write_segment(buffer, position, length, &segment_position, &segment_offset, data);
	}
}

void buffer_free(BUFFER* buffer) {
	size_t segment_position;
	size_t segment_count;
	BUFFER_SEGMENT segment;
	if (buffer) {
		if (buffer->segments) {
			buffer_size(buffer->length, &segment_count);
			for (segment_position = 0; segment_position < segment_count; segment_position++) {
				segment = buffer->segments[segment_position];
				if (segment.data) {
					free(segment.data);
					segment.data = NULL;
				}
			}
			free(buffer->segments);
			buffer->segments = NULL;
		}
		free(buffer);
		buffer = NULL;
	}
}