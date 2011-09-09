#include "sini_thread.h"
#include <windows.h>
#include <process.h>

sini_thread sini_beginthread( sini_threadstart ep, void *Param )
{
	return (sini_thread)_beginthread( ep, 0, Param );
}

void sini_endthread( sini_thread thread )
{
	TerminateThread( (HANDLE)thread, 0 );
}

sini_locker sini_createlocker()
{
	sini_locker ret = (sini_locker)new RTL_CRITICAL_SECTION;
	InitializeCriticalSection( (LPCRITICAL_SECTION)ret );
	return ret;
}

void sini_setlock( sini_locker locker )
{
	EnterCriticalSection( (LPCRITICAL_SECTION)locker );
}

void sini_unsetlock( sini_locker locker )
{
	LeaveCriticalSection( (LPCRITICAL_SECTION)locker );
}

void sini_sleep( unsigned int miliseconds )
{
	Sleep( miliseconds );
}
