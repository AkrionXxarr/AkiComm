#include "Includes.h"

unsigned int __stdcall SendThread(void* params)
{
	ThreadParams *tParams = (ThreadParams*)params;

	string nickname;
	string message;
	string nickVersionPass;

	int input = 0;
	int iResult = 0;
	
	nickVersionPass = tParams->nickname;
	nickVersionPass.append(", ");
	nickVersionPass.append(CLIENT_VERSION);

	WaitForSingleObject(recvInitialize, INFINITE);
	send(tParams->ConnectSocket, nickVersionPass.c_str(), nickVersionPass.size(), 0);
	while(!quit)
	{
		Sleep(10);
		if (_kbhit())
		{
			input = _getch();
		}

		if (input == 13)
		{
			input = 0;
			EnterCriticalSection(&outputCritical);
			message = "";
			Console.SetColorText(FullGreen);
			cout << "-> "; getline(cin, message);
			Console.SetDefaultColor();

			if (message.size() == 0)
			{
				LeaveCriticalSection(&outputCritical);
				continue;
			}

			if (_stricmp(message.c_str(), "/Help") == 0)
			{
				Console.SetColorText(HalfGreen);
				cout << "Command List:" << endl;
				cout << "/Beep - Send an audio alert to everyone." << endl;
				cout << "/CLS - Clear the console screen." << endl;
				cout << "/Help - Displays this list." << endl;
				cout << "/List - Get a list of currently connected users." << endl;
				cout << "/Quit - Disconnect and quit the application." << endl;
				cout << "----" << endl << endl;
				Console.SetDefaultColor();
				LeaveCriticalSection(&outputCritical);
				continue;
			}
			else if (_stricmp(message.c_str(), "/Cls") == 0)
			{
				Console.ClearScreen();
				LeaveCriticalSection(&outputCritical);
				continue;
			}

			iResult = send(tParams->ConnectSocket, message.c_str(), message.size(), 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "Send failed with error: " << WSAGetLastError() << endl;
				SetEvent(quitEvent);
				LeaveCriticalSection(&outputCritical);
				break;
			}

			if (_stricmp(message.c_str(), "/Quit") == 0)
			{
				recv(tParams->ConnectSocket, 0, 0, 0);
				SetEvent(quitEvent);
				LeaveCriticalSection(&outputCritical);
				break;
			}
			LeaveCriticalSection(&outputCritical);
		}
	}
	return 0;
}

unsigned int __stdcall OutputThread(void* params);
unsigned int __stdcall BeepThread(void* params);
unsigned int __stdcall RecvThread(void* params)
{
	ThreadParams *tParams = (ThreadParams*)params;
	ConsoleOperations Console;
	HANDLE outputHandle = NULL;
	HANDLE beepHandle = NULL;

	queue<string> messageQueue;

	char strBuffer[DEFAULT_BUFLEN];
	int recvBuflen = DEFAULT_BUFLEN;

	string message;

	int iResult;

	for (int i = 0; i < DEFAULT_BUFLEN; i++)
	{
		strBuffer[i] = NULL;
	}

	outputHandle = (HANDLE)_beginthreadex(0, 0, &OutputThread, (void*)&messageQueue, 0, 0);
	WaitForSingleObject(outputInitialize, INFINITE);
	SetEvent(recvInitialize);
	while (1)
	{
		for (int i = 0; i < DEFAULT_BUFLEN; i++)
		{
			strBuffer[i] = NULL;
		}

		iResult = recv(tParams->ConnectSocket, strBuffer, recvBuflen, 0);
		if (strBuffer[0] == '/')
		{
			if (iResult < 0 || _stricmp(strBuffer, "/Quit") == 0)
			{
				SetEvent(quitEvent);
				break;
			}
			else if (_stricmp(strBuffer, "/Beep") == 0)
			{
				CloseHandle(beepHandle);
				beepHandle = (HANDLE)_beginthreadex(0, 0, &BeepThread, (void*)0, 0, 0);
				continue;
			}
			else if (_stricmp(strBuffer, "/ServQuit") == 0)
			{
				send(tParams->ConnectSocket, "/Quit", 6, 0);
				continue;
			}
			else if (_stricmp(strBuffer, "/Kick") == 0)
			{
				send(tParams->ConnectSocket, "/Quit", 6, 0);
				continue;
			}
		}
		message.assign(strBuffer);
		messageQueue.push(message);
	}

	quit = true;
	WaitForSingleObject(outputHandle, INFINITE);
	CloseHandle(outputHandle);
	CloseHandle(beepHandle);

	return 0;
}

unsigned int __stdcall OutputThread(void* params)
{
	queue<string> *messageQueue = (queue<string>*)params;
	SetEvent(outputInitialize);
	while (!quit)
	{
		Sleep(100);
		while (!messageQueue->empty())
		{
			EnterCriticalSection(&outputCritical);
			cout << messageQueue->front() << endl;
			messageQueue->pop();
			LeaveCriticalSection(&outputCritical);
		}
	}
	return 0;
}

unsigned int __stdcall BeepThread(void* params)
{
	int beepCount;

	for (beepCount = 0; beepCount < 5; beepCount++)
	{
		Sleep(100);
		MessageBeep(0xFFFFFFFF);
	}

	return 0;
}