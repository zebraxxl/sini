#include "sini_io.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <string.h>
#include <locale.h>
#include "sini_thread.h"

const char *Named = NULL;

char GlobalInputBuffer[4096] = "";
int GlobalInputBufferPosition = 0;
sini_locker LockInput;
sini_locker LockOutput;

void sini_printf( const char *str, ... )
{
	va_list lst;
	va_start( lst, str );

	sini_setlock( LockOutput );

	printf( "                                                                               \r" );
	vprintf( str, lst );
	if (GlobalInputBuffer[0])
	{
			printf( "%s %s\r", Named, GlobalInputBuffer );
	}else if (Named)
		printf( "%s\r", Named );

	sini_unsetlock( LockOutput );
}

void sini_namedread( const char *Name, char *Buffer, int Buffer_size, bool PrintNewLine )
{
	sini_setlock( LockInput );

	Named = Name;
	int key = 0;
	printf( "%s\r", Name );

	while (key != 13)
	{
		key = _getch();
		
		if (key == 0)
			continue;

		if (key == 8)	//backspace
		{
			if (!(GlobalInputBufferPosition))
				continue;

			GlobalInputBuffer[--GlobalInputBufferPosition] = 0;
			printf( "                                                                               \r" );
			printf( "%s %s\r", Name, GlobalInputBuffer );	
			continue;
		}

		GlobalInputBuffer[GlobalInputBufferPosition++] = key;
		GlobalInputBuffer[GlobalInputBufferPosition] = 0;

		printf( "%s %s\r", Name, GlobalInputBuffer );		

		if ((GlobalInputBufferPosition >= 4096)||(GlobalInputBufferPosition >= Buffer_size))
			break;
	}

	strcpy( Buffer, GlobalInputBuffer );
	Buffer[GlobalInputBufferPosition-1] = 0;

	GlobalInputBuffer[0] = 0;
	GlobalInputBufferPosition = 0;
	Named = NULL;
	if (PrintNewLine)
		printf( "\n" );

	sini_unsetlock( LockInput );
}

void sini_ioinit()
{
	LockOutput = sini_createlocker();
	LockInput = sini_createlocker();
}

void sini_passwordread( char *Buffer, int Buffer_size )
{
	sini_setlock( LockInput );

	Named = "Input password:";
	int key = 0;
	printf( "%s\r", Named );

	char InputBuffer[4096] = "";

	while (key != 13)
	{
		key = _getch();
		
		if (key == 0)
			continue;

		if (key == 8)	//backspace
		{
			if (!(GlobalInputBufferPosition))
				continue;

			GlobalInputBuffer[--GlobalInputBufferPosition] = 0;
			InputBuffer[GlobalInputBufferPosition] = 0;

			printf( "                                                                               \r" );
			printf( "%s %s\r", Named, GlobalInputBuffer );	
			continue;
		}

		InputBuffer[GlobalInputBufferPosition] = key;
		GlobalInputBuffer[GlobalInputBufferPosition++] = '*';

		GlobalInputBuffer[GlobalInputBufferPosition] = 0;
		InputBuffer[GlobalInputBufferPosition] = 0;

		printf( "%s %s\r", Named, GlobalInputBuffer );		

		if ((GlobalInputBufferPosition >= 4096)||(GlobalInputBufferPosition >= Buffer_size))
			break;
	}

	strcpy( Buffer, InputBuffer );
	Buffer[GlobalInputBufferPosition-1] = 0;

	GlobalInputBuffer[0] = 0;
	GlobalInputBufferPosition = 0;
	Named = NULL;
	printf( "\n" );

	sini_unsetlock( LockInput );
}
