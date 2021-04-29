#ifndef UTILHDR
#define UTILHDR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef long long long64;

inline int random(int bounds) { return rand() % bounds; }

inline long64 timeNow() {
	FILETIME t1;
	GetSystemTimeAsFileTime(&t1);
	return (long64)t1.dwLowDateTime + ((long64)(t1.dwHighDateTime) << 32LL);
}

inline float deltaTime(long64 startTiming) { return (float)(timeNow() - startTiming) / 10000000; }

#endif