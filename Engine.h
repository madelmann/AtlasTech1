
#pragma once

// Library includes
#include "WinsockWrapper.h"
#include <Windows.h>
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "GLWrapper.h"

// Project includes
#include "EngineUtils.h"

class CSceneObject;

struct TServerInstance
{
public:
	void*	hHandle;
	int		ip[4];																// used to store IP address of server
	int		iGameType;
	int		iServerPort;
	char	sMapName[80];
	char	sServerIP[16];
};


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


void BillBoard_End();
void BillBoard_Cylindrical_Begin(float *cam, float *worldPos);
void BillBoard_Cylindrical_Cheat_Begin();
void BillBoard_Spherical_Begin(float *cam, float *worldPos);
void BillBoard_Spherical_Cheat_Begin();
void CalculateFrameRate();
//TAdditionalCheck CanDrawObject(TObjectData *od, CGraphicObject go);
//int CheckOpenGLExtension(char* extensionName);
float DistanceToCamera(CGraphicObject* obj);
void Editor_ModeSwitch(EditorSpace::Mode_t mode);
POINT GetClientMousePosition();
CVector3 GetClientMousePosition3D();
int GetLODLevel(CGraphicObject* obj);
void glErrorCheck();
CVector3 glPosition3D(int x, int y);
void glPrintText(float x, float y, char* text, ...);
void glPrintText(float x, float y, std::string* text, ...);
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
void SetCameraPosition();
void writeTGA(char *name, unsigned char *buff, int w, int h);


CGraphicObject* Entity_add(char* filename);
CSceneObject* GetObjectByID(int id);
//int Material_Add(char* Filename);
//int Material_Apply(int index, bool bForceMaterialUse);
GLuint Texture_Add(char* Filename);
GLuint Texture_IndexOf(char* Filename);
void Texture_SetActive(char* Filename);
//void Texture_SetActiveID(int id);
int Object3DS_Add(char* Filename);
int Object3DS_IndexOf(char* Filename);
CPlayer* Player_Add(char* Filename);
