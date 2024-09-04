#include "../bass/bass.h"
#include "../bass/bass_addon.h"

#include "../bass_memory/progress.h"

#ifndef BASSMEMORYDEF
#define BASSMEMORYDEF(f) WINAPI f
#endif

BOOL BASSMEMORYDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved);

const VOID* BASSMEMORYDEF(BASSplugin)(DWORD face);

HSTREAM BASSMEMORYDEF(BASS_MEMORY_DSD_StreamCreateFile)(BOOL mem, const void* file, QWORD offset, QWORD length, DWORD flags);

QWORD BASSMEMORYDEF(BASS_MEMORY_DSD_Usage)();

VOID BASSMEMORYDEF(BASS_MEMORY_DSD_Progress)(VOID(*progress)(BASS_MEMORY_PROGRESS* progress));