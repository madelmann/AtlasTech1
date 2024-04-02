#ifndef _CHAT_H
#define _CHAT_H

#include "Main.h"

#define MAX_CHATMESSAGES 8

class CChatMessage
{
public:
	CChatMessage();
	~CChatMessage();

	unsigned long lTickDuration;
	unsigned long lTickStart;
	char Message[255];
};

class CChatManager
{
public:
	CChatManager(int maxvisible);
	~CChatManager();
	void Add(char msg[255], char From[255]);
	void Clear();
	void Delete(int Index);
	void Manage();
	void Render();

	int Count;
	int iMaxVisibleMessages;
	int iVisibleMessages;
	CChatMessage Messages[100];
};

#endif