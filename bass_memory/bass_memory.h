#include "../bass/bass.h"
#include "../bass/bass_addon.h"

#include "progress.h"

#ifndef BASSMEMORYDEF
#define BASSMEMORYDEF(f) WINAPI f
#endif

BOOL BASSMEMORYDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved);

const VOID* BASSMEMORYDEF(BASSplugin)(DWORD face);

HSTREAM BASSMEMORYDEF(BASS_MEMORY_StreamCreateFile)(BOOL mem, const void* file, QWORD offset, QWORD length, DWORD flags);

HSTREAM BASSMEMORYDEF(BASS_MEMORY_StreamCreate)(HSTREAM handle, QWORD offset, QWORD length, DWORD flags);

QWORD BASSMEMORYDEF(BASS_MEMORY_Usage)();

VOID BASSMEMORYDEF(BASS_MEMORY_Progress)(VOID(*progress)(BASS_MEMORY_PROGRESS* progress));