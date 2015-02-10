#include "Includes.h"


unsigned int __stdcall ClientThread(void *params)
{
	EnterCriticalSection(&clientCritical);
	ThreadParams *tParams = (ThreadParams*)params;
	ClientInfo *client;

	ClientInfo *current;

	client = InsertStruct(tParams->pClientInfo);
	LeaveCriticalSection(&clientCritical);

	bool gotComma = false;
	char itoaBuf[33];
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iResult = 0;

	string message;
	string loginMessage;
	string userList;
	string clientName;
	string nickVersion;
	string::iterator iterator;

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ClientThread Initialized (ID: " << client->clientID << " | Handle: " << client->ClientHandle << ")" << endl << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	client->ClientVersion = "";

	for (int i = 0; i < DEFAULT_BUFLEN; i++)
	{
		recvbuf[i] = NULL;
	}

	iResult = recv(client->ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult < 0)
	{
		EnterCriticalSection(&outputCritical);
		cout << "nickname recv failed with error: " << WSAGetLastError() << endl;
		cout << "On ClientID: " << client->clientID << endl;
		LeaveCriticalSection(&outputCritical);
	}

	nickVersion.assign(recvbuf);
	for (iterator = nickVersion.begin(); iterator < nickVersion.end(); iterator++)
	{
		if (*iterator == ',')
		{
			iterator++;
			if (*iterator == ' ')
			{
				gotComma = true;
			}
			else
			{
				break;
			}
			continue;
		}

		if (gotComma)
		{
			client->ClientVersion.append(1, *iterator);
		}
		else
		{
			client->nickname.append(1, *iterator);
		}
	}

	if (!gotComma)
	{
		client->ClientVersion = "INVALID_VERSION";
	}

	loginMessage = client->nickname;
	loginMessage.append(" has connected.");

	EnterCriticalSection(&outputCritical);
	cout << "Client connecting.." << endl;
	cout << "Client ID: " << client->clientID << endl;
	cout << "Client Socket: " << client->ClientSocket << endl;
	cout << "Client Nickname: " << client->nickname << endl;;

	clientName = client->nickname;
	clientName.insert(0, "<");
	clientName.insert(clientName.size(), "> ");

	if (_stricmp(client->ClientVersion.c_str(), SERVER_VERSION) == 0)
	{
		client->isConnected = true;

		gClientIDQueue.push(-1);
		gMessageQueue.push(loginMessage);

		cout << endl << "Version check passed, Client connected." << endl;
		cout << endl << "Total connected: " << tParams->pServerInfo->connected << endl << endl;
		LeaveCriticalSection(&outputCritical);
		while (1)
		{
			for (int i = 0; i < DEFAULT_BUFLEN; i++)
			{
				recvbuf[i] = NULL;
			}

			iResult = recv(client->ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult < 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "message recv failed with error: " << WSAGetLastError() << endl;
				cout << "On ClientID: " << client->clientID << endl;
				LeaveCriticalSection(&outputCritical);
				break;
			}
			message.assign(recvbuf);
			iterator = message.begin();

			if (*iterator == '/')
			{
				if (_stricmp(message.c_str(), "/Quit") == 0)
				{
					iResult = send(client->ClientSocket, "/Quit", 6, 0);
					if (iResult < 0)
					{
						EnterCriticalSection(&outputCritical);
						cout << "quit confirm send failed with error: " << WSAGetLastError() << endl;
						cout << "On ClientID: " << client->clientID << endl;
						LeaveCriticalSection(&outputCritical);
						break;
					}
					break;
				}
				else if (_stricmp(message.c_str(), "/List") == 0)
				{
					userList = "[SERVER]\nUsers connected: ";
					_itoa_s (tParams->pServerInfo->connected, itoaBuf, 10);
					userList.append(itoaBuf);
					userList.append("\n");

					current = pGlobalTail;
					while (current->pNext != pGlobalHead)
					{
						current = current->pNext;

						userList.append(current->nickname);
						userList.append("\n");
					}
					iResult = send(client->ClientSocket, userList.c_str(), userList.size(), 0);
					if (iResult == SOCKET_ERROR)
					{
						EnterCriticalSection(&outputCritical);
						cout << "userList Send failed with error: " << WSAGetLastError() << endl;
						cout << "On Client ID: " << current->clientID << endl;
						LeaveCriticalSection(&outputCritical);
					}
					continue;
				}
				else if (_stricmp(message.c_str(), "/Beep") == 0)
				{
					gClientIDQueue.push(client->clientID);
					gMessageQueue.push("/Beep");
					message.insert(0, " used ");
					message.insert(0, client->nickname);
					message.insert(0, "[SERVER] ");
					message.append("\n");
					gClientIDQueue.push(-1);
					gMessageQueue.push(message);
					continue;
				}
				else
				{
					send(client->ClientSocket, "<Error> Command not recognized", 40, 0);
				}
			}

			message.insert(0, clientName);
			gClientIDQueue.push(client->clientID);
			gMessageQueue.push(message);
		}
	}
	else
	{
		Console.SetColorText(HalfRed);
		cout << endl << "<Client Error> Version mismatch for client " << client->nickname << endl;
		cout << "Server Version: " << SERVER_VERSION << " | Client Version: " << client->ClientVersion << endl << endl;
		Console.SetDefaultColor();
		LeaveCriticalSection(&outputCritical);

		message = "";
		message = "<Version Error> Server Version: ";
		message.append(SERVER_VERSION);
		message.append(" | Client Version: ");
		message.append(client->ClientVersion);

		WaitForSingleObject(broadcastEvent, INFINITE);
		EnterCriticalSection(&broadcastCritical);
		send(client->ClientSocket, message.c_str(), message.size(), 0);
		LeaveCriticalSection(&broadcastCritical);
		WaitForSingleObject(broadcastEvent, INFINITE);
		iResult = send(client->ClientSocket, "/Quit", 6, 0);
	}

	iResult = shutdown(client->ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown failed with error: " << WSAGetLastError() << endl;
	}

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ClientThread closing (ID: " << client->clientID << " | Handle: " << client->ClientHandle << ")" << endl << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	EnterCriticalSection(&closeCritical);
	gClientID = client->clientID;
	SetEvent(closeEvent);
	WaitForSingleObject(_closeEvent, INFINITE);
	ResetEvent(_closeEvent);
	ResetEvent(closeEvent);
	LeaveCriticalSection(&closeCritical);

	return 0;
}
