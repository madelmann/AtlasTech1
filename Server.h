
#pragma once

// Library includes

// Project includes
#include "Main.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"



WSADATA wsaData;
SOCKET ListenSocket = INVALID_SOCKET,
       ClientSocket = INVALID_SOCKET;
struct addrinfo *result = NULL;
struct addrinfo hints;
int recvbuflen = DEFAULT_BUFLEN;

class CGameServer
{
public:
	int StartServer();

    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
};
