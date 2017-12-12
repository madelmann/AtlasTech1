#ifndef _ENGINE_H
#define _ENGINE_H


#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

#include "GLWrapper.h"
#include "Tools.h"
#include "Camera.h"																// Include our camera class and functions
#include "Console.h"
#include "Object.h"
#include "BasicInterpreter.h"
#include "Bullet.h"
#include "Chat.h"
#include "Editor.h"
#include "Entity.h"
#include "FileManager.h"
#include "Flora.h"
#include "Font.h"																// OpenGL font class
#include "Frustum.h"
#include "Light.h"
#include "Map.h"
#include "Material.h"
#include "Mouse.h"
#include "Normals.h"
#include "3DSObjectManager.h"
#include "ParticleSystem.h"
#include "PhysicManager.h"
#include "Player.h"
#include "QuadTree.h"
#include "Bot.h"
#include "ObjectManager.h"
#include "Regions.h"
#include "Scene.h"
#include "Script.h"
#include "Shader.h"
#include "Sound.h"
#include "StaticObject.h"
#include "Statistics.h"
#include "TGA.h"
#include "Texture.h"
#include "Water.h"																// Include our water functions
#include "UserInterface.h"


struct TServerInstance
{
public:
	HANDLE	hHandle;
	int		ip[4];																// used to store IP address of server
	int		iGameType;
	int		iServerPort;
	char	sMapName[80];
	char	sServerIP[16];
};


#define MAX_SERVER_INSTANZES	4
#define MAX_CLIENTS				32
#define SERVER_PORT				27015


#define KEYBINDING_VERSION1			1
#define KEYBINDING_VERSION_ACTUAL	KEYBINDING_VERSION1


// These are for our multi-texture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4

// These are used to increase the detail texture for our terrain
#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// We need to define this for glTexParameteri()
#define GL_CLAMP_TO_EDGE					0x812F								// This is for our skybox textures


// Here are the multitexture function prototypes
typedef void (APIENTRY *PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY *PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
// Here we extern our function pointer for volumetric fog positioning
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);


// Here we extern our function pointers for OpenGL
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
extern PFNWGLEXTSWAPCONTROLPROC				glSwapIntervalEXT;
extern PFNWGLEXTGETSWAPINTERVALPROC			glGetSwapIntervalEXT;

// Here we extern our function pointer for volumetric fog positioning
extern PFNGLFOGCOORDFEXTPROC				glFogCoordfEXT;

// This declares our point sprite extension
extern PFNGLPOINTPARAMETERFARBPROC			glPointParameterfARB;
extern PFNGLPOINTPARAMETERFVARBPROC			glPointParameterfvARB;

// GL_ARB_occlusion_query - Begin
extern PFNGLGENQUERIESARBPROC				glGenQueriesARB;
extern PFNGLDELETEQUERIESARBPROC			glDeleteQueriesARB;
extern PFNGLISQUERYARBPROC					glIsQueryARB;
extern PFNGLBEGINQUERYARBPROC				glBeginQueryARB;
extern PFNGLENDQUERYARBPROC					glEndQueryARB;
extern PFNGLGETQUERYIVARBPROC				glGetQueryivARB;
extern PFNGLGETQUERYOBJECTIVARBPROC			glGetQueryObjectivARB;
extern PFNGLGETQUERYOBJECTUIVARBPROC		glGetQueryObjectuivARB;
// GL_ARB_occlusion_query - End


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



namespace EngineSpace {

struct TKeyBinding
{
	TKeyBinding()
	{
		bPressed = false;
		uKey = 0;
	}
	~TKeyBinding()
	{
		bPressed = false;
		uKey = 0;
	}

	bool	bPressed;
	int		uKey;
};

class CKeyBindings
{
public:
	CKeyBindings();
	bool Load(char Filename[255]);
	bool Save(char Filename[255]);

	int			iVersion;
	TKeyBinding kbALT;
	TKeyBinding	kbCrouch;
	TKeyBinding	kbBackward;
	TKeyBinding	kbForward;
	TKeyBinding	kbJump;
	TKeyBinding	kbRotateLeft;
	TKeyBinding	kbRotateRight;
	TKeyBinding	kbStrafeLeft;
	TKeyBinding	kbStrafeRight;
	TKeyBinding	kbRun;
	TKeyBinding kbUse;
};


class CEngine: public CSingleton< CEngine >
{
	friend class CSingleton< CEngine >;

public:
	CEngine();
	~CEngine();

	int CheckOpenGLExtension(char* extensionName);
	void ConfigRead(char *Filename);
	void Init();
	void Manage();
	void Render();
	void Reset();
	void SetLightModel(bool bLightModel);

	CScreen			Screen;
	bool			bCellShading;
	bool			bClipping;
	bool			bColorMaterial;
	bool			bCompressedTextures;
	bool			bDrawFloor;
	bool			bDrawRoof;
	bool			bFrustumCulling;
	bool			bFullScreen;
	bool			bLightModelTwoSided;
	bool			bMultiTexturing;
	bool			bPointAndLineAntialiasing;
	bool			bRenderWayPoints;
	bool			bShowStats;
	bool			bSphereBoundings;
	bool			bVSync;
	float			fAnsitropicFilter;
	float			fFOV;
	float			fMAXAnsitropicFilter;
	float			fMotionBlur;
	float			fvLODDistance[3];
	GLuint			gluiMinVisibleSamples;				// Minimum visible samples (~pixels) to render if occluded
	int				iAntialias;
	int				iFPS;
	int				iFCFramesAgo;						// Frustrum checked n frames ago
	int				iMajorVersion;
	int				iMaxLights;
	int				iMaxLODLevel;
	int				iMinorVersion;
	int				iMultiSampling;
	int				iShadows;
	unsigned int	uiMaxOcclusionCalculatedBeforeFPS;
	CKeyBindings	Keybinding;

public:
	char* EngineNameString;
	char* EngineVersionString;
};

}


// Global classes
extern EngineSpace::CEngine	*Engine;
extern CBasicInterpreter	g_BasicInterpreter;
extern CBotManager			g_Manager_Bot;
extern CBulletManager		g_Manager_Bullet;
extern CFPSCamera			g_Camera;
extern CChatManager			g_Manager_Chat;
extern CConsole				*Console;
extern EditorSpace::Editor	*Editor;
extern CEntityManager		g_Manager_Entity;
extern CFileManager			g_Manager_File;
extern CFloraManager		g_Manager_Flora;
extern CGLFont				Font;												// Font object used to display text to the screen
extern CFrustum				Frustum;
extern CLight				Light[8];
extern CMap					Map;
extern CMaterialManager		g_Manager_Material;
extern MouseSpace::CMouse	Mouse;
extern C3DSObjectManager	g_Manager_3DSObject;
extern CObjectManager		g_Manager_Object;
extern CParticleManager		g_Manager_ParticleSystem;
extern CPhysicManager		g_Manager_Physic;
extern CPlayerManager		g_Manager_Player;
extern CQuadTree			QuadTree;
extern CRegionManager		g_Manager_Region;
extern CScene				Scene;
extern CScriptManager		g_Manager_Script;
extern CShaderManager		*Shader;
extern CSoundManager		g_Manager_Sound;
extern CStaticObjectManager	g_Manager_StaticObject;
extern CStatisticsManager	g_Manager_Statistic;
extern CTextureManager		*g_Manager_Texture;
extern CWater				Water;
//extern UI::CUIManager		GUI;


// Global variables - Begin
// Engine
extern int			g_FrameCount;
extern float		g_fCameraAngleX;
extern float		g_fCameraAngleY;
extern float		g_fCameraAngleZ;
extern float		g_fCameraOverFloor;
extern float		g_fFrameInterval;
extern float		g_fMouseSpeedDown;
extern float		g_iHeightMapScaleX;
extern float		g_iHeightMapScaleY;
extern float		g_iHeightMapScaleZ;
extern int			g_iViewWidth;

extern float		g_fElpasedTime;
extern double		g_dCurTime;
extern double		g_dFrequency;
extern double		g_dLastTime;
// Editor
//extern int			g_iEditMode;
extern int			g_iOctreeX;
extern int			g_iOctreeZ;
extern int			g_iVertexX;
extern int			g_iVertexZ;
// Lighting
extern int			g_iLightSource;
// Various
extern GLuint		g_iSkyBoxFaceCulling;
extern int			g_iLocalPlayer;
extern int			g_iMapIndex;
extern int			MeshID_SkyBox;
extern CBot*		g_pActiveBot;
extern char*		g_sStatusMessage;

extern float		g_halfWaterSize;
extern const float	kCausticScale;
extern float		g_WaterUV;

extern bool			g_bInsertInWorld;
extern int			InsertIndex;
// Global variables - End



void BillBoard_End();
void BillBoard_Cylindrical_Begin(float *cam, float *worldPos);
void BillBoard_Cylindrical_Cheat_Begin();
void BillBoard_Spherical_Begin(float *cam, float *worldPos);
void BillBoard_Spherical_Cheat_Begin();
void CalculateFrameRate();
//TAdditionalCheck CanDrawObject(TObjectData *od, CGraphicObject go);
int CheckOpenGLExtension(char* extensionName);
float DistanceToCamera(CGraphicObject* obj);
void Editor_ModeSwitch(EditorSpace::Mode_t mode);
POINT GetClientMousePosition();
CVector3 GetClientMousePosition3D();
int GetLODLevel(CGraphicObject* obj);
void glErrorCheck();
CVector3 glPosition3D(int x, int y);
void glPrintText(float x, float y, char* text, ...);
void glPrintText(float x, float y, string* text, ...);
void glShadowProjection(float * l, float * e, float * n);
void makeBGR(unsigned char *p, int size);
bool makeScreenShot(char *fName, int winW, int winH);
inline int mathsNormalize(float *v);
inline float mathsSqr(float a);
void PreProcessSkyBox(float x, float y, float z, float width, float height, float length);
void RenderBullets();
void RenderGrass();
void RenderImage2D(char FName[255], float left, float top, float width, float height);
void RenderImageAtFullScreen(char FName[255]);
void RenderInterface();
void RenderMouse(char MouseEvent[20]);
void RenderPlayers();
void RenderScene();																// This draws everything to the screen
void RenderSkyBox();
void RenderSkyPlane();
void RenderStatusMessage(char* MessageText);
void RenderTest1();
void RenderTest2(Texture &texture);
void RenderTest3();
void RenderToTexture(Texture &texture);
void RenderWayPoints(CBot *bot);
void RenderWeapon();
void RenderWorld();																// This renders all of the world or just the top of it
double roundd(double r);
float roundf(float r);
void SetCameraPosition();
void writeTGA(char *name, unsigned char *buff, int w, int h);

#endif