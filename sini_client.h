#ifndef __SINI_CLIENT_H__
#define __SINI_CLIENT_H__

bool CreateClient( const char *IPAddress, const char *Name );
void CloseClient();
void ClientSendTextMessage( const char *Msg );
void ClientPrintList();
void ClientChangeName( const char *NewName );
void PrintServerList();
void CopyIPFromID( int ID, char *IPBuffer );
void ClearServersList();

#endif	//__SINI_CLIENT_H__
