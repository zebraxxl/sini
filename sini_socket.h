#ifndef __SINI_SOCKET_H__
#define __SINI_SOCKET_H__

typedef void *sini_socket;

#define sini_socket_error	((sini_socket)(~0))
#define sini_error			(-1)
#define sini_broadcast		(0xffffffff)

bool sini_sockinit();
bool sini_sockdelete();

sini_socket		sini_createsocket( bool UDP = false );
bool			sini_closesocket( sini_socket sock );
bool			sini_bind( sini_socket sock, unsigned short port );
bool			sini_listen( sini_socket sock );
void			sini_printips();
sini_socket		sini_accept( sini_socket sock );
sini_socket		sini_accept( sini_socket sock, unsigned int *ip_ret, unsigned short *prot_ret );
unsigned int	sini_parseip( const char *IP );
bool			sini_connect( sini_socket sock, unsigned int ip, unsigned short port );
int				sini_send( sini_socket sock, const char *Buffer, int Len );
int				sini_recv( sini_socket sock, char *Buffer, int Len );
bool			sini_setbroadcast( sini_socket sock, bool val );
int				sini_sendto( sini_socket sock, const void *Data, unsigned int DataLen, unsigned int ip, unsigned short port );
int				sini_recvfrom( sini_socket sock, void *Buffer, unsigned int BufferLen, unsigned int *retIP, unsigned short *retPort );

#endif	//__SINI_SOCKET_H__
