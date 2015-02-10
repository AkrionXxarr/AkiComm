#include "Includes.h"

HANDLE initialized[3];

ClientInfo globalHead;
ClientInfo globalTail;
ClientInfo *pGlobalHead = &globalHead;
ClientInfo *pGlobalTail = &globalTail;
ClientInfo gClient;

int gClientID = 0;

unsigned int __stdcall ListenAccept(void *params);
unsigned int __stdcall ListenDelete(void *params);

unsigned int __stdcall ListenMain(void *params)
{
	ServerInfo *serverInfo = (ServerInfo*)params;
	ClientInfo client;
	ThreadParams tParams;

	initialized[initACCEPT] = CreateEvent(0, 0, 0, 0);
	initialized[initDELETE] = CreateEvent(0, 0, 0, 0);
	initialized[initCLIENT] = CreateEvent(0, 0, 0, 0);

	HANDLE acceptThread;
	HANDLE deleteThread;

	int iResult = 0;
	int ID = 1;

	client.ClientHandle = NULL;
	client.clientID = 0;
	client.ClientSocket = INVALID_SOCKET;
	client.pNext = 0;

	tParams.pClientInfo = &client;
	tParams.pServerInfo = serverInfo;

	InitClientLinkedList(pGlobalTail, pGlobalHead);

	SetEvent(lListInit);

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenMain Initialized" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	iResult = listen(serverInfo->ListenSocket, 100);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(serverInfo->ListenSocket);
		WSACleanup();
		return 1;
	}

	acceptThread = (HANDLE)_beginthreadex(0, 0, &ListenAccept, (void*)&tParams, 0, 0);
	deleteThread = (HANDLE)_beginthreadex(0, 0, &ListenDelete, (void*)&tParams, 0, 0);

	WaitForMultipleObjects(2, initialized, TRUE, INFINITE);

	EnterCriticalSection(&outputCritical);
	cout << endl << "Number connected: " << serverInfo->connected << endl << endl;
	LeaveCriticalSection(&outputCritical);

	while (1)
	{

		WaitForSingleObject(listenEvent, INFINITE);

		if (serverInfo->quit)
		{
			gClientIDQueue.push(-1);
			gMessageQueue.push("[SERVER] Shutting down...");
			break;
		}

		EnterCriticalSection(&clientCritical);
		serverInfo->connected++;
		client.clientID = ID;
		client.ClientHandle = (HANDLE)_beginthreadex(0, 0, &ClientThread, (void*)&tParams, 0, 0);
		ID++;
		LeaveCriticalSection(&clientCritical);
	}

	while (pGlobalTail->pNext != pGlobalHead)
	{
		Sleep(100);
	} 
	if (pGlobalTail->pNext == pGlobalHead)
	{
		EnterCriticalSection(&closeCritical);
		gClientID = 0;
		SetEvent(closeEvent);
		WaitForSingleObject(_closeEvent, INFINITE);
		ResetEvent(_closeEvent);
		ResetEvent(closeEvent);
		LeaveCriticalSection(&closeCritical);
	}

	WaitForSingleObject(deleteThread, INFINITE);

	CloseHandle(acceptThread);

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenAccept Closing" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	CloseHandle(deleteThread);

	CloseHandle(initialized[0]);
	CloseHandle(initialized[1]);
	CloseHandle(initialized[2]);

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenMain Closing" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	return 0;
}

unsigned int __stdcall ListenAccept(void *params)
{
	ThreadParams *tParams = (ThreadParams*)params;

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenAccept Initialized" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	SetEvent(initialized[initACCEPT]);

	EnterCriticalSection(&clientCritical);
	while (1)
	{
		ResetEvent(listenEvent);
		LeaveCriticalSection(&clientCritical);

		tParams->pClientInfo->ClientSocket = accept(tParams->pServerInfo->ListenSocket, NULL, NULL);
		if (tParams->pClientInfo->ClientSocket == INVALID_SOCKET)
		{
			cout << "accept failed with error: " << WSAGetLastError() << endl;
			closesocket(tParams->pServerInfo->ListenSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			EnterCriticalSection(&clientCritical);
			SetEvent(listenEvent);
		}
	}
	return 0;
}

unsigned int __stdcall ListenDelete(void *params)
{
	ThreadParams *tParams = (ThreadParams*)params;

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenDelete Initialized" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	string logoutMessage;

	SetEvent(initialized[initDELETE]);

	ClientInfo client;

	client.ClientHandle = NULL;
	client.clientID = 0;
	client.ClientSocket = INVALID_SOCKET;
	client.pNext = NULL;

	while (1)
	{
		if (tParams->pServerInfo->quit)
		{
			ResetEvent(shutdownEvent);
		}
		WaitForSingleObject(closeEvent, INFINITE);
		WaitForSingleObject(broadcastEvent, INFINITE);
		EnterCriticalSection(&broadcastCritical);
		if (!RemoveStruct(client, gClientID))
		{
			SetEvent(_closeEvent);
			break;
		}
		LeaveCriticalSection(&broadcastCritical);

		SetEvent(_closeEvent);
		WaitForSingleObject(client.ClientHandle, INFINITE);

		if (!tParams->pServerInfo->quit && client.isConnected)
		{
			logoutMessage = client.nickname;
			logoutMessage.append(" has disconnected.");
		}

		EnterCriticalSection(&outputCritical);
		cout << "Client disconnecting.." << endl;
		cout << "Client ID: " << client.clientID << endl;
		cout << "Client Socket: " << client.ClientSocket << endl;
		cout << "Client Nickname: " << client.nickname << endl;
		LeaveCriticalSection(&outputCritical);

		closesocket(client.ClientSocket);
		CloseHandle(client.ClientHandle);
		
		tParams->pServerInfo->connected--;

		EnterCriticalSection(&outputCritical);
		cout << endl << "Number connected: " << tParams->pServerInfo->connected << endl << endl;
		LeaveCriticalSection(&outputCritical);

		if (!tParams->pServerInfo->quit)
		{
			gClientIDQueue.push(-1);
			gMessageQueue.push(logoutMessage);
		}
		else
		{
			SetEvent(shutdownEvent);
		}
	}

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ListenDelete Closing" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	return 0;
}