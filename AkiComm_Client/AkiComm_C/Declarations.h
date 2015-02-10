#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

#define CLIENT_VERSION "1.07"
#define CLIENT_VERSION_LEN 5

extern CRITICAL_SECTION outputCritical;
extern HANDLE quitEvent;
extern HANDLE recvInitialize;
extern HANDLE outputInitialize;

extern bool quit;
extern bool defaultColor;

extern class ConsoleOperations Console;


struct ThreadParams
{
	SOCKET ConnectSocket;

	string nickname;
	string sendMessage;
	string recvMessage;
	string clientVersion;
};

unsigned int __stdcall SendThread(void* params);
unsigned int __stdcall RecvThread(void* params);


#endif