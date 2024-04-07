
#pragma once

//// Memory leak check - Begin
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//
//#include <iostream>
//#include <crtdbg.h>
//#ifdef _DEBUG
//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW
//#endif
//// Memory leak check - End

// Library includes
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>

// Project includes
#include "Engine.h"
#include "Game.h"
#include "Tools.h"



#define MAX_THREADS	8
extern unsigned long    g_dwThreadId[MAX_THREADS];
extern void*		    g_hThread[MAX_THREADS];
extern int			    g_iThreadCount;


bool bSetupPixelFormat(HDC hdc);																// This allows us to configure our window for OpenGL and backbuffering
void Cat(char *Filename);
void ChangeToFullScreen();																		// This changes the screen to full screen mode
HWND CreateMyWindow(char* strWindowName, int width, int height, unsigned long dwStyle, bool bFullScreen, HINSTANCE hInstance);	// This is our own function that makes creating a window modular and easy
bool CreateTexture(UINT &texture, char* strFileName);											// This creates a texture map from a file name and a reference to a UINT to store it
void ConfigRead(char sGame[255]);
void DeInit();																					// This frees all our memory in our program
void Dispatch_MouseDownLeft();
void Dispatch_MouseDownMiddle();
void Dispatch_MouseDownRight();
void Dispatch_MouseMove();
void Dispatch_MouseUpLeft();
void Dispatch_MouseUpMiddle();
void Dispatch_MouseUpRight();
void DrawMousePoint(float X, float Y, float Z);													// This draws a textured Quad at the mouse position
void Init(HWND hWnd);																			// This initializes the whole program
void InitializeOpenGL(int width, int height);													// This sets up OpenGL
WPARAM MainLoop();																				// This controls our main program loop
void Manage_Threads();
void Net_SendMessage(char *msg, bool bBroadCast);

void PrintToConsole(char* text);
int ProcessCommand(char cmd[255]);
void ProcessNetMessage(RPCParameters *rpcParameters);
void ProcessKeyPress(WPARAM Key);
void ProcessKeyPress_Editor(WPARAM Key);
void ProcessKeyPress_Game(WPARAM Key);
void ProcessKeyPress_Menu(WPARAM Key);
void SetVSyncState(bool enable);
void SizeOpenGLScreen(int width, int height);													// This inits our screen translations and projections
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);				// This is our MAIN() for windows
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);				// The window proc which handles all of window's messages.
