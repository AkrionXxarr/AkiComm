#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_
#include "Includes.h"

#define SERVER_VERSION "1.07"

extern HANDLE broadcastEvent;
extern HANDLE shutdownEvent;
extern HANDLE listenEvent;
extern HANDLE closeEvent;
extern HANDLE _closeEvent;
extern HANDLE lListInit;
extern HANDLE initialized[];

extern CRITICAL_SECTION clientCritical;
extern CRITICAL_SECTION closeCritical;
extern CRITICAL_SECTION outputCritical;
extern CRITICAL_SECTION broadcastCritical;

extern struct ClientInfo *pGlobalHead;
extern struct ClientInfo *pGlobalTail;
extern struct ClientInfo globalHead;
extern struct ClientInfo globalTail;
extern struct ClientInfo gClient;

extern class ConsoleOperations Console;

extern int gClientID;
extern int disconnectDelay;

extern queue<string> gMessageQueue;
extern queue<int> gClientIDQueue;

enum initilizations
{
	initACCEPT,
	initDELETE,
	initCLIENT,
};

struct ClientInfo
{
	bool isConnected;
	HANDLE ClientHandle;
	SOCKET ClientSocket;
	int clientID;
	string nickname;
	string ClientVersion;

	ClientInfo *pNext;
};

struct ServerInfo
{
	bool quit;
	int connected;

	SOCKET ListenSocket;
};

struct ThreadParams
{
	ClientInfo *pClientInfo;
	ServerInfo *pServerInfo;
};

unsigned int __stdcall SendThread(void *params);
unsigned int __stdcall ListenMain(void *params);
unsigned int __stdcall ClientThread(void *params);
unsigned int __stdcall ServerInputThread(void *params);

void InitClientLinkedList(ClientInfo *pTail, ClientInfo *pHead);
ClientInfo *InsertStruct(ClientInfo *client);
bool RemoveStruct(ClientInfo &client, int clientID);

#endif