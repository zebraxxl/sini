#include "sini_server.h"
#include "sini_socket.h"
#include "sini_thread.h"
#include "sini_io.h"
#include <string.h>
#include <zlist.h>
#include <zstring.h>
#include <time.h>

bool IsServer = false;
sini_socket ServerSocket = sini_socket_error;
sini_thread MainThread;
char *Password = NULL;
char ServerName[256] = "unnamed";

void SendTextMessage( const char *From, const char *Message );
void RemoveID( unsigned int id );
void SendSCToAll();

class Connection;
void ConnectionRecvWrapper( void *p );
CZList<Connection*> ConnectionsList;

sini_thread StartMessageRecv(const Connection *_this)
{
	return sini_beginthread( ConnectionRecvWrapper, (void*)_this );
}

class Connection
{
private:
	sini_socket Socket;
	char *Name;
	unsigned int ID;
	sini_thread RecvThread;
	unsigned int ClientIP;

public:
	inline unsigned int getID()const{return ID;}
	inline const char *getName()const{return Name;}
	inline unsigned int getIP()const{return ClientIP;}


	Connection( sini_socket ConnectionSocket, char *NickName, unsigned int id, unsigned int IP = 0 )
	{
		Socket = ConnectionSocket;
		ID = id;
		Name = new char[strlen(NickName) + 1];
		strcpy( Name, NickName );
		sini_send( ConnectionSocket, (char*)&id, sizeof(id) );
		RecvThread = StartMessageRecv( this );
		ClientIP = IP;
	}

	~Connection()
	{
		sini_endthread( RecvThread );
		sini_closesocket( Socket );
		delete [] Name;
	}

	void OnRecv()
	{
		char Buffer[4096];
		while (true)
		{
			int i = sini_recv( Socket, Buffer, 4096 );
			if ((i == 0)||(i == sini_error))
				break;
			if ((Buffer[0] == 'M')&&(Buffer[1] == 'S'))
			{
				::SendTextMessage( Name, &Buffer[2] );
			}else if ((Buffer[0] == 'B')&&(Buffer[1] == 'C'))
			{
				RemoveID( ID );
				return;
			}else if ((Buffer[0] == 'G')&&(Buffer[1] == 'L'))
			{
				CZStringA Answer = "UL    ";
				for (CZList<Connection*>::ListIteraror i = ConnectionsList.get_begin(); (!i.IsNULL()); i++)
				{
					Answer += (*i)->getName();
					Answer += "\n    ";
				}
				sini_send( Socket, Answer.get_ptr(), Answer.get_length()+1 );
			}else if ((Buffer[0] == 'C')&&(Buffer[1] == 'N'))
			{
				CZStringA Message = CZStringA( Name );
				Message += " changed name to ";

				delete [] Name;
				Name = new char[strlen(&Buffer[2])+1];
				strcpy( Name, &Buffer[2] );
				Message += Name;

				::SendTextMessage( "Chat God", Message.get_ptr() );
			}
		}
		RemoveID( ID );
	}

	void SendTextMessage( const char *From, const char *Message, unsigned short Time = 0x011E )
	{
		int BufferLen = (int)strlen(From) + (int)strlen(Message) + 6;
		char *Buffer = new char[BufferLen];
		Buffer[0] = 'M';
		Buffer[1] = 'S';
		Buffer[2] = 0;

		strcat( Buffer, From );
		int i = (int)strlen( Buffer ) + 1;
		strcpy( &Buffer[i], Message );

		i = (int)strlen( &Buffer[i] ) + i + 1;
		(*((unsigned short*)(&Buffer[i]))) = Time;

		sini_send( Socket, Buffer, BufferLen );
		delete [] Buffer;
	}

	void SendCS()
	{
		sini_send( Socket, "CS", 2 );
	}
};



void ConnectionRecvWrapper( void *p )
{
	((Connection*)p)->OnRecv();
}

unsigned int NextFreeID = 0;

void CreateNewServerThread();
void ListenThread(void *)
{
	unsigned int IP;
	unsigned short Port;

	sini_socket NewSocket = sini_accept( ServerSocket, &IP, &Port );
	if (NewSocket != sini_socket_error)
		CreateNewServerThread();

	char Buffer[1024];
	sini_recv( NewSocket, Buffer, 1024 );
	if ((Buffer[0] != 'N')&&(Buffer[1] != 'U'))
	{
		sini_closesocket( NewSocket );
		return;
	}

	if (Password)
	{
		char PasswordBuffer[4096];
		sini_send( NewSocket, "GP", 2 );
		sini_recv( NewSocket, PasswordBuffer, 4096 );
		if (strcmp( PasswordBuffer, Password ))
		{
			sini_send( NewSocket, "ER", 2 );
			sini_closesocket( NewSocket );
			return;
		}
	}

	sini_send( NewSocket, "OK", 2 );

	ConnectionsList.AddToEnd( new Connection( NewSocket, &Buffer[2], NextFreeID++, IP ) );
	strcpy( Buffer, &Buffer[2] );
	strcat( Buffer, " with us" );
	SendTextMessage( "Chat God", Buffer );
}

void CreateNewServerThread()
{
	MainThread = sini_beginthread( ListenThread );
}

sini_thread GetServerListThread;
void OnGetServerList( void * )
{
	sini_socket answersocket = sini_createsocket( true );
	if (!sini_bind( answersocket, 27771 ))
	{
		sini_closesocket( answersocket );
		return;
	}

	unsigned int IP;
	unsigned short port;

	while (true)
	{
		char Buffer[3];
		if (sini_recvfrom( answersocket, Buffer, 3, &IP, &port ) == 3)
			if ((Buffer[0] == 'G')&&(Buffer[1] == 'N'))
				sini_sendto( answersocket, ServerName, (int)strlen(ServerName)+1, IP, 27772 );
	}
}

bool CreateServer()
{
	ServerName[255] = 0;
	IsServer = true;
	ServerSocket = sini_createsocket();
	if (ServerSocket == sini_socket_error)
		return false;

	if (!sini_bind( ServerSocket, 27770 ))
	{
		sini_closesocket( ServerSocket );
		return false;
	}

	if (!sini_listen( ServerSocket ))
	{
		sini_closesocket( ServerSocket );
		return false;
	}

	sini_printf( "IP adresses to connect to server: " );
	sini_printips();

	CreateNewServerThread();

	GetServerListThread = sini_beginthread( OnGetServerList );
	return true;
}

void CloseServer()
{
	SendSCToAll();
	sini_endthread( GetServerListThread );
	sini_endthread( MainThread );
	sini_closesocket( ServerSocket );

	if (Password)
		delete [] Password;
}

void SendTextMessage( const char *From, const char *Message )
{
	time_t t;
	time( &t );
	
	struct tm *CurrTime = localtime( &t );

	for (CZList<Connection*>::ListIteraror i = ConnectionsList.get_begin(); (!i.IsNULL()); i++)
		(*i)->SendTextMessage( From, Message, (((CurrTime->tm_hour&0xff)<<8)|(CurrTime->tm_min&0xff)) );
}

void RemoveID( unsigned int id )
{
	CZList<Connection*>::ListIteraror i = ConnectionsList.get_begin();
	for (; (!i.IsNULL()); i++)
		if ((*i)->getID() == id)
			break;

	if (i.IsNULL())
		return;
	Connection* c = (*i);
	ConnectionsList.Delete( i );

	char Buffer[1024];
	strcpy( Buffer, c->getName() );
	strcat( Buffer, " goes from here" );

	SendTextMessage( "Chat God", Buffer );
	delete c;
}

void SendSCToAll()
{
	for (CZList<Connection*>::ListIteraror i = ConnectionsList.get_begin(); (!i.IsNULL()); i++)
	{
		(*i)->SendCS();
		delete (*i);
	}
}

void AddPassword( const char *Pass )
{
	if (!IsServer)
		return;

	if (Password)
		delete [] Password;

	int Len = (int)strlen( Pass )+1;
	Password = new char[Len];
	strcpy( Password, Pass );
	sini_printf( "Server> Password added\n" );
}

void PrintServerInfo()
{
	if (!IsServer)
		return;

	for (CZList<Connection*>::ListIteraror i = ConnectionsList.get_begin(); (!i.IsNULL()); i++)
	{
		unsigned int IP = (*i)->getIP();

		sini_printf( "    %s on %d.%d.%d.%d\n", (*i)->getName(), (IP&0xff000000)>>24, (IP&0xff0000)>>16,
			(IP&0xff00)>>8, IP&0xff );
	}
}

#ifndef min
#  define min(a,b) (((a)>(b))?(b):(a))
#endif

void SetServerName( const char *Name )
{
	memcpy( ServerName, Name, min(255, strlen( Name )) );
}
