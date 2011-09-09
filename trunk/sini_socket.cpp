#include "sini_socket.h"
#include "sini_io.h"
#include <string.h>
#include <stdio.h>

#ifdef WIN32
#  include <Winsock2.h>
#endif

bool sini_sockinit()
{
	WSAData data;
	return (WSAStartup( 0x0202, &data ) == 0);
}

bool sini_sockdelete()
{
	return (WSACleanup() == 0);
}

sini_socket sini_createsocket( bool UDP )
{
	if (!UDP)
		return (sini_socket)(socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ));
	else
		return (sini_socket)(socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ));
}

bool sini_bind( sini_socket sock, unsigned short port )
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ADDR_ANY;
	addr.sin_port = htons( port );

	return (bind( (SOCKET)sock, (sockaddr*)&addr, sizeof(addr) ) == 0);
}

bool sini_closesocket( sini_socket sock )
{
	return (closesocket( (SOCKET)sock ) == 0);
}

bool sini_listen( sini_socket sock )
{
	return (listen( (SOCKET)sock, SOMAXCONN ) == 0);
}

void sini_printips()
{
	char Buffer[1024];
	if (gethostname( Buffer, 1024 ))
	{
		sini_printf( "error\n" );
		return;
	}

	hostent *h = gethostbyname( Buffer );
	if (!h)
	{
		sini_printf( "error\n" );
		return;
	}

	int pos = 0;
	while (h->h_addr_list[pos])
	{
		sini_printf( "    %d.%d.%d.%d\n", (h->h_addr_list[pos][0]&0xff), (h->h_addr_list[pos][1]&0xff),
			(h->h_addr_list[pos][2]&0xff), (h->h_addr_list[pos][3]&0xff) );
		pos++;
	}	
}

sini_socket sini_accept( sini_socket sock )
{
	return (sini_socket)accept( (SOCKET)sock, NULL, NULL );
}

sini_socket	sini_accept( sini_socket sock, unsigned int *ip_ret, unsigned short *prot_ret )
{
	sockaddr_in addr;
	int len = sizeof(addr);

	SOCKET ret = accept( (SOCKET)sock, (sockaddr*)&addr, &len );

	*ip_ret = ntohl( addr.sin_addr.s_addr );
	*prot_ret = ntohs( addr.sin_port );
	return (sini_socket)ret;
}

unsigned int sini_parseip( const char *IP )
{
	unsigned int Oct1, Oct2, Oct3, Oct4;
	if (sscanf( IP, "%d.%d.%d.%d", &Oct1, &Oct2, &Oct3, &Oct4 ) != 4)
		return 0;
	return ((Oct1 << 24)|(Oct2 << 16)|(Oct3 << 8)|(Oct4));
}

bool sini_connect( sini_socket sock, unsigned int ip, unsigned short port )
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( ip );
	addr.sin_port = htons( port );

	return (connect( (SOCKET)sock, (sockaddr*)&addr, sizeof(addr) ) == 0);
}

int sini_send( sini_socket sock, const char *Buffer, int Len )
{
	return send( (SOCKET)sock, Buffer, Len, 0 );
}

int sini_recv( sini_socket sock, char *Buffer, int Len )
{
	return recv( (SOCKET)sock, Buffer, Len, 0 );
}

bool sini_setbroadcast( sini_socket sock, bool val )
{
	int i = (val)?1:0;

	return (setsockopt( (SOCKET)sock, SOL_SOCKET, SO_BROADCAST, (char*)&i, sizeof(i) ) == 0);
}

int	sini_sendto( sini_socket sock, const void *Data, unsigned int DataLen, unsigned int ip, unsigned short port )
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( ip );
	addr.sin_port = htons( port );

	return sendto( (SOCKET)sock, (const char*)Data, (int)DataLen, 0, (sockaddr*)&addr, sizeof(addr) );
}

int sini_recvfrom( sini_socket sock, void *Buffer, unsigned int BufferLen, unsigned int *retIP, unsigned short *retPort )
{
	sockaddr_in addr;
	int len = sizeof(addr);

	int ret = recvfrom( (SOCKET)sock, (char*)Buffer,(int)BufferLen, 0, (sockaddr*)&addr, &len );

	*retIP = ntohl( addr.sin_addr.s_addr );
	*retPort = ntohs( addr.sin_port );
	return ret;
}