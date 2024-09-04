#include <stdint.h>
#include "../bass/bass.h"

#define PCM_FORMAT_INT 1

#define PCM_FORMAT_FLOAT 3

typedef struct {
	uint8_t riff[4];
	uint32_t file_size;
	uint8_t wave[4];
	uint8_t fmt[4];
	uint32_t fmt_size;
	uint16_t format;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_alignment;
	uint16_t depth;
	uint8_t data[4];
	uint32_t data_size;
} WAVE_HEADER;

BOOL create_wave_header(const HSTREAM handle, WAVE_HEADER* const header);