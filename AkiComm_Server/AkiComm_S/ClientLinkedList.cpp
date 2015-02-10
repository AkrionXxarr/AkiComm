#include "Includes.h"

void CreateHead(ClientInfo *pHead);
void CreateTail(ClientInfo *pTail, ClientInfo *pHead);

void InitClientLinkedList(ClientInfo *pTail, ClientInfo *pHead)
{
	CreateHead(pHead);
	CreateTail(pTail, pHead);
}

void CreateHead(ClientInfo *pHead)
{
	pHead->pNext = NULL;
	pHead->ClientHandle = NULL;
	pHead->ClientSocket = INVALID_SOCKET;
	pHead->clientID = -1;
	pHead->nickname = "";
}

void CreateTail(ClientInfo *pTail, ClientInfo *pHead)
{
	pTail->pNext = pHead;
	pTail->ClientHandle = NULL;
	pTail->ClientSocket = INVALID_SOCKET;
	pTail->clientID = -1;
	pTail->nickname = "";
}

ClientInfo *InsertStruct(ClientInfo *client)
{
	ClientInfo *pPrev;

	ClientInfo *pClient = new ClientInfo;

	pClient->ClientHandle = client->ClientHandle;
	pClient->clientID = client->clientID;
	pClient->ClientSocket = client->ClientSocket;
	pClient->pNext = NULL;
	pClient->nickname = "";
	pClient->isConnected = false;


	// Get the address of the struct immediately before the head struct
	pPrev = pGlobalTail;
	while (pPrev->pNext != pGlobalHead)
	{
		pPrev = pPrev->pNext;
	}

	pClient->pNext = pGlobalHead;
	pPrev->pNext = pClient;

	return pClient;
}

bool RemoveStruct(ClientInfo &client, int clientID)
{
	ClientInfo *pClient;
	ClientInfo *pPrev;

	pClient = pGlobalTail;
	pPrev = pGlobalTail;

	if (pClient->pNext == pGlobalHead)
	{
		return false;
	}

	// Get the appropriate structure from the list using clientID
	while (pClient->pNext)
	{
		pClient = pClient->pNext;

		if (pClient->clientID == clientID)
		{
			break;
		}
		else if (!pClient->pNext)
		{
			cout << "Error in RemoveStruct: clientID not found" << endl;
			return true;
		}
	}

	// Get the structure immediately before pClient
	while (pPrev->pNext != pClient)
	{
		pPrev = pPrev->pNext;
	}

	pPrev->pNext = pClient->pNext;
	pClient->pNext = NULL;

	client.ClientHandle = pClient->ClientHandle;
	client.clientID = pClient->clientID;
	client.ClientSocket = pClient->ClientSocket;
	client.nickname = pClient->nickname;
	client.isConnected = pClient->isConnected;
	client.pNext = pClient->pNext;

	delete pClient;
	return true;
}