#include "Includes.h"

queue<string> gMessageQueue;
queue<int> gClientIDQueue;

unsigned int __stdcall SendThread(void *params)
{
	bool *sendQuit = (bool*)params;
	WaitForSingleObject(lListInit, INFINITE);

	ClientInfo *pHead = pGlobalHead;
	ClientInfo *pTail = pGlobalTail;
	ClientInfo *current;

	int iResult;

	string message;

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "Send Thread Initialized" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	while (!*sendQuit)
	{
		Sleep(100);

		while (!gMessageQueue.empty())
		{
			current = pTail;
			ResetEvent(broadcastEvent);
			EnterCriticalSection(&broadcastCritical);
			while (current->pNext != pHead)
			{
				current = current->pNext;
				message = gMessageQueue.front();

				if (gClientIDQueue.front() != current->clientID || gClientIDQueue.front() == -1)
				{
					iResult = send(current->ClientSocket, message.c_str(), message.size(), 0);
					if (iResult == SOCKET_ERROR)
					{
						EnterCriticalSection(&outputCritical);
						cout << "Send failed with error: " << WSAGetLastError() << endl;
						cout << "On Client ID: " << current->clientID << endl;
						LeaveCriticalSection(&outputCritical);
					}
				}
			}
			LeaveCriticalSection(&broadcastCritical);
			SetEvent(broadcastEvent);
			gClientIDQueue.pop();
			gMessageQueue.pop();
		}
	}

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "SendThread Closing" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	return 0;
}
