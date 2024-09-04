#ifdef _DEBUG
#include <stdio.h>
#endif

#include "../bass/bassdsd.h"

#include "bass_memory_dsd.h"
#include "../bass_memory/cache.h"
#include "../bass_memory/memory_stream.h"
#include "../bass_memory/reader.h"

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

HSTREAM BASSMEMORYDEF(_BASS_DSD_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS* procs, void* user) {
	return BASS_DSD_StreamCreateFileUser(system, flags, procs, user, 0);
}

HSTREAM BASSMEMORYDEF(BASS_MEMORY_DSD_StreamCreateFile)(BOOL mem, const void* file, QWORD offset, QWORD length, DWORD flags) {
	MEMORY_STREAM* stream;
	BUFFER* buffer;
	buffer = read_file_buffer((const wchar_t*)file, (size_t)offset, (size_t)length);
	if (buffer) {
		stream = memory_stream_create((const wchar_t*)file, buffer, &_BASS_DSD_StreamCreateFileUser, flags);
		if (stream) {
			return stream->handle;
		}
	}
	return 0;
}

QWORD BASSMEMORYDEF(BASS_MEMORY_DSD_Usage)() {
	return cache_size();
}

VOID BASSMEMORYDEF(BASS_MEMORY_DSD_Progress)(VOID(*progress)(BASS_MEMORY_PROGRESS* progress)) {
	progress_handler(progress);
}