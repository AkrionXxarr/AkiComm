#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Input / Output
#include <conio.h>
#include <iostream>
#include <string>
using namespace std;

// Windows
#include <windows.h>

// Win Threading
#include <process.h>

// Win Sockets
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// Data Structures
#include <list>
#include <queue>
#include <deque>

typedef deque<char*> CHARDEQUE;
typedef queue<char*> CHARQUEUE;

// Local headers
#include "Declarations.h"
#include "ConsoleOperations.h"


#pragma comment(lib, "Ws2_32.lib")


#define DEFAULT_PORT "3333"
#define DEFAULT_BUFLEN 1024


#endif