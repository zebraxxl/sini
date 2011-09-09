#include "sini_client.h"
#include "sini_socket.h"
#include <string.h>
#include "sini_thread.h"
#include "sini_io.h"
#include "sini_config.h"
#include <zarray.h>
#include <stdio.h>

sini_socket ClientSocket = sini_socket_error;
unsigned int ID = 0;
sini_thread RecvThreadId = NULL;
unsigned int ServerIP = 0;

void PrintMessage( const char *From, const char *Message, unsigned short Time );

void RecvThread( void * )
{
	char Buffer[8192];

	while (true)
	{
		int i = sini_recv( ClientSocket, Buffer, 8192 );
		if ((i == 0)||(i == sini_error))
			return;

		if ((Buffer[0] == 'M')&&(Buffer[1] == 'S'))
		{
			char *Nick = &Buffer[2];

			int i = (int)strlen(Buffer)+1;
			char *Message = &Buffer[i];

			i = (int)strlen( &Buffer[i] ) + i + 1;

			PrintMessage( Nick, Message, *((unsigned short*)(&Buffer[i])) );
		}else if ((Buffer[0] == 'C')&&(Buffer[1] == 'S'))
		{
			sini_printf( "Chat God> Server was shurted down\n" );
			CloseClient();
			return;
		}else if ((Buffer[0] == 'U')&&(Buffer[1] == 'L'))
		{
			sini_printf( "On server:\n%s\r", &Buffer[2] );
		}
	}
}

void StartRecvThread()
{
	RecvThreadId = sini_beginthread( RecvThread );
}

bool CreateClient( const char *IPAddress, const char *Name )
{
	ServerIP = sini_parseip( IPAddress );
	if (!ServerIP)
		return false;

	ClientSocket = sini_createsocket();
	if (ClientSocket == sini_socket_error)
		return false;

	if (!sini_connect( ClientSocket, ServerIP, 27770 ))
	{
		sini_closesocket( ClientSocket );
		return false;
	}

	int NameLength = (int)strlen( Name );
	int BufferLength = NameLength + 1 + 2;
	char Buffer[1024];
	Buffer[0] = 'N';
	Buffer[1] = 'U';
	memcpy( &Buffer[2], Name, NameLength+1 );

	sini_send( ClientSocket, Buffer, BufferLength );

	sini_recv( ClientSocket, Buffer, 2 );
	if ((Buffer[0] == 'G')&&(Buffer[1] == 'P'))
	{
		sini_passwordread( Buffer, 1024 );
		sini_send( ClientSocket, Buffer, (int)strlen(Buffer)+1 );
		sini_recv( ClientSocket, Buffer, 2 );
	}
	if ((Buffer[0] != 'O')||(Buffer[1] != 'K'))
	{
		sini_closesocket( ClientSocket );
		return false;
	}

	int i = sini_recv( ClientSocket, (char*)&ID, sizeof(ID) );		//Getting ID
	if ((i == 0)||(i == sini_error))
	{
		sini_closesocket( ClientSocket );
		return false;
	}

	StartRecvThread();
	return true;
}

void CloseClient()
{
	sini_send( ClientSocket, "BC", 2 );
	sini_endthread( RecvThreadId );
	sini_closesocket( ClientSocket );
	ClientSocket = sini_socket_error;
}

void ClientSendTextMessage( const char *Msg )
{
	if (ClientSocket == sini_socket_error)
		return;
	char Buffer[4096];
	strcpy( Buffer, "MS" );
	strcat( Buffer, Msg );
	sini_send( ClientSocket, Buffer, (int)strlen(Buffer)+1 );
}

void ClientPrintList()
{
	sini_send( ClientSocket, "GL", 2 );
}

void PrintMessage( const char *From, const char *Message, unsigned short Time )
{
	int len = (int)strlen( Message );
	char *Buffer = new char[len+1];

	int m_pos = 0, b_pos = 0;

	while (m_pos < len)
	{
		if (Message[m_pos] == '\\')
		{
			m_pos++;
			if (Message[m_pos] == '\\')
				Buffer[b_pos++] = '\\';
			else if (Message[m_pos] == 'w')
				Buffer[b_pos++] = 2;
			else if (Message[m_pos] == 'b')
				Buffer[b_pos++] = 1;
			else if (Message[m_pos] == 'h')
				Buffer[b_pos++] = 3;
			else if (Message[m_pos] == 'r')
				Buffer[b_pos++] = 4;
			else if (Message[m_pos] == '+')
				Buffer[b_pos++] = 5;
			else if (Message[m_pos] == 'p')
				Buffer[b_pos++] = 6;
			else if (Message[m_pos] == 'm')
				Buffer[b_pos++] = 11;
			else if (Message[m_pos] == 'g')
				Buffer[b_pos++] = 12;
			else if (Message[m_pos] == 's')
				Buffer[b_pos++] = 14;
			else if (Message[m_pos] == '*')
				Buffer[b_pos++] = 15;

			m_pos++;
		}else{
			Buffer[b_pos++] = Message[m_pos++];
		}
	}
	Buffer[b_pos] = 0;

	if (GetShowTime())
		sini_printf( "[%02d:%02d] %s> %s\n", (Time&0xff00)>>8, Time&0xff, From, Buffer );
	else
		sini_printf( "%s> %s\n", From, Buffer );
}

void ClientChangeName( const char *NewName )
{
	char Buffer[8192] = "CN";
	strcat( Buffer, NewName );
	sini_send( ClientSocket, Buffer, (int)strlen(Buffer)+1 );
}

struct ServerInList
{
	char *Name;
	unsigned int IP;
};
CZArray<ServerInList> ServersList;

sini_socket answer_socket = sini_socket_error;
void ServerListRecvThread( void * )
{
	answer_socket = sini_createsocket( true );
	if (answer_socket == sini_socket_error)
		return;

	if (!sini_bind( answer_socket, 27772 ))
	{
		sini_closesocket( answer_socket );
		answer_socket = sini_socket_error;
		return;
	}

	char ServerName[256];
	while (true)
	{
		unsigned int IP = 0;
		unsigned short Port = 27772;

		int len = sini_recvfrom( answer_socket, ServerName, 256, &IP, &Port );

		ServerInList sil;
		sil.Name = new char[strlen(ServerName)+1];
		strcpy( sil.Name, ServerName );
		sil.IP = IP;

		ServersList.AddItem( sil );
	}
}

void PrintServerList()
{
	sini_thread th = sini_beginthread( ServerListRecvThread );

	sini_socket send_socket = sini_createsocket( true );
	sini_setbroadcast( send_socket, true );
	sini_sendto( send_socket, "GN", 3, sini_broadcast, 27771 );
	sini_closesocket( send_socket );

	sini_sleep( 3000 );

	sini_endthread( th );
	if (answer_socket != sini_socket_error)
		sini_closesocket( answer_socket );

	for (unsigned int i = 0; i < ServersList.get_count(); i++)
		sini_printf( "    [% 3d]: %s on %d.%d.%d.%d\n", i, ServersList[i].Name,
		(ServersList[i].IP&0xff000000)>>24, (ServersList[i].IP&0xff0000)>>16,
		(ServersList[i].IP&0xff00)>>8, ServersList[i].IP&0xff );
}

void CopyIPFromID( int i, char *IPBuffer )
{
	sprintf( IPBuffer, "%d.%d.%d.%d", (ServersList[i].IP&0xff000000)>>24, (ServersList[i].IP&0xff0000)>>16,
		(ServersList[i].IP&0xff00)>>8, ServersList[i].IP&0xff );
}

void ClearServersList()
{
	for (unsigned int i = 0; i < ServersList.get_count(); i++)
		delete [] ServersList[i].Name;
	ServersList.Clear();
	ServersList.~CZArray();
}
