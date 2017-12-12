#ifndef _USER_INTERFACE_H
#define _USER_INTERFACE_H


#include "UI\UIObject.h"
#include "Main.h"


extern UI::CUIManager		GUI;
extern UI::CGraphicControl	Mainform;
extern UI::CGraphicControl	Gameform;

extern void InitUserInterface();


void InitGameform();
void InitMainform();
void InitModsform();
void InitMultiplayerform();
void InitSettingsform();
void InitUserInterface();

void Back();

void Mainform_NewGameClick();
void Mainform_ModsClick();
void Mainform_MultiplayerClick();
void Mainform_QuitClick();
void Mainform_SettingsClick();


#endif