
#ifndef _MAIN_H
#define _MAIN_H

// Memory leak check - Begin
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>

#include <iostream>
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
// Memory leak check - End

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <gl.h>										// Header File For The OpenGL32 Library
#include "GLExt.h"
#include <glu.h>									// Header File For The GLu32 Library
#include <glaux.h>									// Header File For The GLaux Library


using namespace std;


#define MAX_CLIENTS				32


extern HWND			g_hWnd;									// This is the handle for the window
extern RECT			g_rRect;								// This holds the window dimensions
extern HDC			g_hDC;									// General HDC - (handle to device context)
extern HGLRC		g_hRC;									// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE	g_hInstance;							// This holds our window hInstance

extern bool			g_bActiveApp;
extern bool			g_bLoading;
extern char			g_sGameName[255];

extern char			g_sServerIP[80];
extern UINT			g_Texture[10];								// This stores the texture IDs

extern char			g_sWindowText[255];
extern char			g_sEditMode[255];


#define MAX_THREADS	8
extern DWORD		g_dwThreadId[MAX_THREADS];
extern HANDLE		g_hThread[MAX_THREADS];
extern int			g_iThreadCount;


#include "Tools.h"
#include "Engine.h"


#include "Game.h"
extern CGame Game;


bool bSetupPixelFormat(HDC hdc);																// This allows us to configure our window for OpenGL and backbuffering
void Cat(char *Filename);
void ChangeToFullScreen();																		// This changes the screen to full screen mode
HWND CreateMyWindow(char* strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);	// This is our own function that makes creating a window modular and easy
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
CGraphicObject* Entity_add(char* filename);
CSceneObject* GetObjectByID(int id);
void Init(HWND hWnd);																			// This initializes the whole program
void InitializeOpenGL(int width, int height);													// This sets up OpenGL
WPARAM MainLoop();																				// This controls our main program loop
void Manage_Threads();
int  Material_Add(char* Filename);
int  Material_Apply(int index, bool bForceMaterialUse);
void Net_SendMessage(char *msg, bool bBroadCast);
int  Object3DS_Add(char* Filename);
int  Object3DS_IndexOf(char* Filename);
CPlayer* Player_Add(char* Filename);
void PrintToConsole(char* text);
int ProcessCommand(char cmd[255]);
void ProcessNetMessage(RPCParameters *rpcParameters);
void ProcessKeyPress(WPARAM Key);
void ProcessKeyPress_Editor(WPARAM Key);
void ProcessKeyPress_Game(WPARAM Key);
void ProcessKeyPress_Menu(WPARAM Key);
void SetVSyncState(bool enable);
void SizeOpenGLScreen(int width, int height);													// This inits our screen translations and projections
GLuint  Texture_Add(char* Filename);
GLuint  Texture_IndexOf(char* Filename);
void  Texture_SetActive(char* Filename);
void  Texture_SetActiveID(int id);
int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);				// This is our MAIN() for windows
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);				// The window proc which handles all of window's messages.

#endif
