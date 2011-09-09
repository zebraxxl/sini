#ifndef __SINI_THREAD_H__
#define __SINI_THREAD_H__

typedef void *sini_thread;
typedef void (*sini_threadstart)( void * );
typedef void *sini_locker;

sini_thread sini_beginthread( sini_threadstart ep, void *Param = 0 );
void sini_endthread( sini_thread thread );

sini_locker sini_createlocker();
void sini_setlock( sini_locker locker );
void sini_unsetlock( sini_locker locker );

void sini_sleep( unsigned int miliseconds );

#endif	//__SINI_THREAD_H__
