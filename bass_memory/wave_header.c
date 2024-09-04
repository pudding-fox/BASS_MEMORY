#include "wave_header.h"

BOOL create_wave_header(const HSTREAM handle, WAVE_HEADER* const header) {
	BASS_CHANNELINFO info;
	QWORD length;
	QWORD size;
	uint16_t format;
	uint16_t sample_size;
	if (!BASS_ChannelGetInfo(handle, &info)) {
		return FALSE;
	}
	length = BASS_ChannelGetLength(handle, BASS_POS_BYTE);
	if (length == -1) {
		return FALSE;
	}
	size = (sizeof(WAVE_HEADER) + length) - 8;
	if (size > UINT_MAX) {
		return FALSE;
	}
	if ((info.flags & BASS_SAMPLE_FLOAT) == BASS_SAMPLE_FLOAT) {
		format = PCM_FORMAT_FLOAT;
		sample_size = 32;
	}
	else {
		format = PCM_FORMAT_INT;
		sample_size = 16;
	}

	header->riff[0] = 'R';
	header->riff[1] = 'I';
	header->riff[2] = 'F';
	header->riff[3] = 'F';
	header->file_size = (uint32_t)size;
	header->wave[0] = 'W';
	header->wave[1] = 'A';
	header->wave[2] = 'V';
	header->wave[3] = 'E';
	header->fmt[0] = 'f';
	header->fmt[1] = 'm';
	header->fmt[2] = 't';
	header->fmt[3] = ' ';
	header->fmt_size = 16;
	header->format = format;
	header->channels = (uint16_t)info.chans;
	header->sample_rate = (uint32_t)info.freq;
	header->byte_rate = (uint32_t)((info.freq * sample_size * info.chans) / 8);
	header->block_alignment = (uint16_t)((sample_size * info.chans) / 8);
	header->depth = sample_size;
	header->data[0] = 'd';
	header->data[1] = 'a';
	header->data[2] = 't';
	header->data[3] = 'a';
	header->data_size = (uint32_t)length;

	return TRUE;
}