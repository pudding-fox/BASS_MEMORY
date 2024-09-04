#include <stdio.h>

#include "bass_memory.h"
#include "cache.h"
#include "memory_stream.h"
#include "reader.h"

//2.4.0.0
#define BASSDTSVERSION 0x02040000

//I have no idea how to prevent linking against this routine in msvcrt.
//It doesn't exist on Windows XP.
//Hopefully it doesn't do anything important.
int _except_handler4_common() {
	return 0;
}

static const BASS_PLUGININFO plugin_info = { BASSDTSVERSION, 0, NULL };

BOOL BASSMEMORYDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE)dll);
		if (HIWORD(BASS_GetVersion()) != BASSVERSION || !GetBassFunc()) {
			MessageBoxA(0, "Incorrect BASS.DLL version (" BASSVERSIONTEXT " is required)", "BASS", MB_ICONERROR | MB_OK);
			return FALSE;
		}
		break;
	}
	return TRUE;
}

const VOID* BASSMEMORYDEF(BASSplugin)(DWORD face) {
	switch (face) {
	case BASSPLUGIN_INFO:
		return (void*)&plugin_info;
	}
	return NULL;
}

HSTREAM BASSMEMORYDEF(BASS_MEMORY_StreamCreateFile)(BOOL mem, const void* file, QWORD offset, QWORD length, DWORD flags) {
	MEMORY_STREAM* stream;
	BUFFER* buffer;
	buffer = read_file_buffer((const wchar_t*)file, (size_t)offset, (size_t)length);
	if (buffer) {
		stream = memory_stream_create((const wchar_t*)file, buffer, &BASS_StreamCreateFileUser, flags);
		if (stream) {
			return stream->handle;
		}
	}
	return 0;
}

HSTREAM BASSMEMORYDEF(BASS_MEMORY_StreamCreate)(HSTREAM handle, QWORD offset, QWORD length, DWORD flags) {
	MEMORY_STREAM* stream;
	BUFFER* buffer;
	wchar_t file[MAX_PATH + 1];
	WAVE_HEADER wave_header;
	if (create_wave_header(handle, &wave_header)) {
		swprintf(file, L"%d.wav", abs(handle));
		buffer = read_stream_buffer((const wchar_t*)file, &wave_header, handle, (size_t)offset, (size_t)length);
		if (buffer) {
			stream = memory_stream_create((const wchar_t*)file, buffer, &BASS_StreamCreateFileUser, flags);
			if (stream) {
				return stream->handle;
			}
		}
	}
	return 0;
}

QWORD BASSMEMORYDEF(BASS_MEMORY_Usage)() {
	return cache_size();
}

VOID BASSMEMORYDEF(BASS_MEMORY_Progress)(VOID(*progress)(BASS_MEMORY_PROGRESS* progress)) {
	progress_handler(progress);
}