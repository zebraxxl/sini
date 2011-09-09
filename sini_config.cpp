#include "sini_config.h"
#include <stdio.h>
#include <string.h>

char NameBuffer[256] = "";
bool ShowTime = false;

bool ConfigFileIsExist = false;

void InitEmptyFile();
void RewriteFile();

void LoadConfig()
{
	FILE *f = fopen( "config.txt", "r" );
	if (!f)
		return;
	
	ConfigFileIsExist = true;
	
	int file_char = getc( f );
	if (file_char == EOF)
	{
		fclose( f );
		InitEmptyFile();
		return;
	}else
		fseek( f, 0, SEEK_SET );

	int NameBufferPos = 0;

	while ((file_char != EOF)&&(file_char != '\n')&&(file_char != '\r'))
	{
		file_char = getc( f );
		if ((file_char == '\n')||(file_char == '\r'))
			continue;

		NameBuffer[NameBufferPos++] = file_char&0xff;
		if (NameBufferPos >= 255)
			break;
	}
	NameBuffer[NameBufferPos] = 0;
	while ((file_char != EOF)&&(file_char != '\n')&&(file_char != '\r')) file_char = getc( f );

	if (fscanf( f, "%d", &NameBufferPos ) >= 1)
		ShowTime = (NameBufferPos?true:false);

	fclose( f );
}

const char *GetNameFromConfig()
{
	if (NameBuffer[0])
		return NameBuffer;
	else
		return NULL;
}

void SetNameInConfig( const char *NewName )
{
	strcpy( NameBuffer, NewName );
	RewriteFile();
}

void InitEmptyFile()
{
	FILE *f = fopen( "config.txt", "w" );

	fprintf( f, "\n0\n" );

	fclose( f );
}

bool GetShowTime()
{
	return ShowTime;
}

void SetShowTime( bool val )
{
	ShowTime = val;
	RewriteFile();
}

void RewriteFile()
{
	if (ConfigFileIsExist)
	{
		FILE *f = fopen( "config.txt", "w" );

		fprintf( f, "%s\n", NameBuffer );
		fprintf( f, "%d\n", (int)ShowTime );

		fclose( f );
	}
}
