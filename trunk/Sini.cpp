#include <stdio.h>
#include "sini_io.h"
#include <string.h>
#include "sini_server.h"
#include "sini_socket.h"
#include "sini_client.h"
#include "sini_config.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	char Name[256];
	char IPBuffer[16];

	if (!sini_sockinit())
	{
		printf( "Error while initializing Sockets\n" );
		return 0;
	}

	sini_ioinit();
	sini_printf( "Welcome to Sini 1.6 from ***Zebr@XXL***!!!\n" );
	sini_printf( "  Enter \".help\" to view list of all commands\n" );

	LoadConfig();
	const char *InCongigName = GetNameFromConfig();

	if (InCongigName)
		strcpy( Name, InCongigName );
	else
	{
		sini_namedread( "Input your name:", Name, 256 );
		SetNameInConfig( Name );
	}

	if (strcmp( Name, "***Zebr@XXL***" ) == 0) sini_printf( "Wow. My creator with me!!!\n" );
	sini_namedread( "Input server IP, .list, or blank for server creation:", IPBuffer, 16 );

	if (strcmp( IPBuffer, ".list" ) == 0)
	{
		PrintServerList();
		sini_namedread( "Input server IP or id, or blank for server creation:", IPBuffer, 16 );
		if (IPBuffer[0] != 0)
		{
			if (sini_parseip( IPBuffer ) == 0)
			{
				int id = atoi( IPBuffer );
				CopyIPFromID( id, IPBuffer );
			}
		}
	}

	ClearServersList();

	if (IPBuffer[0] == 0)
	{
		strcpy( IPBuffer, "127.0.0.1" );

		if (!CreateServer())
		{
			sini_printf( "Error while creating server\n" );
			sini_sockdelete();
			return 0;
		}
	}

	if (!CreateClient( IPBuffer, Name ))
	{
		sini_printf( "Error while connecting to server\n" );
		CloseServer();
		sini_sockdelete();
		return 0;
	}

	bool Running = true;
	char MessageBuffer[2048];

	while (Running)
	{
		sini_namedread( ">", MessageBuffer, 2048, false );
		if (MessageBuffer[0] == '.')
		{
			if (strcmp( &MessageBuffer[1], "exit" ) == 0)
			{
				Running = false;
				continue;
			}else if (strcmp( &MessageBuffer[1], "list" ) == 0)
			{
				ClientPrintList();
				continue;
			}else if (memcmp( &MessageBuffer[1], "name", 4 ) == 0)
			{
				int pos = 0;
				while (MessageBuffer[pos] != ' ') pos++;
				while (MessageBuffer[pos] == ' ') pos++;
				ClientChangeName( &MessageBuffer[pos] );
				SetNameInConfig( &MessageBuffer[pos] );
				continue;
			}else if ((memcmp( &MessageBuffer[1], "password", 4 ) == 0)
				||(memcmp( &MessageBuffer[1], "pass", 4 ) == 0))
			{
				int pos = 0;
				while (MessageBuffer[pos] != ' ') pos++;
				while (MessageBuffer[pos] == ' ') pos++;
				AddPassword( &MessageBuffer[pos] );
				continue;
			}else if (strcmp( &MessageBuffer[1], "help" ) == 0)
			{
				sini_printf( "Sini help:\n");
				sini_printf( "    .exit            - exit from Sini\n");
				sini_printf( "    .list            - show who online\n");
				sini_printf( "    .name [new_name] - change name to [new_name]\n");
				sini_printf( "    .pass [password] - set server password to [password]\n");
				sini_printf( "    .help            - show this message\n");
				sini_printf( "    .showtime        - show time when message was sended\n");
				sini_printf( "    .hidetime        - hide time when message was sended\n");
				sini_printf( "    .info            - show information about online peoples\n");
				sini_printf( "Special symbols:\n");
				sini_printf( "    \\\\  = '\\'\n");
				sini_printf( "    \\w  = '\x02'\n");
				sini_printf( "    \\b  = '\x01'\n");
				sini_printf( "    \\h  = '\x03'\n");
				sini_printf( "    \\r  = '\x04'\n");
				sini_printf( "    \\+  = '\x05'\n");
				sini_printf( "    \\p  = '\x06'\n");
				sini_printf( "    \\m  = '\x0b'\n");
				sini_printf( "    \\g  = '\x0c'\n");
				sini_printf( "    \\s  = '\x0e'\n");
				sini_printf( "    \\*  = '\x0f'\n");
				continue;
			}else if (strcmp( &MessageBuffer[1], "showtime" ) == 0)
			{
				SetShowTime( true );
				continue;
			}else if (strcmp( &MessageBuffer[1], "hidetime" ) == 0)
			{
				SetShowTime( false );
				continue;
			}else if (strcmp( &MessageBuffer[1], "info" ) == 0)
			{
				PrintServerInfo();
				continue;
			}else if (memcmp( &MessageBuffer[1], "serv_name", 4 ) == 0)
			{
				int pos = 0;
				while (MessageBuffer[pos] != ' ') pos++;
				while (MessageBuffer[pos] == ' ') pos++;
				SetServerName( &MessageBuffer[pos] );
				continue;
			}
		}
		ClientSendTextMessage( MessageBuffer );
	}

	CloseClient();
	CloseServer();
	sini_sockdelete();
	return 0;
}

