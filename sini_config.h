#ifndef __SINI_CONFIG_H__
#define __SINI_CONFIG_H__

void LoadConfig();

const char *GetNameFromConfig();
bool GetShowTime();

void SetNameInConfig( const char *NewName );
void SetShowTime( bool val );

#endif	//__SINI_CONFIG_H__
