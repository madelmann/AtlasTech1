// This are compiler directives that include libraries (For Visual Studio).
#pragma comment(lib, "winmm.lib")


#pragma once
#pragma warning(disable : 4068)
#pragma warning(disable : 4611)
#pragma warning(disable : 4996)
#pragma warning(disable : 6031)
#pragma warning(disable : 6067)


//#include <vld.h>
#include <cassert>

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

#include "Main.h"																// This includes our main header file


DWORD		g_dwThreadId[MAX_THREADS];
HANDLE		g_hThread[MAX_THREADS];
int			g_iThreadCount = 0;


char		g_sGameName[255]		= "DaVinci\\";

HWND		g_hWnd;																// This is the handle for the window
RECT		g_rRect;															// This holds the window dimensions
HDC			g_hDC;																// General HDC - (handle to device context)
HGLRC		g_hRC;																// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE	g_hInstance;														// This holds the global hInstance for UnregisterClass() in DeInit()

bool		g_bActiveApp			= false;
bool		g_bLoading				= false;

char		g_sEditMode[255]		= "";
char		g_sMapName[255]			= "";
char		g_sServerIP[80]			= "127.0.0.1";
char		g_sWindowText[255]		= "";

int			iMenuIndex				= 0;


CGame		Game;


UINT		g_Texture[10]			= {0};										// This holds the texture info by an ID


extern CVertex	Terrain[9][256][256];											// Terrain

int				iServerInstances = 0;
TServerInstance	ServerInstance[MAX_SERVER_INSTANZES];


bool	bTeleport = false;

// Multiplayer - Begin
bool				receivedpacket = false;										// used to remember if the opponent has finished yet
int					ip[4];														// used to store IP address of server
RakPeerInterface	*peer;
SystemAddress		saServer;
char				sServerIP[16] = "127.0.0.1";
// Multiplayer - End


void Terrain_Edit()
{
	//// Edit terrain
	//g_Manager_Octree.oFloorMap[g_iMapIndex].OctreeMap[g_iOctreeZ][g_iOctreeX].Vertices[g_iVertexZ][g_iVertexX].Vertex.y += 5.0f;
	//// Edit walk map
	//Terrain[g_iMapIndex][g_iOctreeZ * 16 + g_iVertexZ][g_iOctreeX * 16 + g_iVertexX].Vertex.y += 5.0f;

	//// Preprocess mesh
	//g_Manager_Octree.PreProcess_Single_Version3(&g_Manager_Octree.oFloorMap[g_iMapIndex].OctreeMap[g_iOctreeZ][g_iOctreeX]);
	//g_Manager_Octree.oFloorMap[g_iMapIndex].iMeshID[g_iOctreeZ][g_iOctreeX] = g_Manager_Octree.oFloorMap[g_iMapIndex].OctreeMap[g_iOctreeZ][g_iOctreeX].iMeshID;
}


void Cat(char *Filename)
{
	char FName[255] = "";
	sprintf(FName, "%sMaps\\%s\\SCRIPTS\\%s", g_sGameName, Game.sMapName, Filename);

	FILE* pFile;

	fopen_s(&pFile, FName, "rt");
	if ( !pFile ) {
		sprintf(FName, "%sScripts\\%s", g_sGameName, Filename);

		fopen_s(&pFile, FName, "rt");
		if ( !pFile ) {
			// Display an error message and don't load anything if no file was found
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Cat]: '%s' not found!\n", FName);
			Console->Output(buffer);
			return;
		}
	}

	char oneline[255] = "";

	sprintf(oneline, "File: %s", FName);
	Console->Output(oneline);

	do {
		sprintf(oneline, "");
		readstr(pFile, oneline);

		Console->Output(oneline);
	} while(strcmp(oneline, "") != 0);

	fclose(pFile);
}

void Dispatch_MouseDownLeft()
{
	Mouse.Button1 = MouseSpace::Down;
	Mouse.Button1Begin = timeGetTime();
	GetClientMousePosition();					// Gets The Current Client Cursor Coordinates (Mouse Coordinates)
	Mouse.mp3d = GetClientMousePosition3D();

	char buffer[255] = "";

	if(!Console->IsVisible())
		GUI.update(&Mouse);

	if(!Game.bPlayMode) {
		int id = -1;

		Scene.ObjectHover(Mouse.mp3d);

		switch(Editor->GetMode())
		{
		case EditorSpace::Selection:
			{
				if(Engine->Keybinding.kbRun.bPressed)
				{
					// Try to attach node
					if(Scene.iActive != -1 && Scene.iHoverObject != -1 && Scene.iActive != Scene.iHoverObject)
					{
						// If node is already attached then remove it
						if(!Scene.ObjectNodeAttach(Scene.iActive, Scene.iHoverObject))
							Scene.ObjectNodeRemove(Scene.iActive, Scene.iHoverObject);

						// Reset object selection
						Scene.iActive = -1;
					}
				}
				else
					Scene.ObjectSelect(Mouse.mp3d);
			}
			break;
		case EditorSpace::max3ds:
			{
				sprintf(buffer, "%sObjects", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "3ds");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);
				sprintf(buffer, "%s", objectname.data());
				
				id = g_Manager_3DSObject.Add(buffer);
				g_Manager_3DSObject.Mesh[id]->Spawn(g_Manager_3DSObject.Mesh[id]->getName(), Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z, g_Manager_3DSObject.Mesh[id]->fScale);

				g_Manager_Physic.Add(g_Manager_3DSObject.Mesh[id]);
				Scene.Add(g_Manager_3DSObject.Mesh[id]);
			}
			break;
		case EditorSpace::md2:
			{
				sprintf(buffer, "%sModels", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "md2");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);
				sprintf(buffer, "%s", objectname.data());

				id = g_Manager_Entity.Add(buffer, "");
				g_Manager_Entity.Object[id].Spawn(g_Manager_Entity.Object[g_Manager_Entity.Count - 1].getName(), Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z, g_Manager_Entity.Object[id].fScale);

				g_Manager_Physic.Add(&g_Manager_Entity.Object[id]);
				Scene.Add(&g_Manager_Entity.Object[id]);
			}
			break;
		case EditorSpace::Static:
			{
				sprintf(buffer, "%sStatic", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "static");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);
				sprintf(buffer, "%s", objectname.data());

				id = g_Manager_StaticObject.Add(buffer);
				g_Manager_StaticObject.Object[id].Spawn(g_Manager_StaticObject.Object[id].getName(), Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z, g_Manager_StaticObject.Object[id].fScale);

				Scene.Add(&g_Manager_StaticObject.Object[id]);
			}
			break;
		case EditorSpace::Bot:
			{
				sprintf(buffer, "%sModels", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "md2");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);
				sprintf(buffer, "%s", objectname.data());

				id = g_Manager_Bot.Add(RemoveFileExt(buffer));
				//g_Manager_Bot.Bot[id].Player->Spawn(g_Manager_Bot.Bot[id].Player->mName, true, Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z);
				g_pActiveBot->Player->Spawn(g_pActiveBot->Player->getName(), true, Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z);

				Scene.Add(g_pActiveBot->Player);
			}
			break;
		case EditorSpace::Particle:
			{
				sprintf(buffer, "%sParticles", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "particle");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);
				sprintf(buffer, "%s", objectname.data());

				id = g_Manager_ParticleSystem.Add(buffer);
				if(id >= 0)
				{
					g_Manager_ParticleSystem.Object[id].SetPosition(Mouse.mp3d);
					Scene.Add(&g_Manager_ParticleSystem.Object[id]);
				}
			}
			break;
		case EditorSpace::Light:
			{
			}
			break;
		}

		if(!Engine->Keybinding.kbRun.bPressed)
		{
			if(Scene.iActive != -1 && Scene.iHoverObject != -1 && Scene.iActive != Scene.iHoverObject)
			{
				// If node is already attached then remove it
				if(!Scene.ObjectNodeAttach(Scene.iActive, Scene.iHoverObject))
					Scene.ObjectNodeRemove(Scene.iActive, Scene.iHoverObject);

				// Reset object selection
				Scene.iActive = -1;
			}
		}
	}
	else
	{
		if(Gameform.IsVisible())
		{
			switch(Game.iGameMode)
			{
			case MODE_GAME_COMMAND:
				g_bInsertInWorld = true;
				Game.iGameMode = MODE_GAME_NORMAL;
				break;
			default:
				Game.mPlayer->Shoot(false);

				sprintf(buffer, "%i|%i", CLIENT_SHOOT_BEGIN, g_iLocalPlayer);
				Net_SendMessage(buffer, true);
				break;
			}
		}
	}
}

void Dispatch_MouseDownMiddle()
{
	Mouse.Button3 = MouseSpace::Down;
	Mouse.Button3Begin = timeGetTime();
	GetClientMousePosition();

	if(!Game.bPlayMode)
	{
		sprintf(Scene.sHoverMessage, "");
	}
}

void Dispatch_MouseDownRight()
{
	Mouse.Button2 = MouseSpace::Down;
	Mouse.Button2Begin = timeGetTime();
	GetClientMousePosition();
	Mouse.mp3d = GetClientMousePosition3D();

	if(!Game.bPlayMode)
	{
		switch(Editor->GetMode())
		{
		case EditorSpace::Selection:
			{
				if(Scene.iActive == Scene.iHoverObject)
					Scene.iActive = -1;
				else
					Scene.iActive = Scene.iHoverObject;
			}
			break;
		}
	}
}

void Dispatch_MouseMove()
{
	GetClientMousePosition();							// Gets The Current Client Cursor Coordinates (Mouse Coordinates)

	if(!Console->IsVisible())
		GUI.update(&Mouse);

	if(Game.bPlayMode)
	{
	}
	else
	{
		Mouse.mp3d = GetClientMousePosition3D();

		//if(Mouse.Button1 == MouseSpace::Down)
		//	Scene.ObjectSetPosition(Mouse.mp3d);
		//else
		if(Mouse.Button1 != MouseSpace::Down)
			Scene.ObjectHover(Mouse.mp3d);


		switch(Editor->GetMode())
		{
		case EditorSpace::Selection:
			{
				if(Mouse.Button1 == MouseSpace::Down)
				{
					Scene.ObjectSetPosition(Mouse.mp3d);
				}
				//else if(g_Manager_Menu.bInGameMenu)
				//{
				//	Mouse.mp = GetClientMousePosition();
				//	g_Manager_Menu.CalculateMousePosition((float)Mouse.mp.x, (float)Mouse.mp.y);

				//	Mouse.mp3d = GetClientMousePosition3D();
				//	Scene.ObjectHover(Mouse.mp3d);
				//}
			}
			break;
		case EditorSpace::Terrain:
			{
				Mouse.mp3d.x -= QuadTree.fTerrainXStart;
				Mouse.mp3d.z -= QuadTree.fTerrainZStart;

				g_iOctreeX = int(Mouse.mp3d.x / 256);
				g_iOctreeZ = int(Mouse.mp3d.z / 256);

				g_iVertexX = (int)roundf((Mouse.mp3d.x - g_iOctreeX * 16) / g_iHeightMapScaleX);
				g_iVertexZ = (int)roundf((Mouse.mp3d.z - g_iOctreeZ * 16) / g_iHeightMapScaleZ);
			}
			break;
		case EditorSpace::Rotation:
			if(Mouse.Button1 == MouseSpace::Down)
			{
				Scene.ObjectSetRotation(Mouse.mp3d);
			}
			break;
		default:
			break;
		}
	}
}

void Dispatch_MouseUpLeft()
{
	char buffer[255] = "";
	
	if(timeGetTime() - Mouse.Button1Begin < CLICK_TIME)
		Mouse.Button1 = MouseSpace::Click;
	else
		Mouse.Button1 = MouseSpace::None;

	Mouse.mp = GetClientMousePosition();		// Gets The Current Client Cursor Coordinates (Mouse Coordinates)
	Mouse.mp3d = GetClientMousePosition3D();

	if(!Console->IsVisible()) {
		GUI.update(&Mouse);
	}

	if(Game.bPlayMode)
	{
		if(Gameform.IsVisible())
		{
			Game.mPlayer->bShoot = false;

			sprintf(buffer, "%i|%i", CLIENT_SHOOT_END, g_iLocalPlayer);
			Net_SendMessage(buffer, true);
		}
	}
	else
	{
		switch(Editor->GetMode())
		{
		case EditorSpace::Selection:
			if ( Mouse.Button1 != MouseSpace::Click ) {
				Scene.ObjectSetPosition(Mouse.mp3d);
			}
			break;
		}
	}

	// Reset mouse click after usage
	if ( Mouse.Button1 == MouseSpace::Click ) {
		Mouse.Button1 = MouseSpace::None;
	}
}

void Dispatch_MouseUpMiddle()
{
	if(timeGetTime() - Mouse.Button3Begin < CLICK_TIME)
		Mouse.Button3 = MouseSpace::Click;
	else
		Mouse.Button3 = MouseSpace::None;

	if(!Console->IsVisible())
		GUI.update(&Mouse);
}

void Dispatch_MouseUpRight()
{
	if(timeGetTime() - Mouse.Button2Begin < CLICK_TIME)
		Mouse.Button2 = MouseSpace::Click;
	else
		Mouse.Button2 = MouseSpace::None;

	if(!Console->IsVisible())
		GUI.update(&Mouse);
}

void DrawMousePoint(float X, float Y, float Z)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.7f);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	glEnable(GL_BLEND);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Manager_Texture->CreateTGATexture("PlasmaBall.tga"));
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(X - 5.0f, Y - 5.0f, Z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(X + 5.0f, Y - 5.0f, Z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(X + 5.0f, Y + 5.0f, Z);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(X - 5.0f, Y + 5.0f, Z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);

	// Start drawing the top side as a QUAD
	glBegin(GL_QUADS);	
		glVertex3f(X - 3.0f, Y + 10.0f, Z - 3.0f);
		glVertex3f(X - 3.0f, Y + 10.0f, Z + 3.0f);
		glVertex3f(X + 3.0f, Y + 10.0f, Z + 3.0f); 
		glVertex3f(X + 3.0f, Y + 10.0f, Z - 3.0f);
	glEnd();
	
	// Start drawing the left side as a QUAD
	glBegin(GL_TRIANGLES);		
		glVertex3f(X - 3.0f, Y + 10.0f, Z + 3.0f);
		glVertex3f(X - 3.0f, Y + 10.0f, Z - 3.0f);
		glVertex3f(X, Y, Z); 
	glEnd();

	// Start drawing the right side as a QUAD
	glBegin(GL_TRIANGLES);		
		glVertex3f(X + 3.0f, Y + 10.0f, Z - 3.0f);
		glVertex3f(X + 3.0f, Y + 10.0f, Z + 3.0f);
		glVertex3f(X, Y, Z); 
	glEnd();

	// Start drawing the back side as a QUAD
	glBegin(GL_TRIANGLES);		
		glVertex3f(X - 3.0f, Y + 10.0f, Z - 3.0f);
		glVertex3f(X + 3.0f, Y + 10.0f, Z - 3.0f);
		glVertex3f(X, Y, Z); 
	glEnd();

	// Start drawing the front side as a QUAD
	glBegin(GL_TRIANGLES);		
		glVertex3f(X + 3.0f, Y + 10.0f, Z + 3.0f);
		glVertex3f(X - 3.0f, Y + 10.0f, Z + 3.0f);
		glVertex3f(X, Y, Z); 
	glEnd();
}

CGraphicObject* Entity_add(char* filename)
{
	CGraphicObject* result = NULL;

	if(!filename)
		return result;

	char* sExtention = "";
	sExtention = ExtractFileExt(filename);

	int id = -1;
	if(strcmpi(sExtention, "3ds") == 0)
	{
		id = g_Manager_3DSObject.Add(filename);
		result = g_Manager_3DSObject.Mesh[id];
	}
	else if(strcmpi(sExtention, "md2") == 0)
	{
		id = g_Manager_Entity.Add(filename, filename);
		result = &g_Manager_Entity.Object[id];
	}
	else if(strcmpi(sExtention, "particle") == 0)
	{
		id = g_Manager_StaticObject.Add(filename);
		result = &g_Manager_StaticObject.Object[id];
	}
	else if(strcmpi(sExtention, "static") == 0)
	{
		id = g_Manager_ParticleSystem.Add(filename);
		result = &g_Manager_ParticleSystem.Object[id];
	}
	else
	{
		Console->Output("ERROR: unsupported file format");
		assert(0);
	}

	return result;
}

CSceneObject* GetObjectByID(int id)
{
	return Scene.GetObjectByID(id);
}

///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application
/////
///////////////////////////////// INIT  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	Engine->Init();

	g_Manager_Statistic.SortByColumn(3);

	Game.bPlayMode = true;
}

void Net_SendMessage(char *msg, bool bBroadCast)
{
    if(peer == NULL)
		return;

    if(bBroadCast)
		peer->RPC("ProcessNetMessage", msg, (BitSize_t)((strlen(msg) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, bBroadCast, 0, UNASSIGNED_NETWORK_ID, 0);
    else
		peer->RPC("ProcessNetMessage", msg, (BitSize_t)((strlen(msg) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, saServer, false, 0, UNASSIGNED_NETWORK_ID, 0);
}

///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;
	Packet *packet;
	char buffer[255];
	CVector3 oldPos, oldView, oldAngle;

	long oldtick = GetTickCount();

	LONGLONG tmp;

	QueryPerformanceFrequency((LARGE_INTEGER*)&tmp);
	g_dFrequency = (double)tmp;

	while ( true ) {															// Do our infinite loop
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))							// Check if there was a message
        { 
			if(msg.message == WM_QUIT)											// If the message wasn't to quit
				break;

            TranslateMessage(&msg);												// Find out what the message does
            DispatchMessage(&msg);												// Execute the message
        }
		else {
			// Calculate our frame rate and set our frame interval for time-based movement
			CalculateFrameRate();

			//if(AnimateNextFrame(60))											// Make sure we only animate 60 FPS
			{
				if(Gameform.Visible)
				{
					if(g_bActiveApp && !Mainform.Visible && !Console->IsVisible())
					{
						//CheckForMovement();
						g_Camera.Update();										// Update the camera data
					}

					Engine->Manage();
				}

				Manage_Threads();
				Engine->Render();

				QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
				g_dCurTime = (double)tmp * 1000 / g_dFrequency;
				g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
				g_dLastTime    = g_dCurTime;
			}
			//else
			//{
			//	Sleep(1);														// Let other processes work
			//}
        }

		if(peer)
		{
			packet = peer->Receive();

			while(packet)
			{
				switch(packet->data[0])
				{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					g_Manager_Chat.Add("Another client has disconnected.", "Server");

					g_Manager_Player.Delete(g_Manager_Player.GetPlayerByAddress(packet->systemAddress));
					break;
				case ID_REMOTE_CONNECTION_LOST:
					g_Manager_Chat.Add("Another client has lost the connection.", "Server");

					g_Manager_Player.Delete(g_Manager_Player.GetPlayerByAddress(packet->systemAddress));
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					g_Manager_Chat.Add("Another client has connected.", "Server");

					ProcessCommand("PLAYER.ADD REMOTE");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					g_Manager_Chat.Add("Our connection request has been accepted.", "Server");

					saServer = packet->systemAddress;
					Game.bIsServer = false;

					sprintf(buffer, "%i|1", MS_REQUEST_SERVERINFO);
					Net_SendMessage(buffer, false);
					break;
				case ID_NEW_INCOMING_CONNECTION:
					g_Manager_Chat.Add("A connection is incoming.", "Server");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					g_Manager_Chat.Add("The server is full.", "Server");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					g_Manager_Chat.Add("You have been disconnected.", "Server");

					Game.mPlayer = NULL;
					g_Manager_Player.Clear();

					g_iLocalPlayer = 0;
					g_Manager_Player.Add(Game.sPlayerName);
					Game.mPlayer = &g_Manager_Player.Player[g_iLocalPlayer];
					break;
				case ID_CONNECTION_LOST:
					g_Manager_Chat.Add("Connection lost.", "Server");

					Game.mPlayer = NULL;
					g_Manager_Player.Clear();

					g_iLocalPlayer = 0;
					g_Manager_Player.Add(Game.sPlayerName);
					Game.mPlayer = &g_Manager_Player.Player[g_iLocalPlayer];
					break;
				default:
					sprintf(buffer, "Message with identifier %i has arrived.\n", packet->data[0]);

					g_Manager_Chat.Add(buffer, "Server");
					break;
				}

				peer->DeallocatePacket(packet);

				packet = peer->Receive();										// Stay in the loop as long as there are more packets
			}

			if(g_dCurTime > (unsigned)oldtick + 100)
			{
				if(
					Game.mPlayer->mPosition.x != oldPos.x
					|| Game.mPlayer->mPosition.y != oldPos.y
					|| Game.mPlayer->mPosition.z != oldPos.z
					)
				{
					sprintf(buffer, "%i|%i|%.3f|%.3f|%.3f", CLIENT_POSITION_XYZ, g_iLocalPlayer, Game.mPlayer->mPosition.x, Game.mPlayer->mPosition.y, Game.mPlayer->mPosition.z);
					peer->RPC("ProcessNetMessage", buffer, (BitSize_t)((strlen(buffer) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID, 0);

					oldPos = Game.mPlayer->mPosition;
				}
				if(
					Game.mPlayer->vView.x != oldView.x
					|| Game.mPlayer->vView.y != oldView.y
					|| Game.mPlayer->vView.z != oldView.z
					)
				{
					sprintf(buffer, "%i|%i|%.3f|%.3f|%.3f", CLIENT_VIEW_XYZ, g_iLocalPlayer, Game.mPlayer->vView.x, Game.mPlayer->vView.y, Game.mPlayer->vView.z);
					peer->RPC("ProcessNetMessage", buffer, (BitSize_t)((strlen(buffer) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID, 0);

					oldView = Game.mPlayer->vView;
				}
				if(
					Game.mPlayer->mRotation.x != oldAngle.x
					|| Game.mPlayer->mRotation.y != oldAngle.y
					|| Game.mPlayer->mRotation.z != oldAngle.z
					)
				{
					sprintf(buffer, "%i|%i|%.3f|%.3f|%.3f", CLIENT_VIEWANGLE_XYZ, g_iLocalPlayer, Game.mPlayer->mRotation.x, Game.mPlayer->mRotation.y, Game.mPlayer->mRotation.z);
					peer->RPC("ProcessNetMessage", buffer, (BitSize_t)((strlen(buffer) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID, 0);

					oldAngle.x = Game.mPlayer->mRotation.x;
					oldAngle.y = Game.mPlayer->mRotation.y;
					oldAngle.z = Game.mPlayer->mRotation.z;
				}

				oldtick = GetTickCount();
			}
		}
	}

	Font.ClearGLFont();															// Clear out the resources used by the font class.

	DeInit();																	// Clean up and free all allocated memory

	return(msg.wParam);															// Return from the program
}

void Manage_Threads()
{
	if(g_iThreadCount <= 0)
		return;

	for(int i = g_iThreadCount - 1; i >= 0; i -= 1)
	{
		if(g_hThread[i] != 0)
		{
			CloseHandle(g_hThread[i]);
			g_iThreadCount -= 1;
			g_hThread[i] = 0;
		}
	}
}

int Material_Add(char* Filename)
{
	return g_Manager_Material.Add(Filename);
}

int Material_Apply(int index, bool bForceMaterialUse)
{
	return g_Manager_Material.Apply(index, bForceMaterialUse);
}

int Object3DS_Add(char* Filename)
{
	return g_Manager_3DSObject.Add(Filename);
}

int Object3DS_IndexOf(char* Filename)
{
	return g_Manager_3DSObject.IndexOf(Filename);
}

CPlayer* Player_Add(char* Filename)
{
	g_Manager_Player.Add(Filename);
	return &g_Manager_Player.Player[g_Manager_Player.iActive];
}

void PrintToConsole(char* text)
{
	Console->Output(text);
}

int ProcessCommand(char cmd[255])
{
	Console->Output(cmd);

	if(strlen(cmd) <= 0)
		return -1;

	if(cmd[1] == '/')
		return -1;

	char cmdstr[255];
	sscanf(cmd, "%s \0", cmdstr);

	char buffer[255] = "";

	//if(strcmpi(cmdstr, "3DSOBJECT.SETACTIVE") == 0)
	//{
	//	sscanf(cmd, "3DSOBJECT.SETACTIVE %i", &g_Manager_3DSObject.iActive);
	//}
	// Deprecated - Begin
	if(strcmpi(cmdstr, "BASIC.LOAD") == 0)
    {
	    sscanf(cmd, "BASIC.LOAD %s", &buffer);
		
		g_hThread[g_iThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, (LPVOID)buffer, 0, &g_dwThreadId[g_iThreadCount]);
		g_iThreadCount++;

		return 0;
    }
	// Deprecated - End
	if(strcmpi(cmdstr, "BOT.ADD") == 0)
    {
	    if(g_Manager_Bot.Count >= MAX_BOTS)
		    return -1;

	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "BOT.ADD %s", &buffer);
	    }

		g_Manager_Bot.Add(buffer);

		if(strlen(buffer) == 0)
			sprintf(buffer, "Male");

		g_Manager_Bot.Bot[g_Manager_Bot.iActive].Player->Spawn(buffer, true, 0.0f, 0.0f, 0.0f);
    }
    if(strcmpi(cmdstr, "BOT.MODEL") == 0)
    {
	    sscanf(cmd, "BOT.MODEL %s", &buffer);

		if(!Game.bRunning)
			return 0;

	    g_Manager_Bot.Bot[g_Manager_Bot.iActive].Player->ChangeModel(buffer);

		/*
	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CLIENT_CHANGE_MODEL, g_iLocalPlayer, g_sPlayerModel);

	    Net_SendMessage(buffer2, true);
		*/
    }
    if(strcmpi(cmdstr, "BOT.NAME") == 0)
    {
	    sscanf(cmd, "BOT.NAME %s", &g_Manager_Bot.Bot[g_Manager_Bot.iActive].Player->mName);

		/*
	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CLIENT_CHANGE_NAME, g_iLocalPlayer, g_Manager_Player.Player[g_iLocalPlayer].Name);

	    Net_SendMessage(buffer2, true);
		*/
    }
	if(strcmpi(cmdstr, "BOT.SETACTIVE") == 0)
    {
	    sscanf(cmd, "BOT.SETACTIVE %i", &g_Manager_Bot.iActive);
		g_pActiveBot = &g_Manager_Bot.Bot[g_Manager_Bot.iActive];
    }
	if(strcmpi(cmdstr, "BOT.SETPOSITION") == 0)
    {
		float wx, wz;
		sscanf(cmd, "BOT.SETPOSITION %f %f", &wx, &wz);

		g_Manager_Bot.Bot[g_Manager_Bot.iActive].Player->SetPosition(CVector3(wx, 0.0f, wz));
    }
    if(strcmpi(cmdstr, "BOT.STARTNAME") == 0)
    {
	    sscanf(cmd, "BOT.STARTNAME %s", &Game.sPlayerName);
    }
    if(strcmpi(cmdstr, "BOT.TEAM") == 0)
    {
	    sscanf(cmd, "BOT.TEAM %i", &g_Manager_Bot.Bot[g_Manager_Bot.iActive].Player->iTeam);

		/*
	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%i", CLIENT_CHANGE_TEAM, g_iLocalPlayer, g_pActiveBot->iTeam);

	    Net_SendMessage(buffer2, true);
		*/
    }
	if(strcmpi(cmdstr, "BOT.WAYPOINT.ADD") == 0)
    {
		float wx, wz;
		sscanf(cmd, "BOT.WAYPOINT.ADD %f %f", &wx, &wz);

		g_Manager_Bot.Bot[g_Manager_Bot.iActive].WayPointAdd(wx, wz);
    }
	if(strcmpi(cmdstr, "CAMERA.MODE") == 0)
    {
	    sscanf(cmd, "CAMERA.MODE %i", &g_Camera.iMode);
	}
	if(strcmpi(cmdstr, "CAMERA.SETPOSITION") == 0)
    {
		CVector3 pos;
		sscanf(cmd, "CAMERA.SETPOSITION %f %f %f", &pos.x, &pos.y, &pos.z);

		g_Camera.SetSequencePosition(pos);
	}
	if(strcmpi(cmdstr, "CAMERA.SETVIEW") == 0)
    {
		CVector3 view;
		sscanf(cmd, "CAMERA.SETVIEW %f %f %f", &view.x, &view.y, &view.z);

		g_Camera.SetSequenceView(view);
	}
	if(strcmpi(cmdstr, "CAT") == 0)
    {
	    sscanf(cmd, "CAT %s", &buffer);

		Cat(buffer);
	}
    if(strcmpi(cmdstr, "CLS") == 0)
    {
	    Console->Clear();
    }
    if(strcmpi(cmdstr, "CONNECT") == 0)
    {
	    sprintf(sServerIP, "%s", g_sServerIP);

	    if(strcmpi(cmd, cmdstr) != 0) {
		    sscanf(cmd, "CONNECT %d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
		    sprintf(sServerIP, "%d.%d.%d.%d", (int)ip[0], (int)ip[1], (int)ip[2], (int)ip[3]);
	    }

	    peer = RakNetworkFactory::GetRakPeerInterface();
	    peer->Startup(1, 30, &SocketDescriptor(), 1);
	    peer->Connect(sServerIP, SERVER_PORT, 0, 0);

	    REGISTER_STATIC_RPC(peer, ProcessNetMessage);

	    Net_SendMessage("Client connecting...", false);
    }
	if(strcmpi(cmdstr, "CONSOLE.MODE") == 0)
    {
		int mode;
	    sscanf(cmd, "CONSOLE.MODE %i", &mode);
		Console->setOutputMode(mode);
    }
    if(strcmpi(cmdstr, "DISCONNECT") == 0)
    {
		if(g_iLocalPlayer < 0)
			return 0;

	    Game.mPlayer = NULL;
		g_Manager_Player.Clear();

		g_iLocalPlayer = 0;
		g_Manager_Player.Add(Game.sPlayerName);
		Game.mPlayer = &g_Manager_Player.Player[g_iLocalPlayer];

	    if(peer != NULL)
		    peer->CloseConnection(peer->GetSystemAddressFromIndex(0), true, 0);

		Game.bIsServer = true;
    }
	if(strcmpi(cmdstr, "EDITOR.MODE") == 0)
    {
		int i;
	    sscanf(cmd, "EDITOR.MODE %i", &i);

		Editor->SetMode(static_cast<EditorSpace::Mode_t>(i));
		//Editor_ModeSwitch(g_iEditMode);
    }
	if(strcmpi(cmdstr, "ENGINE.ANTIALIAS") == 0)
    {
	    sscanf(cmd, "ENGINE.ANTIALIAS %i", &Engine->iAntialias);
		sscanf(cmd, "ENGINE.ANTIALIAS %i", &Engine->bPointAndLineAntialiasing);
    }
    if(strcmpi(cmdstr, "ENGINE.CELLSHADING") == 0)
    {
	    sscanf(cmd, "ENGINE.CELLSHADING %i", &Engine->bCellShading);
    }
    if(strcmpi(cmdstr, "ENGINE.CLIP") == 0)
    {
	    sscanf(cmd, "ENGINE.CLIP %i", &Engine->bClipping);
    }
	if(strcmpi(cmdstr, "ENGINE.FOV") == 0)
    {
	    sscanf(cmd, "ENGINE.FOV %f", &Engine->fFOV);
		SizeOpenGLScreen(Engine->Screen.Width, Engine->Screen.Height);
    }
    if(strcmpi(cmdstr, "ENGINE.FRUSTUM") == 0)
    {
	    sscanf(cmd, "ENGINE.FRUSTUM %i", &Engine->bFrustumCulling);
    }
    if(strcmpi(cmdstr, "ENGINE.FULLSCREEN") == 0)
    {
	    sscanf(cmd, "ENGINE.FULLSCREEN %i", &Engine->bFullScreen);

		if(Engine->bFullScreen)
			ChangeToFullScreen();							// Go to full screen
		else
			ChangeDisplaySettings(NULL, 0);
    }
    if(strcmpi(cmdstr, "ENGINE.JUMPHEIGHT") == 0)
    {
	    sscanf(cmd, "ENGINE.JUMPHEIGHT %f", &Game.fJumpHeight);
    }
	if(strcmpi(cmdstr, "ENGINE.LIGHTMODELTWOSIDED") == 0)
    {
	    sscanf(cmd, "ENGINE.LIGHTMODELTWOSIDED %i", &Engine->bLightModelTwoSided);
		Engine->SetLightModel(Engine->bLightModelTwoSided);
    }
	if(strcmpi(cmdstr, "ENGINE.LOD") == 0)
    {
	    sscanf(cmd, "ENGINE.LOD %i", &Engine->iMaxLODLevel);
    }
	if(strcmpi(cmdstr, "ENGINE.MOTIONBLUR") == 0)
    {
	    sscanf(cmd, "ENGINE.MOTIONBLUR %f", &Engine->fMotionBlur);
    }
	if(strcmpi(cmdstr, "ENGINE.MULTISAMPLING") == 0)
    {
	    sscanf(cmd, "ENGINE.MULTISAMPLING %i", &Engine->iMultiSampling);
    }
	if(strcmpi(cmdstr, "ENGINE.MULTITEXTURING") == 0)
    {
	    sscanf(cmd, "ENGINE.MULTITEXTURING %i", &Engine->bMultiTexturing);
    }
	if(strcmpi(cmdstr, "ENGINE.SCREENHEIGHT") == 0)
    {
	    sscanf(cmd, "ENGINE.SCREENHEIGHT %i", &Engine->Screen.Height);
		SizeOpenGLScreen(Engine->Screen.Width, Engine->Screen.Height);
    }
	if(strcmpi(cmdstr, "ENGINE.SCREENWIDTH") == 0)
	{
	    sscanf(cmd, "ENGINE.SCREENWIDTH %i", &Engine->Screen.Width);
		SizeOpenGLScreen(Engine->Screen.Width, Engine->Screen.Height);
	}
    if(strcmpi(cmdstr, "ENGINE.SHADOWS") == 0)
    {
	    sscanf(cmd, "ENGINE.SHADOWS %i", &Engine->iShadows);
    }
	if(strcmpi(cmdstr, "ENGINE.SPHEREBOUNDINGS") == 0)
    {
	    sscanf(cmd, "ENGINE.SPHEREBOUNDINGS %i", &Engine->bSphereBoundings);
    }
    if(strcmpi(cmdstr, "ENGINE.VIEWWIDTH") == 0)
    {
	    sscanf(cmd, "ENGINE.VIEWWIDTH %i", &g_iViewWidth);
    }
    if(strcmpi(cmdstr, "ENGINE.VSYNC") == 0)
    {
	    sscanf(cmd, "ENGINE.VSYNC %i", &Engine->bVSync);

	    SetVSyncState(Engine->bVSync);
    }
	if(strcmpi(cmdstr, "ENTITY.SCALE") == 0)
    {
		sscanf(cmd, "ENTITY.SCALE %f", &g_Manager_Entity.Object[g_Manager_Entity.iActive].fScale);
    }
	if(strcmpi(cmdstr, "ENTITY.SETACTIVE") == 0)
    {
	    sscanf(cmd, "ENTITY.SETACTIVE %i", &g_Manager_Entity.iActive);
    }
	if(strcmpi(cmdstr, "ENTITY.TRANSLATION") == 0)
    {
		sscanf(cmd, "ENTITY.TRANSLATION %f %f %f", &g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationX, &g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationY, &g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationZ);
		g_Manager_Entity.Object[g_Manager_Entity.iActive].BoundingVolume.CY += g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationY;
		g_Manager_Entity.Object[g_Manager_Entity.iActive].BoundingVolume.Y1 += g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationY;
		g_Manager_Entity.Object[g_Manager_Entity.iActive].BoundingVolume.Y2 += g_Manager_Entity.Object[g_Manager_Entity.iActive].fTranslationY;
    }
    //if(strcmpi(cmdstr, "FLORA.RESCALE") == 0)
    //{
	   // sscanf(cmd, "FLORA.RESCALE %f", &g_Manager_Flora.fScale);
    //	
	   // g_Manager_Flora.ReScale();
    //}
    if(strcmpi(cmdstr, "FLORA.SPREAD") == 0)
    {
	    //sscanf(cmd, "FLORA.SPREAD %s %i", &g_Manager_Flora.sFilename, &g_Manager_Flora.iMax);
    	
	    g_Manager_Flora.Spread();
    }
	if(strcmpi(cmdstr, "GAME.ALLOWEDITOR") == 0)
    {
	    sscanf(cmd, "GAME.ALLOWEDITOR %d", &Game.bAllowEditor);

		if(Game.bAllowEditor)
			Editor->Allow();

		Game.bPlayMode = true;
    }
    if(strcmpi(cmdstr, "GAME.LOAD") == 0)
    {
		g_bLoading = true;

	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "GAME.LOAD %s", &buffer);
		    sprintf(buffer, "%s.sav", buffer);
	    }

	    Game.Load(buffer);

		g_bLoading = false;
	    Console->setVisible(false);
    }
    if(strcmpi(cmdstr, "GAME.NAME") == 0)
    {
	    sscanf(cmd, "GAME.NAME %s", &g_sGameName);
		sprintf(g_sGameName, "%s\\", g_sGameName);
    }
    if(strcmpi(cmdstr, "GAME.NEW") == 0)
    {
	    Game.New();
    }
	if(strcmpi(cmdstr, "GAME.PLAYERMODEL") == 0)
    {
		sscanf(cmd, "GAME.PLAYERMODEL %s", &Game.sPlayerModel);
    }
	if(strcmpi(cmdstr, "GAME.PLAYERNAME") == 0)
    {
		sscanf(cmd, "GAME.PLAYERNAME %s", &Game.sPlayerName);
    }
    if(strcmpi(cmdstr, "GAME.RESUME") == 0)
    {
	    if(Game.bRunning)
	    {
		    ShowCursor(FALSE);
	    }
    }
    if(strcmpi(cmdstr, "GAME.SAVE") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "GAME.SAVE %s", &buffer);
		    sprintf(buffer, "%s.sav", buffer);
	    }

	    Game.Save(buffer);

	    Console->setVisible(false);
    }
	if(strcmpi(cmdstr, "GAME.SCRIPTSEQUENCE") == 0)
	{
	    sscanf(cmd, "GAME.SCRIPTSEQUENCE %i", &Game.bScriptSequence);
		g_Camera.SetSequencePosition(g_Camera.Position());
		g_Camera.SetSequenceView(g_Camera.View());
	}
	if(strcmpi(cmdstr, "GAME.STARTMAP") == 0)
    {
		sscanf(cmd, "GAME.STARTMAP %s", &Game.sStartMap);
    }
    if(strcmpi(cmdstr, "GAME.TYPE") == 0)
    {
	    sscanf(cmd, "GAME.TYPE %i", &Game.iGameType);

	    sprintf(buffer, "%i|%s", REMOTE_COMMAND_SERVER, cmd);
	    Net_SendMessage(buffer, false);
	    sprintf(buffer, "%i|%s", REMOTE_COMMAND_CLIENT, cmd);
	    Net_SendMessage(buffer, true);
    }
    if(strcmpi(cmdstr, "HELP") == 0)
    {
	    if(strlen(cmd) == 4)
	    {
		    // Displays all console commands
		    Console->Output(" \0");
		    g_Manager_Script.Execute("Help\\COMMANDS.TXT", false, false);
	    }
	    else
	    {
		    // Displays specific help for a command
		    sscanf(cmd, "HELP %s", &buffer);

		    char buffer2[255] = "";
		    sprintf(buffer2, "Help\\%s.TXT", buffer);

		    Console->Output(" \0");
		    g_Manager_Script.Execute(buffer2, false, false);
	    }
    }
    if(strcmpi(cmdstr, "KICK") == 0)
    {
	    int iKickPlayer = -1;
	    sscanf(cmd, "KICK %i", &iKickPlayer);

	    if(iKickPlayer == g_iLocalPlayer)
	    {
		    ProcessCommand("DISCONNECT");
		    g_Manager_Chat.Add("You have been kicked.", "Server");
	    }
	    else
	    {
		    sprintf(buffer, "%i|%i|%s", REMOTE_COMMAND_CLIENT, g_iLocalPlayer, cmd);
		    Net_SendMessage(buffer, true);
	    }
    }
	if(strcmpi(cmdstr, "LIGHT") == 0)
    {
		int iLight = -1;
	    sscanf(cmd, "LIGHT %i", &iLight);

		if(iLight == -1)
			return -1;

		bool enabled;
		CVector3 pos;
		CLight::LightMode_t mode;
		sscanf(cmd, "LIGHT %i %i %f %f %f %i", &iLight, &enabled, &pos.x, &pos.y, &pos.z, &mode);

		if(enabled) {
			Light[iLight].setMode(mode);
			Light[iLight].setPosition(pos);
			Light[iLight].enable();
		}
    }
	if(strcmpi(cmdstr, "LIGHTAMBIENT") == 0)
    {
		int iLight = -1;
		sscanf(cmd, "LIGHTAMBIENT %i", &iLight);

		if(iLight == -1)
			return -1;

		float ambiant[4];
		sscanf(cmd, "LIGHTAMBIENT %i %f %f %f %f", &iLight, &ambiant[0], &ambiant[1], &ambiant[2], &ambiant[3]);

		Light[iLight].setAmbiant(CRGBA(ambiant[0], ambiant[1], ambiant[2], ambiant[3]));
    }
	if(strcmpi(cmdstr, "LIGHTDIFFUSE") == 0)
    {
		int iLight = -1;
		sscanf(cmd, "LIGHTDIFFUSE %i", &iLight);

		if(iLight == -1)
			return -1;

		float diffuse[4];
		sscanf(cmd, "LIGHTDIFFUSE %i %f %f %f %f", &iLight, &diffuse[0], &diffuse[1], &diffuse[2], &diffuse[3]);

		Light[iLight].setDiffuse(CRGBA(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
    }
	if(strcmpi(cmdstr, "LIGHTSPECULAR") == 0)
    {
		int iLight = -1;
		sscanf(cmd, "LIGHTSPECULAR %i", &iLight);

		if(iLight == -1)
			return -1;

		float specular[4];
		sscanf(cmd, "LIGHTSPECULAR %i %f %f %f %f", &iLight, &specular[0], &specular[1], &specular[2], &specular[3]);

		Light[iLight].setSpecular(CRGBA(specular[0], specular[1], specular[2], specular[3]));
    }
    if(strcmpi(cmdstr, "LOCALPLAYER") == 0)
    {
	    sscanf(cmd, "LOCALPLAYER %i", &g_iLocalPlayer);

	    if(g_iLocalPlayer > g_Manager_Player.Count - 1)
	    {
		    g_iLocalPlayer = g_Manager_Player.Count - 1;
	    }
    }
    if(strcmpi(cmdstr, "MAP.ALPHAMAP") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.ALPHAMAP %s", &Map.sAlphaMap);
		    g_Manager_Texture->CreateBMPTexture(Map.sAlphaMap);
	    }
    }
	if(strcmpi(cmdstr, "MAP.AMBIENTCOLOR") == 0)
    {
		sscanf(cmd, "MAP.AMBIENTCOLOR %f %f %f", &Map.fAmbientColor[0], &Map.fAmbientColor[1], &Map.fAmbientColor[2]);
	}
    if(strcmpi(cmdstr, "MAP.BASETEXTURE") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.BASETEXTURE %s", &Map.sBaseTexture);
		    g_Manager_Texture->CreateTexture(Map.sBaseTexture);
	    }
    }
	if(strcmpi(cmdstr, "MAP.FLOOR") == 0)
    {
		sscanf(cmd, "MAP.FLOOR %s", &Map.sFloorMap);
	}
	if(strcmpi(cmdstr, "MAP.FLOOR.OFFSET") == 0)
    {
		sscanf(cmd, "MAP.FLOOR.OFFSET %f", &Map.fFloorOffset);
	}
	if(strcmpi(cmdstr, "MAP.FOG.AFFECTSSKYBOX") == 0)
    {
	    sscanf(cmd, "MAP.FOG.AFFECTSSKYBOX %i", &Map.bFogAffectsSkybox);
    }
    if(strcmpi(cmdstr, "MAP.FOG.DENSITY") == 0)
    {
	    sscanf(cmd, "MAP.FOG.DENSITY %f", &Map.fFogDensity);
	    glFogf(GL_FOG_DENSITY, Map.fFogDensity);
    }
	if(strcmpi(cmdstr, "MAP.GRAVITY") == 0)
    {
		float g[3];
		sscanf(cmd, "MAP.GRAVITY %f %f %f", &g[0], &g[1], &g);
		Map.SetGravity(CVector3(g[0], g[1], g[2]));
		g_Manager_Physic.SetGravity(CVector3(g[0], g[1], g[2]));
	}
    if(strcmpi(cmdstr, "MAP.HEIGHTMAP") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.HEIGHTMAP %s", &Map.sFloorMap);
	    }
    }
    if(strcmpi(cmdstr, "MAP.LOAD") == 0)
    {
	    g_bLoading = true;

	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.LOAD %s", &buffer);
		    sprintf(buffer, "%s.map", buffer);
	    }

	    Map.Load(buffer);

		GUI.find("Mainform")->Hide();
		GUI.popup(&Gameform);
    }
    if(strcmpi(cmdstr, "MAP.NEW") == 0)
    {
	    Map.New();

	    //Console->Clear();
	    Console->setVisible(false);
    }
	if(strcmpi(cmdstr, "MAP.ROOF") == 0)
    {
		sscanf(cmd, "MAP.ROOF %s", &Map.sRoofMap);
	}
	if(strcmpi(cmdstr, "MAP.ROOF.OFFSET") == 0)
    {
		sscanf(cmd, "MAP.ROOF.OFFSET %f", &Map.fRoofOffset);
	}
    if(strcmpi(cmdstr, "MAP.SAVE") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.SAVE %s", &buffer);
	    }

	    Map.Save(buffer);
    }
    if(strcmpi(cmdstr, "MAP.SECONDTEXTURE") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "MAP.SECONDTEXTURE %s", &Map.sSecondTexture);
		    g_Manager_Texture->CreateTexture(Map.sSecondTexture);
	    }
    }
	if(strcmpi(cmdstr, "MAP.WATER.COLOR") == 0)
    {
		sscanf(cmd, "MAP.WATER.COLOR %f %f %f", &Map.vWaterColor.x, &Map.vWaterColor.y, &Map.vWaterColor.z);
	}
	if(strcmpi(cmdstr, "MAP.WATER.ENABLED") == 0)
    {
	    sscanf(cmd, "MAP.WATER.ENABLED %i", &Map.bWaterEnabled);
    }
	if(strcmpi(cmdstr, "MAP.WATER.FLOWSPEED") == 0)
    {
	    sscanf(cmd, "MAP.WATER.FLOWSPEED %f", &Map.fWaterFlow);
    }
	if(strcmpi(cmdstr, "MAP.WATER.HEIGHT") == 0)
    {
	    sscanf(cmd, "MAP.WATER.HEIGHT %f", &Map.fWaterHeightBase);
		Map.fWaterHeight = Map.fWaterHeightBase;
		Water.SetPosition(CVector3(0.f, Map.fWaterHeightBase, 0.f));
		Water.SetBoundingBox();
    }
	if(strcmpi(cmdstr, "MAP.WATER.NORMALS") == 0)
    {
	    sscanf(cmd, "MAP.WATER.NORMALS %i", &Map.bWaterNormals);
    }
	if(strcmpi(cmdstr, "MAP.WATER.REFLECTIONS") == 0)
    {
	    sscanf(cmd, "MAP.WATER.REFLECTIONS %i", &Map.bWaterReflections);
    }
	if(strcmpi(cmdstr, "MAP.WATER.REFRACTIONS") == 0)
    {
	    sscanf(cmd, "MAP.WATER.REFRACTIONS %i", &Map.bWaterRefractions);
    }
	if(strcmpi(cmdstr, "MAP.WATER.WAVEHEIGHT") == 0)
    {
		sscanf(cmd, "MAP.WATER.WAVEHEIGHT %f", &Map.fWaveHeight);
    }
	if(strcmpi(cmdstr, "MAP.WATER.WAVEINTERVAL") == 0)
    {
		sscanf(cmd, "MAP.WATER.WAVEINTERVAL %i", &Map.iWaveInterval);
    }
	if(strcmpi(cmdstr, "MAP.WIND") == 0)
    {
		sscanf(cmd, "MAP.WIND %f %f %f", &Map.vWind.x, &Map.vWind.y, &Map.vWind.z);
	}
	if(strcmpi(cmdstr, "MULTIPLAYER.SERVER") == 0)
	{
		sscanf(cmd, "MULTIPLAYER.SERVER %d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
		sprintf(sServerIP, "%d.%d.%d.%d", (int)ip[0], (int)ip[1], (int)ip[2], (int)ip[3]);
	}
	if(strcmpi(cmdstr, "OBJECT.ATTACH") == 0)
    {
		int child = -1;
		int parent = -1;
		
		sscanf(cmd, "OBJECT.ATTACH %d %d", &child, &parent);
		
		if(child > -1 && parent > -1)
			Scene.ObjectNodeAttach(child, parent);
	}
	if(strcmpi(cmdstr, "OBJECT.MATERIAL") == 0)
    {
	    sscanf(cmd, "OBJECT.MATERIAL %s", &buffer);
		g_Manager_Object.setMaterial(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.MESH") == 0)
    {
	    sscanf(cmd, "OBJECT.MESH %s", &buffer);
		g_Manager_Object.setMesh(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.MESHTYPE") == 0)
    {
	    sscanf(cmd, "OBJECT.MESHTYPE %s", &buffer);
		g_Manager_Object.setMeshType(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.NORMALMAP") == 0)
    {
	    sscanf(cmd, "OBJECT.NORMALMAP %s", &buffer);
		g_Manager_Object.setNormalMap(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.MOVE") == 0)
    {
		int id = -1;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.MOVE %d %f %f %f", &id, &x, &y, &z);
		if(id == -1)
			id = Scene.Count - 1;

		Scene.ObjectIDMove(id, CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "OBJECT.REMOVE") == 0)
    {
		int child = -1;
		int parent = -1;
		
		sscanf(cmd, "OBJECT.REMOVE %d %d", &child, &parent);
		
		if(child > -1 && parent > -1)
			Scene.ObjectNodeRemove(child, parent);
	}
	if(strcmpi(cmdstr, "OBJECT.ROTATE") == 0)
    {
		int id = -1;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.ROTATE %d %f %f %f", &id, &x, &y, &z);
		if(id == -1)
			id = Scene.Count - 1;

		Scene.ObjectIDRotate(id, CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "OBJECT.SETPOSITION") == 0)
    {
		int id = -1;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.SETPOSITION %d %f %f %f", &id, &x, &y, &z);
		if(id == -1)
			id = Scene.Count - 1;

		Scene.ObjectIDSetPosition(id, CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "OBJECT.SETROTATION") == 0)
    {
		int id = -1;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.SETROTATION %d %f %f %f", &id, &x, &y, &z);
		if(id == -1)
			id = Scene.Count - 1;

		Scene.ObjectIDSetRotation(id, CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "OBJECT.SHADER") == 0)
    {
	    sscanf(cmd, "OBJECT.SHADER %s", &buffer);
		g_Manager_Object.setShader(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.SPAWN") == 0)
    {
		if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "OBJECT.SPAWN %s", &buffer);
	    }
		else
			return -1;

		float x = 0.0f, y = 0.0f, z = 0.0f;

		try
		{
			if(strcmpi(cmd, cmdstr) != 0)
			{
				sscanf(cmd, "OBJECT.SPAWN %s %f %f %f", &buffer, &x, &y, &z);
			}
		}
		catch(...)
		{
			// warning C4702: unreachable code
		    //return -1;
		}

		g_Manager_Object.spawn(buffer);

		//Scene.Add(g_Manager_3DSObject.Mesh[g_Manager_3DSObject.Mesh.size() - 1]);
	}
	if(strcmpi(cmdstr, "OBJECT.TEXTURE") == 0)
    {
	    sscanf(cmd, "OBJECT.TEXTURE %s", &buffer);
		g_Manager_Object.setTexture(static_cast<string>(buffer));
	}
	if(strcmpi(cmdstr, "OBJECT.TIMED_MOVE") == 0)
    {
		int id = -1;
		int repeat = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.TIMED_MOVE %d %f %f %f %d", &id, &x, &y, &z, &repeat);
		if(id == -1)
			id = Scene.Count - 1;
		sprintf(buffer, "OBJECT.MOVE %d %f %f %f", id, x, y, z);

		Scene.TimedOperation_Add(buffer, repeat);
	}
	if(strcmpi(cmdstr, "OBJECT.TIMED_ROTATE") == 0)
    {
		int id = -1;
		int repeat = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "OBJECT.TIMED_ROTATE %d %f %f %f %d", &id, &x, &y, &z, &repeat);
		if(id == -1)
			id = Scene.Count - 1;
		sprintf(buffer, "OBJECT.ROTATE %d %f %f %f", id, x, y, z);

		Scene.TimedOperation_Add(buffer, repeat);
	}
	if(strcmpi(cmdstr, "OBJECT.TRIGGER") == 0)
    {
		int id = -1;
	
		sscanf(cmd, "OBJECT.TRIGGER %d %s", &id, &buffer);
		if(id == -1)
			id = Scene.Count - 1;

		sprintf(GetObjectByID(id)->go->sTriggerScript, "%s", buffer);
	}
	if(strcmpi(cmdstr, "PARTICLE.ALPHA") == 0)
    {
		float l = 0.0f;

		sscanf(cmd, "PARTICLE.ALPHA %f", &l);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetAlpha(l);
	}
	if(strcmpi(cmdstr, "PARTICLE.AREA") == 0)
    {
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "PARTICLE.AREA %f %f %f", &x, &y, &z);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetArea(CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "PARTICLE.BILLBOARD") == 0)
    {
		int billboard = 0;

		sscanf(cmd, "PARTICLE.BILLBOARD %d", &billboard);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetBillboarding(billboard);
	}
	if(strcmpi(cmdstr, "PARTICLE.BLEND") == 0)
    {
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetBlending();
	}
	if(strcmpi(cmdstr, "PARTICLE.COLOR") == 0)
    {
		CVector3 color;

		sscanf(cmd, "PARTICLE.COLOR %f %f %f", &color.x, &color.y, &color.z);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetColor(color);
	}
	if(strcmpi(cmdstr, "PARTICLE.FADEIN") == 0)
    {
		float l = 0.0f;

		sscanf(cmd, "PARTICLE.FADEIN %f", &l);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetFadeIn(l);
	}
	if(strcmpi(cmdstr, "PARTICLE.FADEOUT") == 0)
    {
		float l = 0.0f;

		sscanf(cmd, "PARTICLE.FADEOUT %f", &l);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetFadeOut(l);
	}
	if(strcmpi(cmdstr, "PARTICLE.GLOBALGRAVITY") == 0)
    {
		bool g = 0;

		sscanf(cmd, "PARTICLE.GLOBALGRAVITY %i", &g);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetGlobalGravity(g);
	}
	if(strcmpi(cmdstr, "PARTICLE.GRAVITY") == 0)
    {
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "PARTICLE.GRAVITY %f %f %f", &x, &y, &z);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetGravity(CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "PARTICLE.LIVECYCLE") == 0)
    {
		float l = 0.0f;

		sscanf(cmd, "PARTICLE.LIVECYCLE %f", &l);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetLifeCycle(l);
	}
	if(strcmpi(cmdstr, "PARTICLE.LOCK") == 0)
    {
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].Lock();
	}
	if(strcmpi(cmdstr, "PARTICLE.LOOP") == 0)
    {
		bool loop = false;

		sscanf(cmd, "PARTICLE.LOOP %d", &loop);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetLoop(loop);
	}
	if(strcmpi(cmdstr, "PARTICLE.MATERIAL") == 0)
    {
		char s[255] = "";

		sscanf(cmd, "PARTICLE.MATERIAL %s", &s);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetMaterial(s);
	}
	if(strcmpi(cmdstr, "PARTICLE.MAXPARTICLES") == 0)
    {
		DWORD m = 0;

		sscanf(cmd, "PARTICLE.MAXPARTICLES %d", &m);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetMaxParticles(m);
	}
	if(strcmpi(cmdstr, "PARTICLE.POLYGON") == 0)
    {
		bool p = 0;

		sscanf(cmd, "PARTICLE.POLYGON %d", &p);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetPolygonMode(p);
	}
	if(strcmpi(cmdstr, "PARTICLE.POSITION") == 0)
    {
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "PARTICLE.POSITION %f %f %f", &x, &y, &z);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetPosition(CVector3(x, y, z));
	}
	//if(strcmpi(cmdstr, "PARTICLE.POSITION2") == 0)
 //   {
	//	float x = 0.0f, y = 0.0f, z = 0.0f;

	//	sscanf(cmd, "PARTICLE.POSITION2 %f %f %f", &x, &y, &z);
	//	g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetPosition2(CVector3(x, y, z));
	//}
	if(strcmpi(cmdstr, "PARTICLE.RELEASE") == 0)
    {
		DWORD r = 0;

		sscanf(cmd, "PARTICLE.RELEASE %d", &r);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetNumToRelease(r);
	}
	if(strcmpi(cmdstr, "PARTICLE.RELEASEINTERVAL") == 0)
    {
		float r = 0.0f;

		sscanf(cmd, "PARTICLE.RELEASEINTERVAL %f", &r);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetReleaseInterval(r);
	}
	if(strcmpi(cmdstr, "PARTICLE.SIZE") == 0)
    {
		float s = 0.0f;

		sscanf(cmd, "PARTICLE.SIZE %f", &s);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetSize(s);
	}
	if(strcmpi(cmdstr, "PARTICLE.TEXTURE") == 0)
    {
		char s[255] = "";

		sscanf(cmd, "PARTICLE.TEXTURE %s", &s);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetTexture(s);
	}
	if(strcmpi(cmdstr, "PARTICLE.VELOCITY") == 0)
    {
		float x = 0.0f, y = 0.0f, z = 0.0f;

		sscanf(cmd, "PARTICLE.VELOCITY %f %f %f", &x, &y, &z);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetVelocity(CVector3(x, y, z));
	}
	if(strcmpi(cmdstr, "PARTICLE.VELOCITYVAR") == 0)
    {
		float v = 0.0f;

		sscanf(cmd, "PARTICLE.VELOCITYVAR %f", &v);
		g_Manager_ParticleSystem.Object[g_Manager_ParticleSystem.iActive].SetVelocityVar(v);
	}
    if(strcmpi(cmdstr, "PLAYER.ADD") == 0)
    {
	    if(g_Manager_Player.Count >= g_Manager_Player.iMaxPlayers)
		    return -1;

	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "PLAYER.ADD %s", &buffer);
	    }

		g_Manager_Player.Add(buffer);

		if(strlen(buffer) == 0)
			sprintf(buffer, "Male");

		g_Manager_Player.Player[g_Manager_Player.Count - 1].Spawn(buffer, true, 0.0f, 0.0f, 0.0f);
    }
    if(strcmpi(cmdstr, "PLAYER.MODEL") == 0)
    {
		if(!Game.bRunning)
			return 0;

	    sscanf(cmd, "PLAYER.MODEL %s", &Game.sPlayerModel);

	    g_Manager_Player.Player[g_iLocalPlayer].ChangeModel(Game.sPlayerModel);

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CLIENT_CHANGE_MODEL, g_iLocalPlayer, Game.sPlayerModel);

	    Net_SendMessage(buffer2, true);
    }
    if(strcmpi(cmdstr, "PLAYER.NAME") == 0)
    {
	    sscanf(cmd, "PLAYER.NAME %s", &g_Manager_Player.Player[g_iLocalPlayer].mName);

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CLIENT_CHANGE_NAME, g_iLocalPlayer, g_Manager_Player.Player[g_iLocalPlayer].mName);

	    Net_SendMessage(buffer2, true);
    }
	if(strcmpi(cmdstr, "PLAYER.SETACTIVE") == 0)
    {
	    sscanf(cmd, "PLAYER.SETACTIVE %i", &g_Manager_Player.iActive);
    }
	if(strcmpi(cmdstr, "PLAYER.SETPOSITION") == 0)
    {
		float wx, wz;
		sscanf(cmd, "PLAYER.SETPOSITION %f %f", &wx, &wz);

		g_Manager_Player.Player[g_Manager_Player.iActive].SetPosition(CVector3(wx, 0.0f, wz));
    }
    if(strcmpi(cmdstr, "PLAYER.TEAM") == 0)
    {
	    sscanf(cmd, "PLAYER.TEAM %i", &g_Manager_Player.Player[g_iLocalPlayer].iTeam);

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%i", CLIENT_CHANGE_TEAM, g_iLocalPlayer, g_Manager_Player.Player[g_iLocalPlayer].iTeam);

	    Net_SendMessage(buffer2, true);
    }
    if(strcmpi(cmdstr, "QUIT") == 0)
    {
	    if(peer != NULL)
		    peer->CloseConnection(peer->GetSystemAddressFromIndex(0), true, 0);
    	
	    PostQuitMessage(0);
    }
	if(strcmpi(cmdstr, "REGION.ADD") == 0)
    {
		bool  b  = false;
		float x1 = 0.0f;
		float y1 = 0.0f;
		float z1 = 0.0f;
		float x2 = 0.0f;
		float y2 = 0.0f;
		float z2 = 0.0f;

	    sscanf(cmd, "REGION.ADD %d %f %f %f %f %f %f %s", &b, &x1, &y1, &z1, &x2, &y2, &z2, &buffer);
		g_Manager_Region.Add(b, x1, y1, z1, x2, y2, z2, buffer);
    }
	if(strcmpi(cmdstr, "REGION.LOCK") == 0)
	{
		g_Manager_Region.LockActive();
	}
    if(strcmpi(cmdstr, "REMOTE") == 0)
    {
	    if(strlen(cmd) > 7)
	    {
		    for(unsigned int i = 7; i < strlen(cmd); i += 1)
			    sprintf(buffer, "%s%c", buffer, cmd[i]);
	    }
    	
	    char buffer2[255];
	    sprintf(buffer2, "%i|%i|%s", REMOTE_COMMAND_CLIENT, g_iLocalPlayer, buffer);
	    Net_SendMessage(buffer2, true);
    }
	if(strcmpi(cmdstr, "ROOF.LOAD") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "ROOF.LOAD %s", &buffer);
	    }

		QuadTree.LoadRoof(buffer);
		sprintf(Map.sRoofMap, "%s", buffer);

		Console->setVisible(false);
    }
    if(strcmpi(cmdstr, "SAY") == 0)
    {
	    if(strlen(cmd) < 4)
		    return -1;

	    for(unsigned int i = 4; i < strlen(cmd); i += 1)
	    {
		    sprintf(buffer, "%s%c", buffer, cmd[i]);
	    }
	    sprintf(buffer, "%s\0", buffer);

	    g_Manager_Chat.Add(buffer, g_Manager_Player.Player[g_iLocalPlayer].mName);

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CHAT_MESSAGE, g_iLocalPlayer, buffer);

	    Net_SendMessage(buffer2, true);
    }
	if(strcmpi(cmdstr, "SAYTEAM") == 0)
    {
	    if(strlen(cmd) < 4)
		    return -1;

	    for(unsigned int i = 8; i < strlen(cmd); i += 1)
	    {
		    sprintf(buffer, "%s%c", buffer, cmd[i]);
	    }
	    sprintf(buffer, "%s\0", buffer);

	    g_Manager_Chat.Add(buffer, g_Manager_Player.Player[g_iLocalPlayer].mName);

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%i|%s\0", CHAT_MESSAGE_TEAM, g_iLocalPlayer, g_Manager_Player.Player[g_iLocalPlayer].iTeam, buffer);

	    Net_SendMessage(buffer2, true);
    }
    if(strcmpi(cmdstr, "SCRIPT.EXECUTE") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "SCRIPT.EXECUTE %s", &buffer);

		    g_Manager_Script.Execute(buffer, true, true);
	    }
    }
    if(strcmpi(cmdstr, "SCRIPT.EXECUTECOMMAND") == 0)			// muss noch überarbeitet werden!
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "SCRIPT.EXECUTECOMMAND %s", &buffer);
			
			ProcessCommand(buffer);
	    }
    }
    if(strcmpi(cmdstr, "SERVER") == 0)
    {
	    if(strlen(cmd) > 7)
	    {
		    for(unsigned int i = 7; i < strlen(cmd); i += 1)
			    sprintf(buffer, "%s%c", buffer, cmd[i]);
	    }
    	
	    char buffer2[255];
	    sprintf(buffer2, "%i|%i|%s", REMOTE_COMMAND_SERVER, g_iLocalPlayer, buffer);
	    Net_SendMessage(buffer2, true);
    }
	if(strcmpi(cmdstr, "SERVER.MAPCHANGE") == 0)
    {
		g_bLoading = true;

		if(strcmpi(cmd, cmdstr) != 0)
		{
			sscanf(cmd, "SERVER.MAPCHANGE %s", &buffer);
			sprintf(buffer, "%s.map", buffer);
		}

		sprintf(cmd, "%i|%i|%s\0", REQUEST_CHANGE_MAP, g_iLocalPlayer, buffer);
		Net_SendMessage(cmd, true);

		//Map.Load(buffer);

		//g_bLoading = false;
		//Console->bVisible = false;
    }
    if(strcmpi(cmdstr, "SERVER.SHOWGAMES") == 0)
    {
		sprintf(buffer, "%i", MS_REQUEST_RUNNINGGAMES);
		Net_SendMessage(buffer, true);

		iServerInstances = 0;
    }
	if(strcmpi(cmdstr, "SOUND.PLAY") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "SOUND.PLAY %s", &buffer);

		    int iSound = g_Manager_Sound.Add(buffer, false);
			g_Manager_Sound.PlaySoundByID(iSound);
	    }
    }
	if(strcmpi(cmdstr, "SOUND.PLAYLOOP") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "SOUND.PLAYLOOP %s", &buffer);

		    int iSound = g_Manager_Sound.Add(buffer, true);
			g_Manager_Sound.PlaySoundByID(iSound);
	    }
    }
	if(strcmpi(cmdstr, "SOUND.STOPALL") == 0)
    {
	    g_Manager_Sound.Clear();
    }
	if(strcmpi(cmdstr, "STATICOBJECT.SETACTIVE") == 0)
    {
	    sscanf(cmd, "STATICOBJECT.SETACTIVE %i", &g_Manager_StaticObject.iActive);
    }
    if(strcmpi(cmdstr, "STATISTICS.SORTBY") == 0)
    {
	    int iSortBy = 3;
	    sscanf(cmd, "STATISTICS.SORTBY %i", &iSortBy);

	    g_Manager_Statistic.SortByColumn(iSortBy);
    }
	if(strcmpi(cmdstr, "TELL") == 0)
    {
	    if(strlen(cmd) < 4)
		    return -1;

	    for(unsigned int i = 4; i < strlen(cmd); i += 1)
	    {
		    sprintf(buffer, "%s%c", buffer, cmd[i]);
	    }
	    sprintf(buffer, "%s\0", buffer);

	    g_Manager_Chat.Add(buffer, "");

	    if(peer == NULL)
		    return 0;

	    char buffer2[255] = "";
	    sprintf(buffer2, "%i|%i|%s\0", CHAT_MESSAGE, g_iLocalPlayer, buffer);

	    Net_SendMessage(buffer2, true);
    }
    if(strcmpi(cmdstr, "TERRAIN.LOAD") == 0)
    {
	    if(strcmpi(cmd, cmdstr) != 0)
	    {
		    sscanf(cmd, "TERRAIN.LOAD %s", &buffer);
	    }

		QuadTree.LoadFloor(buffer);
		sprintf(Map.sFloorMap, "%s", buffer);

		Console->setVisible(false);
    }
	if(strcmpi(cmdstr, "TEXTURE.ANSITROPIC") == 0)
    {
		sscanf(cmd, "TEXTURE.ANSITROPIC %f", &Engine->fAnsitropicFilter);
		g_Manager_Texture->Apply();
    }
	if(strcmpi(cmdstr, "TEXTURE.APPLY") == 0)
    {
	    g_Manager_Texture->Apply();
    }
	if(strcmpi(cmdstr, "TEXTURE.COMPRESS") == 0)
	{
		sscanf(cmd, "TEXTURE.COMPRESS %i", &Engine->bCompressedTextures);
	}
	if(strcmpi(cmdstr, "TEXTURE.FILTER") == 0)
    {
	     sscanf(cmd, "TEXTURE.FILTER %i", &g_Manager_Texture->iFilter);
		 g_Manager_Texture->Apply();
    }
	if(strcmpi(cmdstr, "TEXTURE.SHOW") == 0)
    {
		sprintf(buffer, "TEXTURE.FILTERMAX = %f", g_Manager_Texture->iMaxFilter[g_Manager_Texture->iFilter]);
		Console->Output(buffer);

		sprintf(buffer, "TEXTURE.FILTERMIN = %f", g_Manager_Texture->iMinFilter[g_Manager_Texture->iFilter]);
		Console->Output(buffer);
    }

	return 0;
}

void ProcessKeyPress(WPARAM Key)
{
	if(Console->IsVisible())
    {
		Console->KeyPressed(Key);
		return;
    }

	Engine->Keybinding.kbRun.bPressed = ((GetKeyState(Engine->Keybinding.kbRun.uKey) & 0x80) == 0);

    if(Game.bPlayMode)
    {
		if(Mainform.IsVisible())
			ProcessKeyPress_Menu(Key);
		else
			ProcessKeyPress_Game(Key);
    }
    else
    {
		ProcessKeyPress_Editor(Key);
    }
}

void ProcessKeyPress_Editor(WPARAM Key)
{
    char buffer[255] = "";

    switch(Key)
    {
    case VK_ADD:
	    switch(Editor->GetMode())
	    {
	    case EditorSpace::Selection:
			Scene.ObjectRotate(CVector3(0.0f, 1.0f, 0.0f));
			break;
		case EditorSpace::md2:
			Scene.ObjectScale(0.1f);
			break;
	    case EditorSpace::Fog:
		    Map.fFogDensity += 0.000005f;						// Increase the fog density
		    glFogf(GL_FOG_DENSITY, Map.fFogDensity);
		    break;
	    case EditorSpace::Water:
		    Map.fWaterHeightBase += 0.1f;
		    break;
	    case EditorSpace::Light:
		    CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.y += 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
    case VK_BACK:
	    break;
    case VK_DELETE:
	    switch(Editor->GetMode())
	    {
	    case EditorSpace::Selection:
			Map.DeleteIndex(Scene.Delete());
			break;
	    }
	    break;
    case VK_DOWN:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Selection:
			Scene.ObjectMove(CVector3(0.0f, 0.0f, 1.0f));
		    break;
	    case EditorSpace::Light:
		    CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.z += 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
    case VK_ESCAPE:
		if(Editor->GetMode() != EditorSpace::Selection)
		{
			Editor->SetMode(EditorSpace::Selection);
			sprintf(g_sEditMode, "");
		}
		else
			Scene.iActive = -1;
	    break;
    case VK_F1:
		Editor->NextMode();
	    break;
    case VK_F2:
	    break;
    case VK_F3:
	    InsertIndex--;
	    if(InsertIndex < 0)
		    InsertIndex = 0;

	    switch(Editor->GetMode())
	    {
	    case EditorSpace::max3ds:
		    sprintf(g_sEditMode, "Mode-3DS");
		    break;
	    case EditorSpace::md2:
		    sprintf(g_sEditMode, "Mode-MD2");
		    break;
		case EditorSpace::Static:
			sprintf(g_sEditMode, "Mode-STATIC");
			break;
		case EditorSpace::Bot:
			sprintf(g_sEditMode, "Mode-BOD");
			break;
		case EditorSpace::Particle:
			sprintf(g_sEditMode, "Mode-PARTICLE");
			break;
		case EditorSpace::Terrain:
		    sprintf(g_sEditMode, "Mode-TERRAIN");
		    break;
	    }

	    if(g_Manager_File.filelist.size())
	    {
		    basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);

		    sprintf(buffer, "%s", objectname.data());
		    sprintf(g_sEditMode, "%s( %s )", g_sEditMode, buffer);
	    }
	    break;
    case VK_F4:
	    InsertIndex++;
	    if((unsigned)InsertIndex >= g_Manager_File.filelist.size())
		    InsertIndex = g_Manager_File.filelist.size() - 1;

	    switch(Editor->GetMode())
	    {
	    case EditorSpace::max3ds:
		    sprintf(g_sEditMode, "Mode-3DS");
		    break;
	    case EditorSpace::md2:
		    sprintf(g_sEditMode, "Mode-MD2");
		    break;
		case EditorSpace::Static:
			sprintf(g_sEditMode, "Mode-STATIC");
			break;
		case EditorSpace::Bot:
			sprintf(g_sEditMode, "Mode-BOT");
			break;
		case EditorSpace::Particle:
			sprintf(g_sEditMode, "Mode-PARTICLE");
			break;
		case EditorSpace::Terrain:
		    sprintf(g_sEditMode, "Mode-TERRAIN");
		    break;
	    }

	    if(g_Manager_File.filelist.size())
	    {
		    basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);

		    sprintf(buffer, "%s", objectname.data());
		    sprintf(g_sEditMode, "%s( %s )", g_sEditMode, buffer);
	    }
	    break;
    case VK_F5:
	    Game.bPlayMode = true;
		Engine->bClipping = true;
		Scene.iActive = -1;

		if(g_iLocalPlayer != -1)
		{
			g_Camera.SetPosition(Game.mPlayer->mPosition);
			g_Camera.SetView(Game.mPlayer->vView);
		}

		ShowCursor(FALSE);
	    break;
    case VK_F6:
	    break;
    case VK_F7:
	    break;
    case VK_F8:
	    break;
    case VK_F9:
		Console->setVisible(true);
	    break;
    case VK_F10:
	    break;
    case VK_F11:
		Scene.ObjectSelectPrevious();
	    break;
    case VK_F12:
		Scene.ObjectSelectNext();
	    break;
    case VK_LEFT:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Selection:
			Scene.ObjectMove(CVector3(-1.0f, 0.0f, 0.0f));
		    break;
	    case EditorSpace::Light:
			CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.x -= 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
    case VK_RETURN:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Light:
			if(Light[g_iLightSource - 1].getEnabled())
				Light[g_iLightSource - 1].disable();
			else
				Light[g_iLightSource - 1].enable();
		    break;
		case EditorSpace::Terrain:
			{
				//g_Manager_3DSObject.Count = 0;
				//g_Manager_Entity.Count = 0;

				sprintf(buffer, "%sTerrain", g_sGameName);
				g_Manager_File.Searchfiles(buffer, "tga");

				basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);

				if(Engine->bDrawRoof)
				{
					sprintf(Map.sRoofMap, "%s", objectname.data());
					QuadTree.LoadRoof(Map.sRoofMap);
				}
				else
				{
					sprintf(Map.sFloorMap, "%s", objectname.data());
					QuadTree.LoadFloor(Map.sFloorMap);
				}
			}
		    break;
	    }
	    break;
    case VK_RIGHT:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Selection:
			Scene.ObjectMove(CVector3(1.0f, 0.0f, 0.0f));
		    break;
	    case EditorSpace::Light:
		    CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.x += 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
    case VK_SPACE:
	    break;
    case VK_SUBTRACT:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Selection:
			Scene.ObjectRotate(CVector3(0.0f, -1.0f, 0.0f));
			break;
		case EditorSpace::md2:
			Scene.ObjectScale(-0.1f);
			break;
	    case EditorSpace::Fog:
		    Map.fFogDensity -= 0.000005f;						// Increase the fog density
		    if (Map.fFogDensity < 0.0f)
			    Map.fFogDensity = 0.0f;
		    glFogf(GL_FOG_DENSITY, Map.fFogDensity);
		    break;
	    case EditorSpace::Water:
		    Map.fWaterHeightBase -= 0.1f;
		    break;
	    case EditorSpace::Light:
		    CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.y -= 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
    case VK_TAB:
	    break;
    case VK_UP:
	    switch(Editor->GetMode())
	    {
		case EditorSpace::Selection:
			Scene.ObjectMove(CVector3(0.0f, 0.0f, -1.0f));
		    break;
	    case EditorSpace::Light:
		    CVector3 pos = Light[g_iLightSource - 1].getPosition();
		    pos.z -= 50.0f;
			Light[g_iLightSource - 1].setPosition(pos);
		    break;
	    }
	    break;
	case '0':
		break;
	case '1':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 1;
		    break;
		}
		break;
	case '2':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 2;
		    break;
		}
		break;
	case '3':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 3;
		    break;
		}
		break;
	case '4':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 4;
		    break;
		}
		break;
	case '5':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 5;
		    break;
		}
		break;
	case '6':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 6;
		    break;
		}
		break;
	case '7':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 7;
		    break;
		}
		break;
	case '8':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			g_iLightSource = 8;
		    break;
		}
		break;
	case '9':
		break;
	case 'B':
		Engine->bLightModelTwoSided = !Engine->bLightModelTwoSided;
		Engine->SetLightModel(Engine->bLightModelTwoSided);
		break;
	case 'C':
		Engine->bColorMaterial = !Engine->bColorMaterial;
		break;
	case 'F':
		if(strlen(Map.sFloorMap) > 0)
			Engine->bDrawFloor = !Engine->bDrawFloor;
		else
			Engine->bDrawFloor = false;
		break;
	case 'L':
		switch(Editor->GetMode())
	    {
	    case EditorSpace::Light:
			switch(Light[g_iLightSource - 1].getMode())
			{
			case CLight::DirectionalLight:
				Light[g_iLightSource - 1].setMode(CLight::PointLight);
				break;
			case CLight::PointLight:
				Light[g_iLightSource - 1].setMode(CLight::SpotLight);
				break;
			case CLight::SpotLight:
				Light[g_iLightSource - 1].setMode(CLight::DirectionalLight);
				break;
			}
			break;

			Light[g_iLightSource - 1].setLight();
		}
		break;
	case 'R':
		if(strlen(Map.sRoofMap) > 0)
			Engine->bDrawRoof = !Engine->bDrawRoof;
		else
			Engine->bDrawRoof = false;
		break;
	case 'T':
		Editor->SetMode(EditorSpace::Rotation);
		break;
	case 'W':
		Engine->bRenderWayPoints = !Engine->bRenderWayPoints;
		break;
	case 'X':
		Editor->bAllowMoveX = !Editor->bAllowMoveX;
		break;
	case 'Y':
		Editor->bAllowMoveY = !Editor->bAllowMoveY;
		break;
	case 'Z':
		Editor->bAllowMoveZ = !Editor->bAllowMoveZ;
		break;
    default:
	    break;
    }
}

void ProcessKeyPress_Game(WPARAM Key)
{
	switch(Key)
	{
	case VK_ADD:
		Scene.ObjectScale(0.01f);
		break;
	case VK_ESCAPE:
		ShowCursor(TRUE);
		GUI.popup(&Mainform);
		break;
	case VK_F1:
		break;
	case VK_F2:
		break;
	case VK_F3:
		if(g_Manager_Entity.Count > 0)
		{
			if(g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Model.currentAnim > 0)
				g_Manager_Entity.Object[g_Manager_Entity.Count - 1].SetAnimation(--g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Model.currentAnim);
		}
		break;
	case VK_F4:
		if(g_Manager_Entity.Count > 0)
		{
			if(g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Model.currentAnim < g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Model.numOfAnimations)
				g_Manager_Entity.Object[g_Manager_Entity.Count - 1].SetAnimation(++g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Model.currentAnim);
		}
		break;
	case VK_F5:
		if(!Game.bAllowEditor)
			break;

		ShowCursor(TRUE);

		Game.bPlayMode = false;
		Engine->bClipping = false;
		break;
	case VK_F6:
		break;
	case VK_F7:
		Game.Load("QuickSave.sav");
		break;
	case VK_F8:
		Game.Save("QuickSave.sav");
		break;
	case VK_F9:
		Console->setVisible(true);
		break;
	case VK_F10:
		break;
	case VK_F11:
		break;
	case VK_F12:
		makeScreenShot("Screen.tga", Engine->Screen.Width, Engine->Screen.Height);
		break;
	case VK_PRINT:
		makeScreenShot("Screen.tga", Engine->Screen.Width, Engine->Screen.Height);
		break;
	case VK_RETURN:
		break;
	case VK_SPACE:
		{
			// Push object
			Mouse.mp3d = GetClientMousePosition3D();

			CVector3 push = Normalize(g_Camera.View());
			push.x *= 10.0f;
			push.y *= 10.0f;
			push.z *= 10.0f;
			CSceneObject* scobj = Scene.GetObjectByID(Scene.ObjectHover(Mouse.mp3d));
			
			if(scobj != NULL)
				scobj->go->ApplyForce(Physic::Push, push);
		}
		break;
	case VK_SUBTRACT:
		Scene.ObjectScale(-0.01f);
		break;
	case VK_TAB:
		Engine->bShowStats = !Engine->bShowStats;
		break;
	case '1':
		switch(Game.iGameMode)
		{
		case MODE_GAME_COMMAND:
			Game.iGameMode_Command = MODE_GAME_COMMAND_SENDBOT;
			break;
		default:
			Game.mPlayer->WeaponSwitch("92fs.wpn");
			break;
		}
		break;
	case '2':
		Game.mPlayer->WeaponSwitch("Shotgun.wpn");
		break;
	case '3':
		Game.mPlayer->WeaponSwitch("Q3Shotgun.wpn");
		break;
	case '4':
		Game.mPlayer->WeaponSwitch("Q3Plasmagun.wpn");
		break;
	case '5':
		Game.mPlayer->WeaponSwitch("Minigun.wpn");
		break;
	case '6':
		Game.mPlayer->WeaponSwitch("Mauser.wpn");
		break;
	case '7':
		Game.mPlayer->WeaponSwitch("MG16.wpn");
		break;
	case '8':
		Game.mPlayer->WeaponSwitch("Q3RocketLauncher.wpn");
		break;
	case '9':
		Game.mPlayer->WeaponSwitch("BigGun.wpn");
		break;
	case '0':
		//Game.mPlayer->WeaponSwitch("MG16.wpn");
		break;
	case 'B':
		bTeleport = true;
		break;
	//case 'C':
	//	Game.iGameMode = MODE_GAME_COMMAND;
	//	break;
	case 'C':
		Engine->bColorMaterial = !Engine->bColorMaterial;
		break;
	case 'G':
		if(Light[LIGHT_FLASHLIGHT].getEnabled())
			Light[LIGHT_FLASHLIGHT].disable();
		else
			Light[LIGHT_FLASHLIGHT].enable();
		break;
	case 'I':
		Engine->bClipping = !Engine->bClipping;
		break;
	case 'R':
		Game.mPlayer->Reload();
		break;
	default:
		break;
	}
}

void ProcessKeyPress_Menu(WPARAM Key)
{
	switch(Key)
    {
    case VK_ADD:
	    break;
    case VK_BACK:
	    break;
    case VK_DELETE:
	    break;
    case VK_DOWN:
	    break;
    case VK_ESCAPE:
		GUI.popback(&Mainform);
	    break;
    case VK_F1:
	    break;
    case VK_F2:
	    break;
    case VK_F3:
	    break;
    case VK_F4:
	    break;
    case VK_F5:
	    break;
    case VK_F6:
	    break;
    case VK_F7:
	    break;
    case VK_F8:
	    break;
    case VK_F9:
		Console->setVisible(true);
	    break;
    case VK_F10:
	    break;
    case VK_F11:
	    break;
    case VK_F12:
	    break;
    case VK_LEFT:
	    break;
    case VK_RETURN:
	    break;
    case VK_RIGHT:
	    break;
    case VK_SPACE:
	    break;
    case VK_SUBTRACT:
	    break;
    case VK_TAB:
	    break;
    case VK_UP:
	    break;
    default:
	    break;
    }
}

void ProcessNetMessage(RPCParameters *rpcParameters)
{
    if(!rpcParameters->input)
	    return;

    int iMsgID = -1;
    int iPlayer = -1;

    int i = 0;

    char buffer[255] = "";

    while(rpcParameters->input[i])
    {
	    sprintf(buffer, "%s%c", buffer, rpcParameters->input[i]);
	    i++;
    }

    sscanf_s(buffer, "%i", &iMsgID);

    switch(iMsgID)
    {
	case MS_ANSWER_SERVERINFO:
		{
			int mav = -1;
			int miv = -1;
			// MessageID | ServerVersionMajor | ServerVersionMinor
			sscanf(buffer, "%i|%i|%i", &iMsgID, &mav, &miv);

			sprintf(buffer, "Server Version: %i.%i", mav, miv);
			Console->Output(buffer);
		}
		break;
	case MS_ANSWER_RUNNINGGAMES:
		{
		int iServerIPCount = 0;
		int iMapNameCount = 0;
		char msg[255] = "";

		Console->Output(buffer);

		// MessageID | ServerPort | ServerIPLength | MapNameLength | GameType | ServerIP | MapName
		sscanf(buffer, "%i|%i|%i|%i|%i|%s", &iMsgID, &ServerInstance[iServerInstances].iServerPort, &iServerIPCount, &iMapNameCount, &ServerInstance[iServerInstances].iGameType, &msg);
	    
		ServerInstance[iServerInstances].hHandle = 0;
		sprintf(ServerInstance[iServerInstances].sMapName, "");
		sprintf(ServerInstance[iServerInstances].sServerIP, "");

		i = 0;
	    while(msg[i] != '|')
	    {
			sprintf(ServerInstance[iServerInstances].sServerIP, "%s%c", ServerInstance[iServerInstances].sServerIP, msg[i]);
		    i++;
	    }
	    i++;

		while(msg[i] != '|' && msg[i] != '\0')
	    {
			sprintf(ServerInstance[iServerInstances].sMapName, "%s%c", ServerInstance[iServerInstances].sMapName, msg[i]);
		    i++;
	    }

		iServerInstances++;
		}
		break;
    case ANSWER_UNIQUEID:
	    {
	    // MessageID | InvolvedPlayer | ID
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    sscanf(buffer, "%i|%i|%i", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].mId);

	    char playername[255] = "";
	    sprintf(playername, "PLAYER.NAME %s", Game.sPlayerName);
	    ProcessCommand(playername);

		sprintf(buffer, "%i|%i", REQUEST_GAMEINFO, iPlayer);
		Net_SendMessage(buffer, false);
		//peer->RPC("ProcessNetMessage", buffer, (BitSize_t)((strlen(buffer) + 1) * 8), HIGH_PRIORITY, RELIABLE_ORDERED, 0, saServer, false, 0, UNASSIGNED_NETWORK_ID, 0);
	    }
	    break;
    case REQUEST_PLAYERINFO:
	    // MessageID | InvolvedPlayer | ID | Kills | Deaths | Name
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		if(iPlayer < 0)
			break;

	    if(iPlayer > g_Manager_Player.Count - 1)
	    {
		    g_Manager_Player.Add("Player");
		    g_Manager_Player.Player[g_Manager_Player.Count - 1].Spawn("Rhino", 0.0f, 0.0f, 0.0f, 1.0f);
	    }

	    sscanf(buffer, "%i|%i|%i|%i|%i|%s", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].mId, &g_Manager_Player.Player[iPlayer].iKills, &g_Manager_Player.Player[iPlayer].iDeaths, &g_Manager_Player.Player[iPlayer].mName);
	    g_Manager_Statistic.Manage();
	    break;
    case CLIENT_LOCALPLAYER:
	    {
	    // MessageID | InvolvedPlayer [| NewPlayerIndex]
	    Game.mPlayer = NULL;
	    g_Manager_Player.Clear();

	    int newplayerid = -1;

	    try
	    {
		    sscanf(buffer, "%i|%i|%i", &iMsgID, &iPlayer, &newplayerid);
	    }
	    catch(...)
	    {
			// warning C4702: unreachable code
		    //sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    }

		if(iPlayer < 0)
			break;

	    g_iLocalPlayer = iPlayer - 1;
	    if(newplayerid != -1)
		    g_iLocalPlayer = newplayerid;

	    for(i = 0; i < g_iLocalPlayer; i += 1)
	    {
		    char playername[255] = "Player";
		    sprintf(buffer, "%s%i", playername, i);

		    g_Manager_Player.Add(buffer);
		    g_Manager_Player.Player[i].Spawn("Male", false, 0.0f, 0.0f, 0.0f);
	    }

		if(g_iLocalPlayer < 0)
			break;

		g_Manager_Player.Add(Game.sPlayerName);
		Game.mPlayer = &g_Manager_Player.Player[g_iLocalPlayer];

	    if(newplayerid == -1)
	    {
		    sprintf(buffer, "%i|%i", REQUEST_UNIQUEID, g_iLocalPlayer);
		    Net_SendMessage(buffer, false);
	    }

	    g_Manager_Statistic.Manage();

		Game.bIsServer = false;
	    }
	    break;
    case CLIENT_POSITION_XYZ:
	    // MessageID | InvolvedPlayer | PositionX | PositionY | PositionZ
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    sscanf(buffer, "%i|%i|%f|%f|%f", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].mPosition.x, &g_Manager_Player.Player[iPlayer].mPosition.y, &g_Manager_Player.Player[iPlayer].mPosition.z);
	    break;
    case CLIENT_VIEW_XYZ:
	    // MessageID | InvolvedPlayer | ViewX | ViewY | ViewZ
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		if(iPlayer < 0)
			break;

	    if(iPlayer != g_iLocalPlayer)
		    sscanf(buffer, "%i|%i|%f|%f|%f", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].vView.x, &g_Manager_Player.Player[iPlayer].vView.y, &g_Manager_Player.Player[iPlayer].vView.z);
	    break;
    case CLIENT_VIEWANGLE_XYZ:
	    // MessageID | InvolvedPlayer | fAngleX | fAngleY | fAngleZ
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		if(iPlayer < 0)
			break;

	    sscanf(buffer, "%i|%i|%f|%f|%f", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].mRotation.x, &g_Manager_Player.Player[iPlayer].mRotation.y, &g_Manager_Player.Player[iPlayer].mRotation.z);
	    break;
    case CLIENT_CHANGE_NAME:
	    // MessageID | InvolvedPlayer | PlayerID | PlayerName
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
		
		if(iPlayer < 0)
			break;

	    sscanf(buffer, "%i|%i|%i|%s", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].mId, &g_Manager_Player.Player[iPlayer].mName);

	    for(i = g_Manager_Player.Count - 1; i < iPlayer; i += 1)
	    {
		    sprintf(buffer, "PLAYER.ADD %s", g_Manager_Player.Player[iPlayer].mName);
		    ProcessCommand(buffer);
	    }
	    break;
    case CLIENT_CHANGE_TEAM:
	    // MessageID | InvolvedPlayer | PlayerTeam
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		if(iPlayer < 0)
			break;

	    sscanf(buffer, "%i|%i|%i", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].iTeam);
	    break;
    case CLIENT_CHANGE_MODEL:
	    // MessageID | FromPlayer | NewModel
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		if(iPlayer < 0)
			break;

	    sscanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &g_Manager_Player.Player[iPlayer].cModel);

	    g_Manager_Player.Player[iPlayer].ChangeModel(g_Manager_Player.Player[iPlayer].cModel);
	    break;
	case CLIENT_CHANGE_WEAPON:
		// MessageID | FromPlayer | NewWeapon
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
		sscanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &buffer);

		if(iPlayer < 0)
			break;

		g_Manager_Player.Player[iPlayer].Weapon.iModel = g_Manager_3DSObject.Add(buffer);
		break;
    case CHAT_MESSAGE:
	    {
	    // MessageID | FromPlayer | Message
		sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    char msg[255] = "";
	    scanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &msg);

	    i = 0;
	    while(buffer[i] != '|')
	    {
		    i++;
	    }
	    i++;

	    while(buffer[i] != '|')
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }
	    sscanf(msg, "%i", &iPlayer);
	    sprintf(msg, "");
	    i++;

	    while((unsigned)i < strlen(buffer))
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }

	    if(strlen(msg) > 0)
		    g_Manager_Chat.Add(msg, g_Manager_Player.Player[iPlayer].mName);
	    }
	    break;
	case CHAT_MESSAGE_TEAM:
		{
	    // MessageID | FromPlayer | ToTeam | Message
		sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    char msg[255] = "";
		int iTeam = -1;
	    scanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &iTeam, &msg);

	    i = 0;
	    while(buffer[i] != '|')
	    {
		    i++;
	    }
	    i++;

	    while(buffer[i] != '|')
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }
	    sscanf(msg, "%i", &iTeam);
	    sprintf(msg, "");
	    i++;

	    while((unsigned)i < strlen(buffer))
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }

	    if(strlen(msg) > 0 && iTeam == Game.mPlayer->iTeam)
		    g_Manager_Chat.Add(msg, g_Manager_Player.Player[iPlayer].mName);
	    }
		break;
    case REQUEST_CHANGE_MAP:
	    {
	    // MessageID | ToPlayer | Mapname
		sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    sscanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &Game.sMapName);

		//if(iPlayer != g_iLocalPlayer)
		Map.Load(Game.sMapName);
		}
	    break;
	case ANSWER_GAMEINFO:
		{
		// MessageID | game type | map name
		sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);

		char newmap[60] = "";
	    sscanf(buffer, "%i|%i|%s", &iMsgID, &Game.iGameType, &newmap);

		if(strcmpi(newmap, "") != 0)
		{
			if(strcmpi(Game.sMapName, newmap) != 0)
			{
				Map.Load(newmap);
			}
		}
		}
		break;
    case CLIENT_BULLET:
	    // MessageID | FromPlayer
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
    	
	    g_Manager_Player.Player[iPlayer].Shoot(true);
	    break;
    case CLIENT_SHOOT_BEGIN:
	    // MessageID | FromPlayer
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    g_Manager_Player.Player[iPlayer].bShoot = true;
	    break;
    case CLIENT_SHOOT_END:
	    // MessageID | FromPlayer
	    sscanf(buffer, "%i|%i", &iMsgID, &iPlayer);
	    g_Manager_Player.Player[iPlayer].bShoot = false;
	    break;
    case CLIENT_GET_HIT:
	    // MessageID | ToPlayer | FromPlayer
	    int iFromPlayer;
	    sscanf(buffer, "%i|%i|%i", &iMsgID, &iPlayer, &iFromPlayer);

	    if(!g_Manager_Player.Hit(iPlayer, iFromPlayer, true))
	    {
		    sprintf(buffer, "%i|%i", CLIENT_SHOTBY, iPlayer);
		    Net_SendMessage(buffer, true);

		    g_Manager_Statistic.Manage();
	    }
	    break;
    case REMOTE_COMMAND_CLIENT:
		{
	    // MessageID | FromPlayer | RemoteCommand
		char msg[200] = "";
	    scanf(buffer, "%i|%i|%s", &iMsgID, &iPlayer, &msg);

	    i = 0;
	    while(buffer[i] != '|')
	    {
		    i++;
	    }
	    i++;

	    while(buffer[i] != '|')
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }
	    sscanf(msg, "%i", &iPlayer);
	    sprintf(msg, "");
	    i++;

	    while((unsigned)i < strlen(buffer))
	    {
		    sprintf(msg, "%s%c", msg, buffer[i]);
		    i++;
	    }

	    if(strlen(msg) > 0)
		    ProcessCommand(msg);
	    }
	    break;
    }
}

void SetVSyncState(bool enable)
{
	if(enable)
		glSwapIntervalEXT(1);
	else
		glSwapIntervalEXT(0);
}

GLuint Texture_Add(char* Filename)
{
	return g_Manager_Texture->CreateTexture(Filename);
}

GLuint Texture_IndexOf(char* Filename)
{
	return g_Manager_Texture->IndexOf(Filename);
}

void Texture_SetActive(char* Filename)
{
	g_Manager_Texture->SetActiveTexture(Filename);
}

void Texture_SetActiveID(int id)
{
	g_Manager_Texture->SetActiveTextureID(id);
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT	ps;
	CVector3	vPos;
	//char		buffer[255];

    switch (uMsg)
	{
	case WM_SYSCOMMAND:												// Intercept System Commands
		{
			switch (wParam)											// Check System Calls
			{
				case SC_SCREENSAVE:									// Screensaver Trying To Start?
				case SC_MONITORPOWER:								// Monitor Trying To Enter Powersave?
				return 0;											// Prevent From Happening
			}
			break;
		}
		break;
	case WM_ACTIVATEAPP:
		{
			g_bActiveApp = !g_bActiveApp;
		}
		break;
	case WM_MOVE:
		{
			RECT rWindow;
			GetWindowRect(hWnd, &rWindow);
			Engine->Screen.Middle.x = rWindow.left + Engine->Screen.Width / 2;
			Engine->Screen.Middle.y = rWindow.top + Engine->Screen.Height / 2;
		}
		break;
    case WM_SIZE:													// If the window is resized
		{
			if(!Engine->bFullScreen)								// Do this only if we are NOT in full screen
			{
				SizeOpenGLScreen(LOWORD(lParam), HIWORD(lParam));	// LoWord=Width, HiWord=Height
				GetClientRect(hWnd, &g_rRect);						// Get the window rectangle

				Engine->Screen.Width = LOWORD(lParam);
				Engine->Screen.Height = HIWORD(lParam);
				Engine->Screen.Middle.x = Engine->Screen.Width / 2;
				Engine->Screen.Middle.y = Engine->Screen.Height / 2;

				Font.SetWidthHeight(LOWORD(lParam), HIWORD(lParam));
			}

			if(Engine->fMotionBlur > 0.0f)
			{
				glClearAccum(0.0, 0.0, 0.0, 1.0);
				glClear(GL_ACCUM_BUFFER_BIT);
			}
		}
        break;
	case WM_PAINT:													// If we need to repaint the scene
		{
			BeginPaint(hWnd, &ps);									// Init the paint struct		
			EndPaint(hWnd, &ps);									// EndPaint, Clean up
		}
		break;
	case WM_MOUSEMOVE:
		Dispatch_MouseMove();
		break;
	case WM_LBUTTONDOWN:
		Dispatch_MouseDownLeft();
		break;
	case WM_LBUTTONUP:
		Dispatch_MouseUpLeft();
		break;
	case WM_RBUTTONDOWN:								// If the right mouse button was clicked		
		Dispatch_MouseDownRight();
		break;
	case WM_RBUTTONUP:
		Dispatch_MouseUpRight();
		break;
	case WM_MBUTTONDOWN:
		Dispatch_MouseDownMiddle();
		break;
	case WM_MBUTTONUP:
		Dispatch_MouseUpMiddle();
		break;
	case WM_MOUSEWHEEL:
		break;
	case WM_KEYDOWN:
		ProcessKeyPress(wParam);
		break;
    case WM_CLOSE:										// If the window is closed

		if(peer != NULL)
			peer->CloseConnection(peer->GetSystemAddressFromIndex(0), true, 0);

        PostQuitMessage(0);								// Tell windows we want to quit

        break;
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Return the default
}