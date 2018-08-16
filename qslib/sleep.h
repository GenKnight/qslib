#ifndef _QSLIB_SLEEP_H_
#define _QSLIB_SLEEP_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace qs
{
	inline void sleep(unsigned long time)
	{
#ifdef _WIN32
		Sleep(time);
#else
		usleep(time * 1000);
#endif
	}
}

#endif
