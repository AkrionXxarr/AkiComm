#include "Includes.h"

unsigned int __stdcall ListenThreadAccept(void *params)
{
	ServerInfo *srvinfo = (ServerInfo*)params;

	EnterCriticalSection(&critClient);
	cout << "In ListenThreadAccept (LTA)" << endl;
	while (1)
	{
		ResetEvent(clientIncEvent);
		LeaveCriticalSection(&critClient);

		srvinfo->ClientSocket = accept(srvinfo->ListenSocket, NULL, NULL);
		if (srvinfo->ClientSocket == INVALID_SOCKET)
		{
			cout << "accept failed with error: " << WSAGetLastError() << endl;
			closesocket(srvinfo->ListenSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			EnterCriticalSection(&critClient);
			SetEvent(clientIncEvent);
		}
	}
	return 0;
}

unsigned int __stdcall ListenThreadInc(void *params)
{
	ServerInfo *srvinfo = (ServerInfo*)params;

	HANDLE acceptHandle;
	HANDLE decrimentHandle;
	int iResult;

	cout << "In ListenThreadInc (LTI)" << endl;

	iResult = listen(srvinfo->ListenSocket, 5);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(srvinfo->ListenSocket);
		WSACleanup();
		return 1;
	}

	acceptHandle = (HANDLE)_beginthreadex(0, 0, &ListenThreadAccept, params, 0, 0);
	decrimentHandle = (HANDLE)_beginthreadex(0, 0, &ListenThreadDec, params, 0, 0);

	while (1)
	{
		EnterCriticalSection(&critClient);
		cout << "Number Connected: " << srvinfo->connected << endl;
		LeaveCriticalSection(&critClient);

		WaitForSingleObject(clientIncEvent, INFINITE);

		if (srvinfo->quit)
		{
			break;
		}

		EnterCriticalSection(&critClient);
		srvinfo->clientHandles[srvinfo->connected] = (HANDLE)_beginthreadex(0, 0, &ClientThread, params, 0, 0);
		srvinfo->connected++;
		LeaveCriticalSection(&critClient);
	}

	CloseHandle(acceptHandle);

	cout << "Leaving ListenThreadInc (LTI)" << endl;
	return 0;
}

unsigned int __stdcall ListenThreadDec(void *params)
{
	ServerInfo *srvinfo = (ServerInfo*)params;

	EnterCriticalSection(&critClient);
	cout << "In ListenThreadDec (LTD)" << endl;
	LeaveCriticalSection(&critClient);

	while (1)
	{
		EnterCriticalSection(&critClient);
		LeaveCriticalSection(&critClient);

		WaitForSingleObject(clientDecEvent, INFINITE);
		EnterCriticalSection(&critClient);
		CloseHandle(srvinfo->clientHandles[srvinfo->connected]);
		srvinfo->connected--;
		cout << "Number Connected: " << srvinfo->connected << endl;
		LeaveCriticalSection(&critClient);
	}

	cout << "LEaving ListenThreadDec (LTD)" << endl;

	return 0;
}