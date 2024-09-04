#include "progress.h"
#include "bass_memory.h"

VOID(*_progress_handler_)(BASS_MEMORY_PROGRESS* progress) = NULL;

void progress_handler(VOID(*progress)(BASS_MEMORY_PROGRESS* progress)) {
	_progress_handler_ = progress;
}

void progress_begin(const wchar_t* const file) {
	progress_update(file, PROGRESS_BEGIN, PROGRESS_BEGIN, NULL);
}

void progress_update(const wchar_t* const file, const QWORD position, const QWORD length, BOOL* const cancel) {
	BASS_MEMORY_PROGRESS progress;
	if (!_progress_handler_) {
		return;
	}
	wcscpy((wchar_t*)progress.file, file);
	progress.position = position;
	progress.length = length;
	progress.cancel = FALSE;
	_progress_handler_(&progress);
	if (cancel) {
		*cancel = progress.cancel;
	}
}

void progress_end(const wchar_t* const file) {
	progress_update(file, PROGRESS_END, PROGRESS_END, NULL);
}
