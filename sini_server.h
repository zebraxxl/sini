#ifndef __SINI_SERVER_H__
#define __SINI_SERVER_H__

extern bool IsServer;

bool CreateServer();
void CloseServer();
void AddPassword( const char *Pass );
void PrintServerInfo();
void SetServerName( const char *Name );


#endif	//__SINI_SERVER_H__
