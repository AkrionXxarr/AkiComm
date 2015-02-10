#include "Includes.h"

unsigned int __stdcall ClientThread(void *params)
{
	ServerInfo *srvinfo = (ServerInfo*)params;
	SOCKET ClientSocket = srvinfo->ClientSocket;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//int clientID = srvinfo->connected;

	for (int i = 0; i < DEFAULT_BUFLEN; i++)
	{
		recvbuf[i] = NULL;
	}

	int iResult = 0;
	int iSendResult = 0;

	EnterCriticalSection(&critClient);
	cout << "In ClientThread" << endl;
	LeaveCriticalSection(&critClient);

	do {

			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				printf("Bytes received: %d\n", iResult);
				cout << "String recieved: " << recvbuf << endl;

			// Echo the buffer back to the sender
				iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %s\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				printf("[ECHO] Bytes sent: %d\n", iSendResult);
				cout << "[ECHO] String sent: " << recvbuf << endl;
			}
			else if (iResult == 0)
				printf("Connection closing...\n");
			else  {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

		} while (iResult > 0);



	// srvinfo->quit = true;
	
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed with error: " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ClientSocket);

	cout << "Leaving ClientThread" << endl;
	
	EnterCriticalSection(&critClient);
	SetEvent(clientDecEvent);
	ResetEvent(clientDecEvent);
	LeaveCriticalSection(&critClient);

	return 0;
}