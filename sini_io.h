#ifndef __SINI_IO_H__
#define __SINI_IO_H__

void sini_printf( const char *str, ... );
void sini_namedread( const char *Name, char *Buffer, int Buffer_size, bool PrintNewLine = true );
void sini_passwordread( char *Buffer, int Buffer_size );
void sini_ioinit();

#endif	//__SINI_IO_H__
