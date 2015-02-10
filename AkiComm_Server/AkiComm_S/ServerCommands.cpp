#include "Includes.h"

unsigned int __stdcall ServerShutdown(void *params);
unsigned int __stdcall ServerInputThread(void *params)
{
	ServerInfo *serverInfo = (ServerInfo*)params;
	ClientInfo *current;

	HANDLE shutdownHandle;

	bool foundID = false;

	int intID;
	int input;

	string message;
	string chID;
	string::iterator iterator;

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ServerInputThread Initialized" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	while (!serverInfo->quit)
	{
		input = _getch();

		if (input == 13)
		{
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

			iterator = message.begin();

			if (*iterator == '/')
			{
				if (_stricmp(message.c_str(), "/CLS") == 0)
				{
					Console.ClearScreen();
					LeaveCriticalSection(&outputCritical);
					continue;
				}
				else if (_stricmp(message.c_str(), "/Details") == 0)
				{
					chID = "";
					Console.SetColorText(FullGreen);
					cout << "User ID number: "; getline(cin, chID);
					Console.SetDefaultColor();
					intID = atoi(chID.c_str());

					current = pGlobalTail;
					while (current->pNext != pGlobalHead)
					{
						current = current->pNext;

						if (current->clientID == intID)
						{
							foundID = true;
							break;
						}
					}
					if (!foundID)
					{
						Console.SetColorText(HalfRed);
						cout << "<Error> Could not find ClientID: " << intID << endl;
						Console.SetDefaultColor();
						LeaveCriticalSection(&outputCritical);
					}
					else
					{
						Console.SetColorText(HalfGreen);
						cout << "Client (&" << current << ")" << endl;
						cout << "Nickname: " << current->nickname << endl;
						cout << "ID: " << current->clientID << endl;
						cout << "Handle: " << current->ClientHandle << endl;
						cout << "Socket: " << current->ClientSocket << endl;
						cout << "pNext: &" << current->pNext << endl;
						Console.SetDefaultColor();
						LeaveCriticalSection(&outputCritical);
						
						foundID = false;
					}
					cout << endl;
					LeaveCriticalSection(&outputCritical);
					continue;
				}
				else if (_stricmp(message.c_str(), "/Help") == 0)
				{
					Console.SetColorText(HalfGreen);
					cout << "Command List:" << endl;
					cout << "/CLS - Clear the console screen." << endl;
					cout << "/Details - Gets details on a specific user via Client ID." << endl;
					cout << "/Help - Displays this list." << endl;
					cout << "/Kick - Kicks a user via Client ID." << endl;
					cout << "/List - Get a list of currently connected client Names & IDs." << endl;
					cout << "/Shutdown - Disconnects clients & shuts the server down." << endl;
					cout << "----" << endl;
					cout << endl;
					Console.SetDefaultColor();
					LeaveCriticalSection(&outputCritical);
					continue;
				}
				else if (_stricmp(message.c_str(), "/Kick") == 0)
				{
					chID = "";
					Console.SetColorText(FullGreen);
					cout << "User ID number: "; getline(cin, chID);
					Console.SetDefaultColor();
					intID = atoi(chID.c_str());

					current = pGlobalTail;
					while (current->pNext != pGlobalHead)
					{
						current = current->pNext;

						if (current->clientID == intID)
						{
							foundID = true;
							break;
						}
					}
					if (!foundID)
					{
						Console.SetColorText(HalfRed);
						cout << "<Error> Could not find ClientID: " << intID << endl;
						Console.SetDefaultColor();
						LeaveCriticalSection(&outputCritical);
					}
					else
					{
						Console.SetColorText(FullRed);
						cout << "Kicking " << current->nickname << "..." << endl;
						Console.SetDefaultColor();


						message = "";
						message = current->nickname;
						message.append(" was kicked.");
						gClientIDQueue.push(-1);
						gMessageQueue.push(message);

						ResetEvent(broadcastEvent);

						WaitForSingleObject(broadcastEvent, INFINITE);
						send(current->ClientSocket, "/Kick", 6, 0);
						
						foundID = false;
						LeaveCriticalSection(&outputCritical);
					}
					cout << endl;
					LeaveCriticalSection(&outputCritical);
					continue;
				}
				else if (_stricmp(message.c_str(), "/List") == 0)
				{
					Console.SetColorText(HalfGreen);
					cout << "Users connected: " << serverInfo->connected << endl;
					current = pGlobalTail;
					while (current->pNext != pGlobalHead)
					{
						current = current->pNext;
						cout << current->nickname << " (ID: " << current->clientID << ")" << endl;
					}
					cout << endl;
					Console.SetDefaultColor();
					LeaveCriticalSection(&outputCritical);
					continue;
				}
				else if (_stricmp(message.c_str(), "/Shutdown") == 0)
				{
					cout << endl;
					LeaveCriticalSection(&outputCritical);

					serverInfo->quit = true;
					SetEvent(listenEvent);

					shutdownHandle = (HANDLE)_beginthreadex(0, 0, &ServerShutdown, (void*)0, 0, 0);
					WaitForSingleObject(shutdownHandle, INFINITE);
					CloseHandle(shutdownHandle);
					continue;
				}
				else
				{
					Console.SetColorText(HalfRed);
					cout << "<Error> Command not recognized" << endl << endl;
					Console.SetDefaultColor();
					LeaveCriticalSection(&outputCritical);
					continue;
				}
			}

			cout << "Broadcasting: " << message << endl;		

			message.insert(0, "[SERVER] ");

			gClientIDQueue.push(-1);
			gMessageQueue.push(message);
			LeaveCriticalSection(&outputCritical);
		}
	}

	EnterCriticalSection(&outputCritical);
	Console.SetColorText(HalfYellow);
	cout << "ServerInputThread Closing" << endl;
	Console.SetDefaultColor();
	LeaveCriticalSection(&outputCritical);

	return 0;
}

unsigned int __stdcall ServerShutdown(void *params)
{
	ClientInfo *current;

	current = pGlobalTail;
	while (!gMessageQueue.empty())
	{
		Sleep(100);
	}
	while (current->pNext != pGlobalHead)
	{
		current = current->pNext;

		send(current->ClientSocket, "/ServQuit", 10, 0);
		current = pGlobalTail;
		WaitForSingleObject(shutdownEvent, INFINITE);
	}
	return 0;
}