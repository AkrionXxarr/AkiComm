#include "Includes.h"

CRITICAL_SECTION outputCritical;

HANDLE quitEvent;
HANDLE recvInitialize;
HANDLE outputInitialize;

bool quit = false;
bool defaultColor = true;

ConsoleOperations Console;

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	addrinfo *result = NULL;
	addrinfo *ptr = NULL;
	addrinfo hints;

	string nickname;
	string message;

	ThreadParams tParams;
	tParams.ConnectSocket = INVALID_SOCKET;

	HANDLE sendHandle;
	HANDLE recvHandle;

	int iResult;

	if (argc != 4)
	{
		cout << "Usage: " << argv[0] << " server-name server-port nickname" << endl;
		return 1;
	}
	else
	{
		tParams.nickname = argv[3];
	}

	quitEvent = CreateEvent(0, 0, 0, 0);
	recvInitialize = CreateEvent(0, 0, 0, 0);
	outputInitialize = CreateEvent(0, 0, 0, 0);

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		WSACleanup();
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		tParams.ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (tParams.ConnectSocket == INVALID_SOCKET)
		{
			cout << "socket failed with error: " << WSAGetLastError();
			WSACleanup();
			return 1;
		}

		iResult = connect(tParams.ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(tParams.ConnectSocket);
			tParams.ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (tParams.ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unable to connect to server." << endl;
		WSACleanup();
		return 1;
	}

	cout << endl;
	cout << "=== AkiComm Client (v" << CLIENT_VERSION << ") ===" << endl;
	cout << "- Press Enter to use the input line, Enter again to send." << endl;
	cout << "- Type /Help to see a list of commands." << endl;

	InitializeCriticalSection(&outputCritical);

	cout << endl;
	sendHandle = (HANDLE)_beginthreadex(0, 0, &SendThread, (void*)&tParams, 0, 0);
	recvHandle = (HANDLE)_beginthreadex(0, 0, &RecvThread, (void*)&tParams, 0, 0);
	

	WaitForSingleObject(quitEvent, INFINITE);

	WaitForSingleObject(sendHandle, INFINITE);
	WaitForSingleObject(recvHandle, INFINITE);

	closesocket(tParams.ConnectSocket);

	CloseHandle(sendHandle);
	CloseHandle(recvHandle);

	WSACleanup();
	DeleteCriticalSection(&outputCritical);

	return 0;
}