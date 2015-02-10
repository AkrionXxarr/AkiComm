#include "Includes.h"

HANDLE broadcastEvent;
HANDLE shutdownEvent;
HANDLE listenEvent;
HANDLE closeEvent;
HANDLE _closeEvent;
HANDLE lListInit;

CRITICAL_SECTION clientCritical;
CRITICAL_SECTION closeCritical;
CRITICAL_SECTION outputCritical;
CRITICAL_SECTION broadcastCritical;

ConsoleOperations Console;

int disconnectDelay = 0;

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	addrinfo *result = NULL;
	addrinfo hints;
	ServerInfo serverInfo;

	HANDLE myHandles[3];

	bool sendQuit = false;

	broadcastEvent = CreateEvent(0, TRUE, TRUE, 0);
	shutdownEvent = CreateEvent(0, 0, 0, 0);
	listenEvent = CreateEvent(0, 0, 0, 0);
	closeEvent = CreateEvent(0, 0, 0, 0);
	_closeEvent = CreateEvent(0, 0, 0, 0);
	lListInit = CreateEvent(0, 0, 0, 0);

	int iResult;

	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " server-port" << endl;
		return 1;
	}

	InitializeCriticalSection(&clientCritical);
	InitializeCriticalSection(&closeCritical);
	InitializeCriticalSection(&outputCritical);
	InitializeCriticalSection(&broadcastCritical);

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Initialize ServerInfo struct
	serverInfo.connected = 0;
	serverInfo.ListenSocket = INVALID_SOCKET;
	serverInfo.quit = false;


	// Resolve the server address and port
	iResult = getaddrinfo(NULL, argv[1], &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	serverInfo.ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverInfo.ListenSocket == INVALID_SOCKET)
	{
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(serverInfo.ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(serverInfo.ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << endl;
	cout << "=== AkiComm Server (v" << SERVER_VERSION << ") ===" << endl;
	cout << "- Press Enter to use the input line, Enter again to send." << endl;
	cout << "- Type /Help to see a list of commands." << endl << endl;

	freeaddrinfo(result);

	myHandles[0] = (HANDLE)_beginthreadex(0, 0, &ListenMain, (void*)&serverInfo, 0, 0);
	myHandles[1] = (HANDLE)_beginthreadex(0, 0, &SendThread, (void*)&sendQuit, 0, 0);
	myHandles[2] = (HANDLE)_beginthreadex(0, 0, &ServerInputThread, (void*)&serverInfo, 0, 0);

	WaitForSingleObject(myHandles[2], INFINITE);
	WaitForSingleObject(myHandles[0], INFINITE);
	sendQuit = true;
	WaitForSingleObject(myHandles[1], INFINITE);

	DeleteCriticalSection(&clientCritical);
	DeleteCriticalSection(&closeCritical);
	DeleteCriticalSection(&outputCritical);
	DeleteCriticalSection(&broadcastCritical);

	CloseHandle(myHandles[0]);
	CloseHandle(myHandles[1]);
	CloseHandle(myHandles[2]);

	CloseHandle(broadcastEvent);
	CloseHandle(shutdownEvent);
	CloseHandle(closeEvent);
	CloseHandle(_closeEvent);
	CloseHandle(lListInit);

	cout << endl << "------- Process End -------" << endl << endl;

	return 0;
}