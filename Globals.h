
#pragma once

// Library includes

// Project includes
#include "GLWrapper.h"


#define MAX_SERVER_INSTANZES	4
#define MAX_CLIENTS				32
#define SERVER_PORT				27015


#define KEYBINDING_VERSION1			1
#define KEYBINDING_VERSION_ACTUAL	KEYBINDING_VERSION1


// Usable editmodes - Begin
//EDITMODE
#define MAXEDITMODES	10														// Usable editmodes (i.e. visible in editor)
#define _SELECTION		0
#define _3DS			1
#define _MD2			2
#define _STATIC			3
#define _BOT			4
#define _PARTICLE		5
#define _WATER			6
#define _FOG			7
#define _LIGHT			8
#define _TERRAIN		9
// Usable editmodes - End
// Hidden editmodes - Begin
#define EDITMODE_SCENE_NODE		10
// Hidden editmodes - End


#define REFLECTION_ID	0
#define REFRACTION_ID	1
#define NORMAL_ID		2
#define DUDVMAP_ID		3
#define DEPTH_ID		4
#define INGAME_ID		5

#define NUM_WATER_TEX		32
#define WATER_START_ID		5
#define WATER_END_ID WATER_START_ID + NUM_WATER_TEX

#define MAX_LODLEVELS			3

#define CAMERAOVERFLOOR_DEFAULT	10.0f
#define CAMERAOVERFLOOR_DUCK	6.0f


#define LIGHT_SUN			0
#define LIGHT_FLASHLIGHT	7


// Master server
#define MS_REQUEST_SERVERINFO			100
#define MS_ANSWER_SERVERINFO			101
#define MS_REQUEST_RUNNINGGAMES			200
#define MS_REQUEST_RUNNINGGAMES_STOP	201
#define MS_ANSWER_RUNNINGGAMES			202
// Connection issues
#define REQUEST_UNIQUEID				0x01
#define ANSWER_UNIQUEID					0x01
#define REQUEST_PLAYERINFO				0x02
// General game issues
#define SERVER_STARTGAME				0x03
// temporary movement
#define CLIENT_LOCALPLAYER				0x04
#define CLIENT_POSITION_XYZ				0x05
#define	CLIENT_VIEW_XYZ					0x06
#define CLIENT_VIEWANGLE_XYZ			0x07
#define CLIENT_CHANGE_NAME				10
#define CLIENT_CHANGE_TEAM				15
#define CLIENT_CHANGE_MODEL				50
#define CLIENT_CHANGE_WEAPON			60
// Game info
#define REQUEST_GAMEINFO				110
#define ANSWER_GAMEINFO					111
// Map issues
#define REQUEST_CHANGE_MAP				120
#define ANSWER_CHANGE_MAP				121
// Moving issues
#define CLIENT_MOVE_SPEED				300
#define CLIENT_STRAFE_SPEED				305
// Shooting issues
#define CLIENT_BULLET					400
#define CLIENT_SHOOT_BEGIN				405
#define CLIENT_SHOOT_END				406
#define CLIENT_GET_HIT					410
#define CLIENT_SHOTBY					415
// Chat issues
#define CHAT_MESSAGE					500
#define CHAT_MESSAGE_TEAM				505
// Remote server control
#define REMOTE_COMMAND_SERVER			600
// Remote client control
#define REMOTE_COMMAND_CLIENT			650



class CBot;
class CConsole;
class CGame;
namespace EngineSpace {
	class CEngine;
}
class CPlayerManager;
class CTextureManager;


extern CConsole*				Console;
extern CGame					Game;
extern EngineSpace::CEngine*	Engine;

extern CPlayerManager*			g_Manager_Player;
extern CTextureManager*			g_Manager_Texture;

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

// Global variables - Begin
// Engine
extern int			g_FrameCount;
extern float		g_halfWaterSize;
extern float		g_WaterUV;

extern const float	kCausticScale;
extern int			InsertIndex;
extern int			MeshID_SkyBox;

extern bool			g_bCompressedTextures;
extern bool			g_bInsertInWorld;

extern double		g_dCurTime;
extern double		g_dFrequency;
extern double		g_dLastTime;

extern float		g_fAnsitropicFilter;
extern float		g_fCameraAngleX;
extern float		g_fCameraAngleY;
extern float		g_fCameraAngleZ;
extern float		g_fCameraOverFloor;
extern float		g_fElpasedTime;
extern float		g_fFrameInterval;
extern float		g_fMouseSpeedDown;

//extern int			g_iEditMode;
extern float		g_iHeightMapScaleX;
extern float		g_iHeightMapScaleY;
extern float		g_iHeightMapScaleZ;
extern int			g_iMaxLODLevel;
extern int			g_iViewWidth;
extern int			g_iOctreeX;
extern int			g_iOctreeZ;
extern int			g_iVertexX;
extern int			g_iVertexZ;
extern int			g_iLightSource;
extern GLuint		g_iSkyBoxFaceCulling;
extern int			g_iLocalPlayer;
extern int			g_iMapIndex;
extern CBot*		g_pActiveBot;
extern char*		g_sStatusMessage;
// Global variables - End
