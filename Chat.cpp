
// Library includes
#include <cstring>
#include <stdio.h>

// Project includes
#include "Chat.h"
#include "EngineUtils.h"


CChatMessage::CChatMessage()
{
	lTickDuration = 0;
	lTickStart = 0;
	sprintf(Message, "");
}

CChatMessage::~CChatMessage()
{
	lTickDuration = 0;
	lTickStart = 0;
	sprintf(Message, "");
}


CChatManager::CChatManager(int maxvisible)
{
	Count = 0;
	iMaxVisibleMessages = maxvisible;
}

CChatManager::~CChatManager()
{
	Count = 0;
}

void CChatManager::Add(char msg[255], char From[255])
{
	if(Count > MAX_CHATMESSAGES)
	{
		for(int i = 1; i < MAX_CHATMESSAGES; i += 1)
		{
			Messages[i] = Messages[i - 1];
		}
		Count = MAX_CHATMESSAGES - 1;
	}

	Messages[Count].lTickDuration = 8000;// + 200 * strlen(msg);
	Messages[Count].lTickStart = GetTickCount();
	if(strcmp(From, "") == 0)
		sprintf(Messages[Count].Message, "%s", msg);
	else
		sprintf(Messages[Count].Message, "%s: %s", From, msg);

	Count++;
}

void CChatManager::Clear()
{
	for(int i = 0; i < Count; i += 1)
	{
		Delete(i);
	}
}

void CChatManager::Delete(int Index)
{
	if(Index < 0 || Index >= Count)
		return;

	if(Index == Count - 1)
	{
		Messages[Index].~CChatMessage();	
		Count--;

		return;
	}

	for(int i = Index; i < Count - 1; i += 1)
	{
		Messages[i] = Messages[i + 1];
	}

	Messages[Count - 1].~CChatMessage();	
	Count--;
	iVisibleMessages--;
}

void CChatManager::Manage()
{
	if(Count <= 0)
		return;

	iVisibleMessages = Count;
	if(iVisibleMessages > iMaxVisibleMessages)
		iVisibleMessages = iMaxVisibleMessages;

	for(int i = 0; i < Count; i += 1)
	{
		if(Messages[i].lTickStart + Messages[i].lTickDuration < GetTickCount())
			Delete(i);
	}
}

void CChatManager::Render()
{
	if(g_Manager_Chat.Count > 0)
	{
		glColor3f(0.22f, 0.41f, 0.66f);

		for(int i = iVisibleMessages - 1; i >= 0; i -= 1)
		{
			glPrintText(-0.95f, -0.83f + (g_Manager_Chat.iVisibleMessages - i - 1) * 0.08f, g_Manager_Chat.Messages[i].Message);
		}
	}
}