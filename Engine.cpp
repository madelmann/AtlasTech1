// This are compiler directives that include libraries (For Visual Studio).
#pragma comment(lib, "alut.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "RakNetLibStatic.lib")
#pragma comment(lib, "ws2_32.lib")



#include "Main.h"
#include "Engine.h"



/* Computes the inner product between vectors v and q */ 

#define mathsInnerProduct(v,q) \
	((v)[0] * (q)[0] + \
	(v)[1] * (q)[1] + \
	(v)[2] * (q)[2])	


/* a = b x c */

#define mathsCrossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];


/* vector a = b - c, where b and c represent points*/

#define mathsVector(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];


// Our function pointers for the ARB multitexturing functions
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= NULL;
PFNWGLEXTSWAPCONTROLPROC		glSwapIntervalEXT		= NULL;
PFNWGLEXTGETSWAPINTERVALPROC	glGetSwapIntervalEXT	= NULL;

// This is our fog extension function pointer to set a vertice's depth
PFNGLFOGCOORDFEXTPROC			glFogCoordfEXT			= NULL;

// This defines our point sprite extension
PFNGLPOINTPARAMETERFARBPROC		glPointParameterfARB	= NULL;
PFNGLPOINTPARAMETERFVARBPROC	glPointParameterfvARB	= NULL;

// GL_ARB_occlusion_query - Begin
PFNGLGENQUERIESARBPROC			glGenQueriesARB			= NULL;
PFNGLDELETEQUERIESARBPROC		glDeleteQueriesARB		= NULL;
PFNGLISQUERYARBPROC				glIsQueryARB			= NULL;
PFNGLBEGINQUERYARBPROC			glBeginQueryARB			= NULL;
PFNGLENDQUERYARBPROC			glEndQueryARB			= NULL;
PFNGLGETQUERYIVARBPROC			glGetQueryivARB			= NULL;
PFNGLGETQUERYOBJECTIVARBPROC	glGetQueryObjectivARB	= NULL;
PFNGLGETQUERYOBJECTUIVARBPROC	glGetQueryObjectuivARB	= NULL;
// GL_ARB_occlusion_query - End


EngineSpace::CEngine	*Engine							= NULL;
CBasicInterpreter		g_BasicInterpreter;
CBotManager				g_Manager_Bot;
CBulletManager			g_Manager_Bullet;
CFPSCamera				g_Camera;												// This is our global camera object
CChatManager			g_Manager_Chat(8);
CConsole				*Console						= NULL;
EditorSpace::Editor		*Editor							= NULL;
CEntityManager			g_Manager_Entity;
CFileManager			g_Manager_File;
CFloraManager			g_Manager_Flora;
CGLFont					Font;													// Font object used to display text to the screen
CFrustum				Frustum;
CLight					Light[8];
CMap					Map;
CMaterialManager		g_Manager_Material;
MouseSpace::CMouse		Mouse;
C3DSObjectManager		g_Manager_3DSObject;
CObjectManager			g_Manager_Object;
CParticleManager		g_Manager_ParticleSystem;
CPhysicManager			g_Manager_Physic;
CPlayerManager			g_Manager_Player;
CQuadTree				QuadTree;
CRegionManager			g_Manager_Region;
CScene					Scene;
CScriptManager			g_Manager_Script;
CShaderManager			*Shader							= NULL;
CSoundManager			g_Manager_Sound;
CStaticObjectManager	g_Manager_StaticObject;
CStatisticsManager		g_Manager_Statistic;
CTextureManager			*g_Manager_Texture				= NULL;
CWater					Water;
//UI::CUIManager			GUI;




// Global variables - Begin
// Engine
int					g_FrameCount			= 0;
float				g_fCameraAngleX			= 0.0f;
float				g_fCameraAngleY			= 0.0f;
float				g_fCameraAngleZ			= 0.0f;
float				g_fCameraOverFloor		= CAMERAOVERFLOOR_DEFAULT;
float				g_fFrameInterval		= 0.0f;
float				g_fMouseSpeedDown		= 495.0f;
int					g_iViewWidth			= 1;
float				g_iHeightMapScaleX		= 16;
float				g_iHeightMapScaleY		= 2;
float				g_iHeightMapScaleZ		= 16;

float				g_fElpasedTime;
double				g_dCurTime;
double				g_dFrequency;
double				g_dLastTime;
// Editor
int					g_iEditMode				= EditorSpace::Selection;
int					g_iOctreeX				= 0;
int					g_iOctreeZ				= 0;
int					g_iVertexX				= 0;
int					g_iVertexZ				= 0;
// Lighting
int					g_iLightSource			= 1;
// Various
bool				bBillBoardCheat			= false;
GLuint				g_iSkyBoxFaceCulling	= GL_BACK;
//bool				g_bDetail				= true;										// This controls if we have detail texturing on or off
//int				g_DetailScale			= 8;										// This handles the current scale for the texture matrix for the detail texture
int					g_iMapIndex				= 0;
int					g_iLocalPlayer			= -1;
int					g_TextureSize			= 512;										// The size of the textures that we will render on the fly (reflection/refraction/depth)
float				g_halfWaterSize			= 2048;
float				g_WaterUV				= 10.0f;									// The scale for the water textures
const float			kCausticScale			= 4.0f;										// The scale for the caustics (light textures underwater)
int					MeshID_SkyBox			= 0;
CBot				*g_pActiveBot			= NULL;
char*				g_sStatusMessage		= "";

bool				g_bInsertInWorld		= false;
int					InsertIndex				= -1;
// Global variables - End


// Local variables - Begin
CRGBA light0_ambient(0.2f, 0.2f, 0.2f, 1.0f);
CRGBA light0_diffuse(0.4f, 0.4f, 0.4f, 1.0f);
CRGBA light0_specular(1.0f, 1.0f, 1.0f, 0.0f);
CVector3 light0_position(-4096.0f, 1024.0f, 0.0f);

CRGBA light1_ambient(0.2f, 0.2f, 0.2f, 1.0f);
CRGBA light1_diffuse(0.4f, 0.4f, 0.4f, 1.0f);
CRGBA light1_specular( 1.0f, 1.0f, 1.0f, 0.0f);
CVector3 light1_position(0.0f, 200.0f, 0.0f);
// Local variables - End


float e[] = { 0.0f,		0.8f,		0.0f };		// Point of the plane
float l[] = { 0.0f,		0.0f,		0.0f };		// Coordinates of the light source
float n[] = { 0.0f,		-1.0f,		0.0f };		// Normal vector for the plane



namespace EngineSpace {

CKeyBindings::CKeyBindings()
{
	iVersion			=	KEYBINDING_VERSION_ACTUAL;
	kbALT.uKey			=	VK_LSHIFT;
	kbCrouch.uKey		=	'Q';
	kbBackward.uKey		=	'S';
	kbForward.uKey		=	'W';
	kbJump.uKey			=	'F';
	kbRotateLeft.uKey	=	VK_LEFT;
	kbRotateRight.uKey	=	VK_RIGHT;
	kbStrafeLeft.uKey	=	'A';
	kbStrafeRight.uKey	=	'D';
	kbRun.uKey			=	VK_SHIFT;
	kbUse.uKey			=	'E';
}

bool CKeyBindings::Load(char Filename[255])
{
	if(strcmpi(Filename, "") == 0)
		return false;

	char FName[255] = "";
	sprintf(FName, "%s%s", g_sGameName, Filename);

	FILE *pFile = NULL;
	fopen_s(&pFile, FName, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!pFile) 
	{
		// Display an error message and don't load anything if no file was found
		char buffer[255] = "";
		sprintf(buffer, "ERROR: Unable to find keybindings file '%s'!\n", FName);
		Console->Output(buffer);
		return false;
	}

	fread(&iVersion, sizeof(int), 1, pFile);

	switch(iVersion)
	{
	case KEYBINDING_VERSION1:
		fread(&kbCrouch.uKey, sizeof(int), 1, pFile);
		fread(&kbBackward.uKey, sizeof(int), 1, pFile);
		fread(&kbForward.uKey, sizeof(int), 1, pFile);
		fread(&kbJump.uKey, sizeof(int), 1, pFile);
		fread(&kbRotateLeft.uKey, sizeof(int), 1, pFile);
		fread(&kbRotateRight.uKey, sizeof(int), 1, pFile);
		fread(&kbStrafeLeft.uKey, sizeof(int), 1, pFile);
		fread(&kbStrafeRight.uKey, sizeof(int), 1, pFile);
		fread(&kbRun.uKey, sizeof(int), 1, pFile);
		break;
	}

	fclose(pFile);

	return true;
}

bool CKeyBindings::Save(char Filename[255])
{
	if(strcmpi(Filename, "") == 0)
		return false;

	char FName[255] = "";
	sprintf(FName, "%s%s", g_sGameName, Filename);

	FILE *pFile = NULL;
	fopen_s(&pFile, FName, "wb");

	// Make sure we have a valid file pointer (we found the file)
	if(!pFile) 
	{
		// Display a warning
		char buffer[255] = "";
		sprintf(buffer, "WARNING: Unable to find keybindings file '%s'!", FName);
		Console->Output(buffer);
	}

	iVersion			=	KEYBINDING_VERSION_ACTUAL;
	kbCrouch.uKey		=	'Q';
	kbBackward.uKey		=	'S';
	kbForward.uKey		=	'W';
	kbJump.uKey			=	'F';
	kbRotateLeft.uKey	=	VK_LEFT;
	kbRotateRight.uKey	=	VK_RIGHT;
	kbStrafeLeft.uKey	=	'A';
	kbStrafeRight.uKey	=	'D';
	kbRun.uKey			=	VK_SHIFT;

	fwrite(&iVersion, sizeof(int), 1, pFile);
	fwrite(&kbCrouch.uKey, sizeof(int), 1, pFile);
	fwrite(&kbBackward.uKey, sizeof(int), 1, pFile);
	fwrite(&kbForward.uKey, sizeof(int), 1, pFile);
	fwrite(&kbJump.uKey, sizeof(int), 1, pFile);
	fwrite(&kbRotateLeft.uKey, sizeof(int), 1, pFile);
	fwrite(&kbRotateRight.uKey, sizeof(int), 1, pFile);
	fwrite(&kbStrafeLeft.uKey, sizeof(int), 1, pFile);
	fwrite(&kbStrafeRight.uKey, sizeof(int), 1, pFile);
	fwrite(&kbRun.uKey, sizeof(int), 1, pFile);

	fclose(pFile);

	return true;
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////CEngine - BEGIN//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

namespace EngineSpace {

CEngine::CEngine()
{
	EngineNameString = "Atlas Client";
	EngineVersionString = "v0.1";

	Screen.Depth = 32;
	Screen.Height = 768;
	Screen.Middle.x = 512;
	Screen.Middle.y = 384;
	Screen.Width = 1024;

	bCellShading = false;
	bClipping = true;
	bColorMaterial = false;
	bCompressedTextures = false;
	bDrawFloor = false;
	bDrawRoof = false;
	bFrustumCulling = true;
	bFullScreen = false;
	bLightModelTwoSided = false;
	bMultiTexturing = true;
	bPointAndLineAntialiasing = false;
	bRenderWayPoints = false;
	bShowStats = false;
	bSphereBoundings = false;
	bVSync = true;
	for(unsigned int i = 0; i < 8; i += 1)
	{
		Light[i].clear();
		Light[i].setLight();
	}
	fAnsitropicFilter = 1.0f;
	fFOV = 50.0f;
	fMAXAnsitropicFilter = 1.0f;
	fMotionBlur = 0.0f;
	fvLODDistance[0] = 0.0f;
	fvLODDistance[1] = 512.0f;
	fvLODDistance[2] = 1024.0f;
	gluiMinVisibleSamples = 16;
	iAntialias = 0;
	iFPS = 0;
	iFCFramesAgo = 2;
	iMaxLights = 8;
	iMaxLODLevel = 3;
	iMultiSampling = 0;
	iShadows = 0;
	uiMaxOcclusionCalculatedBeforeFPS = 3;
}

CEngine::~CEngine()
{
	SAFE_DELETE ( Editor );
	SAFE_DELETE ( Shader );
	SAFE_DELETE ( g_Manager_Texture );
}

int CEngine::CheckOpenGLExtension(char* extensionName)
{
	// get the list of supported extensions
	char* extensionList = (char*)glGetString(GL_EXTENSIONS);

	if (!extensionName || !extensionList)
	{
		return 0;
	}

	while(extensionList)
	{
		// find the length of the first extension substring
		unsigned int firstExtensionLength = strcspn(extensionList, " ");

		if (strlen(extensionName) == firstExtensionLength &&
			strncmp(extensionName, extensionList, firstExtensionLength) == 0)
		{
			return 1;
		}	

		// move to the next substring
		extensionList += firstExtensionLength + 1;
	}

	SAFE_DELETE ( extensionName );
    return 0;
}

void CEngine::ConfigRead(char *Filename)
{
	Console->Clear();
	Console->Logging_Start();

	char buffer[255] = "";
	sprintf(buffer, "%s", Filename);

	FILE *pFile;

	fopen_s(&pFile, buffer, "rt");
	if(!pFile) 
	{
		// Display an error message and don't load anything if no file was found
		char buffer[255] = "";
		sprintf(buffer, "ERROR[ConfigRead]: '%s' not found!\n", buffer);
		Console->Output(buffer);
		return;
	}

	char oneline[255] = "";

	do
	{
		sprintf(oneline, "");
		readstr(pFile, oneline);

		ProcessCommand(oneline);
	} while(strcmp(oneline, "") != 0);

	fclose(pFile);
}

void CEngine::Init()
{
	g_Manager_Physic.Reset();
	g_Manager_Physic.SetGravity(CVector3(0.0f, -9.81f, 0.0f));
	Shader = new CShaderManager;
	g_Manager_Texture = new CTextureManager();
	g_Manager_Texture->Reset();
	GUI.setscreen(&Screen);

	Editor = new EditorSpace::Editor(g_Manager_Texture);

	// Get maximum opengl lights
	glGetIntegerv(GL_MAX_LIGHTS, &iMaxLights);

	// Here we initialize our multitexturing functions
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");

    // Make sure our multi-texturing extensions were loaded correctly
    if(!glActiveTextureARB || !glMultiTexCoord2fARB)
    {
	    // Print an error message and quit.
	    MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
	    PostQuitMessage(0);
    }

	// Find the correct function pointer that houses the fog coordinate function
	glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)wglGetProcAddress("glFogCoordfEXT");
	if(!glFogCoordfEXT)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support volumetric fog", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Check for anisotropic filter extension
	if(CheckOpenGLExtension("GL_EXT_texture_filter_anisotropic"))
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMAXAnsitropicFilter);
	}

	// Check for VSync extension
	if(CheckOpenGLExtension("WGL_EXT_swap_control"))
	{
		//get address's of both functions and save them
		glSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)
		wglGetProcAddress("wglSwapIntervalEXT");
		glGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)
		wglGetProcAddress("wglGetSwapIntervalEXT");
	}

	// Check for occlusion query extension
	if(CheckOpenGLExtension("GL_ARB_occlusion_query"))
	{
		glGenQueriesARB        = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
        glDeleteQueriesARB     = (PFNGLDELETEQUERIESARBPROC)wglGetProcAddress("glDeleteQueriesARB");
        glIsQueryARB           = (PFNGLISQUERYARBPROC)wglGetProcAddress("glIsQueryARB");
        glBeginQueryARB        = (PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
        glEndQueryARB          = (PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
        glGetQueryivARB        = (PFNGLGETQUERYIVARBPROC)wglGetProcAddress("glGetQueryivARB");
        glGetQueryObjectivARB  = (PFNGLGETQUERYOBJECTIVARBPROC)wglGetProcAddress("glGetQueryObjectivARB");
        glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)wglGetProcAddress("glGetQueryObjectuivARB");

        if( !glGenQueriesARB  || !glDeleteQueriesARB || !glIsQueryARB || 
            !glBeginQueryARB || !glEndQueryARB || !glGetQueryivARB ||
            !glGetQueryObjectivARB || !glGetQueryObjectuivARB )
        {
            MessageBox(NULL,"One or more GL_ARB_occlusion_query functions were not found",
                "ERROR", MB_OK|MB_ICONEXCLAMATION);
            return;
        }
	}


	glEnable(GL_DEPTH_TEST);													// Enable depth testing
	glEnable(GL_TEXTURE_2D);

	float fogColor[4] = {0.95f, 0.93f, 0.76f, 1.0f};							// Pick a tan color for our fog with a full alpha

	// Now that we have our 4 colors, let's set the fog mode.  I chose GL_EXP2, but the default
	// is GL_EXP.  There is also a GL_LINEAR.  Try them all to get a feel for the type of fog
	// you are looking for.  They are computed each with different equations.  I think the
	// exponential ones look the best. (f = e^(- density * z)) (Z is the eye or camera position)
	glFogi(GL_FOG_MODE, GL_EXP2);												// Fog Mode
	glFogf(GL_FOG_DENSITY, 0.25f);												// How Dense Will The Fog Be
	glFogf(GL_FOG_START, 2000.0f);												// Fog Start Depth
	glFogf(GL_FOG_END, 8192.0f);												// Fog End Depth
	glFogfv(GL_FOG_COLOR, fogColor);											// Set Fog Color

	glHint(GL_FOG_HINT, GL_NICEST);												// Fog Hint Value
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);							// Really nice perspective calculations
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);						// Really fast perspective calculations

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light0_ambient);
	//glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);			// sets lighting to one-sided

	glShadeModel(GL_SMOOTH);

	//Light[LIGHT_SUN].setAmbiant(light0_ambient);
	//Light[LIGHT_SUN].setDiffuse(light0_diffuse);
	//Light[LIGHT_SUN].setSpecular(light0_specular);
	//Light[LIGHT_SUN].setPosition(light0_position);
	//Light[LIGHT_SUN].setAttenuationConstant(0.5f);
	//Light[LIGHT_SUN].setAttenuationLinear(0.0005f);
	//Light[LIGHT_SUN].setAttenuationQuadratic(0.f);
	//Light[LIGHT_SUN].setIndex(LIGHT_SUN);
	//Light[LIGHT_SUN].enable();

	Light[LIGHT_SUN].setupDirectional(light0_ambient, light0_diffuse, light0_specular, light0_position, 1.f, 0.0005f, 0.f);
	Light[LIGHT_SUN].setIndex(LIGHT_SUN);
	Light[LIGHT_SUN].enable();

	CVector3 spotdir(0.0f, 0.0f, -1.0f);
	CVector3 spotpos(0.0f, 0.0f, 0.0f);

	Light[LIGHT_FLASHLIGHT].setAmbiant(light0_ambient);
	Light[LIGHT_FLASHLIGHT].setDiffuse(light0_diffuse);
	Light[LIGHT_FLASHLIGHT].setPosition(spotpos);
	Light[LIGHT_FLASHLIGHT].setSpecular(light0_specular);
	Light[LIGHT_FLASHLIGHT].setCutoff(10.0f);
	Light[LIGHT_FLASHLIGHT].setDirection(spotdir);
	Light[LIGHT_FLASHLIGHT].setExponent(10.0f);
	//Light[LIGHT_FLASHLIGHT].SetAttenuationConstant(1.0f);
	//Light[LIGHT_FLASHLIGHT].SetAttenuationLinear(0.001f);
	//Light[LIGHT_FLASHLIGHT].SetAttenuationQuadratic(0.004f);
	Light[LIGHT_FLASHLIGHT].setIndex(LIGHT_FLASHLIGHT);
	Light[LIGHT_FLASHLIGHT].enable();


	// Since our water is reflecting the world that our camera is looking at,
	// we need to have 3 custom textures that we will render to.  These textures
	// will then be used in the shaders to do the reflection and refraction.
	// The depth texture will just store a single value for the depth of
	// each pixel.  This is why we use GL_DEPTH_COMPONENT and require 1 channel.
	g_Manager_Texture->CreateRenderTexture(g_Texture, g_TextureSize, g_TextureSize, 3, GL_RGB, REFLECTION_ID);
	g_Manager_Texture->CreateRenderTexture(g_Texture, g_TextureSize, g_TextureSize, 3, GL_RGB, REFRACTION_ID);
	g_Manager_Texture->CreateRenderTexture(g_Texture, g_TextureSize, g_TextureSize, 1, GL_DEPTH_COMPONENT, DEPTH_ID);
	// Ingame render target
	g_Manager_Texture->CreateRenderTexture(g_Texture, Screen.Width, Screen.Height, 3, GL_RGB, INGAME_ID);


	char buffer[255] = "";

	sprintf(buffer, "%sTextures\\normalmap.bmp", g_sGameName);
	CreateTexture(g_Texture[NORMAL_ID],	 buffer);								// Load the normal map water texture
	sprintf(buffer, "%sTextures\\dudvmap.bmp", g_sGameName);
	CreateTexture(g_Texture[DUDVMAP_ID], buffer);								// Load the dudv map water texture

	g_Manager_Texture->CreateTexture("ConsoleBackground.bmp");					// Load the console background image
	g_Manager_Texture->CreateTexture("Screen_Loading.bmp");						// Load the Loading Screen image
	g_Manager_Texture->CreateTexture("Screen_Menu.bmp");						// Load the main menu background image
	g_Manager_Texture->CreateTexture("Screen_Saving.bmp");						// Load the Saving Screen image
	g_Manager_Texture->CreateTexture("Crosshair0.bmp");
	g_Manager_Texture->CreateTexture("Mouse.tga");
	//g_Manager_Texture->CreateBMPTexture1D("CellShading1.bmp");
	//g_Manager_Texture->CreateBMPTexture1D("CellShading2.bmp");
	//g_Manager_Texture->CreateBMPTexture1D("CellShading3.bmp");
	//g_Manager_Texture->CreateBMPTexture1D("CellShading4.bmp");
	//g_Manager_Texture->CreateAnimBMPTextures("Caustics\\caust", NUM_WATER_TEX);
	//g_Manager_Texture->CreateTexture("PostEffect_Film.tga");
	g_Manager_Texture->CreateTexture("Grass.tga");
	//g_Manager_Texture->CreateTexture("color_map.jpg");
	g_Manager_Texture->CreateTexture("normal_map.jpg");
	//g_Manager_Texture->CreateTexture("bumpmap2_test1_color.jpg");
	//g_Manager_Texture->CreateTexture("bumpmap2_test1_normal.jpg");
	//g_Manager_Texture->CreateTexture("Sky.bmp");
	//g_Manager_Texture->CreateTexture("Arial.jpg");
	//g_Manager_Texture->CreateTexture("AlphaBlack.bmp");
	//g_Manager_Texture->CreateTexture("AlphaWhite.bmp");

	InitUserInterface();

	g_Manager_Texture->CreateTexture("Dummy.bmp");								// !This has to be the last texture loaded before the custom textures!


	Shader->Add("BumpMap");
	Shader->Add("bumpy");
	Shader->Add("Cell Shading");
	Shader->Add("Cell Shading 2");
	Shader->Add("Debug Normals");
	Shader->Add("Per Pixel Lighting");
	Shader->Add("Per Pixel Lighting2");
	Shader->Add("Phong Light");
	Shader->Add("Phong Light+");
	Shader->Add("Simple Toon");
	Shader->Add("Simple Toon 2");
	Shader->Add("Simple Toon 3");
	Shader->Add("Water");

	// Initialize the font object so we can display output using it.
	if(!Font.Initialize(g_hDC, 25, "Arial"))
		return;

	Font.selectFont(g_Manager_Texture->IndexOf("Arial.jpg"));

	g_Manager_ParticleSystem.Init();
}

void CEngine::Manage()
{
	if(g_bLoading)
		return;

	g_Manager_Bot.Manage();

	g_Manager_Bullet.Manage();

	g_Manager_Chat.Manage();

	g_Manager_Physic.Manage();

	if(Engine->bClipping)
		g_Manager_Region.Check(Game.mPlayer->mPosition);
	
	Scene.Manage();

	g_Manager_Sound.Manage();

	if(bShowStats)
		g_Manager_Statistic.Manage();

	if(Map.fWaveHeight != 0.0f)
		Map.ProcessWaves();
}

void CEngine::Render()
{
	g_FrameCount += 1;															// Increase our frame counter

	if(Engine->bPointAndLineAntialiasing)
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
	}

	RenderScene();																// Render the scene every frame

	if(Engine->bPointAndLineAntialiasing)
	{
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
	}
}

void CEngine::Reset()
{
	bCellShading = false;
	bClipping = true;
	bDrawFloor = false;
	bDrawRoof = false;
	bFrustumCulling = true;
	bLightModelTwoSided = false;
	bRenderWayPoints = false;
	bShowStats = false;
	bSphereBoundings = false;
	for(unsigned int i = 0; i < 8; i += 1)
	{
		Light[i].clear();
		Light[i].setLight();
	}
	fFOV = 50.0f;
	fMotionBlur = 0.0f;
	iFPS = 0;
	iFCFramesAgo = 2;

	g_FrameCount = 0;

	Scene.Reset();
}

void CEngine::SetLightModel(bool bLightModel)
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, bLightModel);
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////CEngine - END////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


/*-----------------------------------------------------------------
The objects motion is restricted to a rotation on a predefined axis
The function bellow does cylindrical billboarding on the Y axis, i.e.
the object will be able to rotate on the Y axis only.
-----------------------------------------------------------------*/
void BillBoard_Cylindrical_Begin(float *cam, float *worldPos)
{
	float lookAt[3] = {0.0f, 0.0f, 1.0f}, objToCamProj[3], upAux[3], angleCosine;

	// objToCamProj is the vector in world coordinates from the local origin to the camera
	// projected in the XZ plane
	objToCamProj[0] = cam[0] - worldPos[0] ;
	objToCamProj[1] = 0.0f;
	objToCamProj[2] = cam[2] - worldPos[2] ;


	// normalize both vectors to get the cosine directly afterwards
	mathsNormalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	mathsCrossProduct(upAux,lookAt, objToCamProj);

	// compute the angle
	angleCosine = mathsInnerProduct(lookAt, objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and v vectors are too close together then |aux| could
	// be bigger than 1 due to lack of precision
	if ((angleCosine < 0.999999f) && (angleCosine > -0.999999f))
		glRotatef((float)(acos(angleCosine) * 180.0f / 3.14f), upAux[0], upAux[1], upAux[2]);

	bBillBoardCheat = false;
}

void BillBoard_Cylindrical_Cheat_Begin()
{
	float modelview[16];
	int i, j;

	// save the current modelview matrix
	glPushMatrix();

		// get the current modelview matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

		for(i = 0; i < 3; i += 2) 
			for(j = 0; j < 3; j += 1)
			{
				if (i == j)
					modelview[i * 4 + j] = 1.0f;
				else
					modelview[i * 4 + j] = 0.0f;
			}

		// set the modelview matrix
		glLoadMatrixf(modelview);

		bBillBoardCheat = true;
}

void BillBoard_End()
{
	// restore the previously 
	// stored modelview matrix
	if(bBillBoardCheat)
	{
		glPopMatrix();
		bBillBoardCheat = false;
	}
}

/*----------------------------------------------------------------
True billboarding. With the spherical version the object will 
always face the camera. It requires more computational effort than
the cylindrical billboard though. The parameters camX,camY, and camZ,
are the target, i.e. a 3D point to which the object will point.
----------------------------------------------------------------*/
void BillBoard_Spherical_Begin(float *cam, float *worldPos)
{
	float lookAt[3] = {0.0f, 0.0f, 1.0f}, objToCamProj[3], objToCam[3], upAux[3], angleCosine;

	// objToCamProj is the vector in world coordinates from the local origin to the camera
	// projected in the XZ plane
	objToCamProj[0] = cam[0] - worldPos[0] ;
	objToCamProj[1] = 0.0f;
	objToCamProj[2] = cam[2] - worldPos[2] ;

	// normalize both vectors to get the cosine directly afterwards
	mathsNormalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	mathsCrossProduct(upAux, lookAt, objToCamProj);

	// compute the angle
	angleCosine = mathsInnerProduct(lookAt, objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and v vectors are too close together then |aux| could
	// be bigger than 1 due to lack of precision
	if ((angleCosine < 0.999999) && (angleCosine > -0.999999))
		glRotatef((float)(acos(angleCosine) * 180 / 3.14), upAux[0], upAux[1], upAux[2]);	


	// The second part tilts the object so that it faces the camera

	// objToCam is the vector in world coordinates from the local origin to the camera
	objToCam[0] = cam[0] - worldPos[0] ;
	objToCam[1] = cam[1] - worldPos[1] ;
	objToCam[2] = cam[2] - worldPos[2] ;

	
	mathsNormalize(objToCam);

	// Compute the angle between v and v2, i.e. compute the
	// required angle for the lookup vector
	angleCosine = mathsInnerProduct(objToCamProj, objToCam);


	// Tilt the object. The test is done to prevent instability when objToCam and objToCamProj have a very small
	// angle between them
	if ((angleCosine < 0.999999f) && (angleCosine > -0.999999f))
		if (objToCam[1] < 0)
			glRotatef((float)(acos(angleCosine) * 180.0f / 3.14f), 1.0f, 0.0f, 0.0f);	
		else
			glRotatef((float)(acos(angleCosine) * 180.0f / 3.14f), -1.0f, 0.0f, 0.0f);	

	bBillBoardCheat = false;
}

void BillBoard_Spherical_Cheat_Begin()
{
	float modelview[16];
	int i, j;

	// save the current modelview matrix
	glPushMatrix();

		// get the current modelview matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

		// undo all rotations
		// beware all scaling is lost as well 
		for(i = 0; i < 3; i += 1) 
			for(j = 0; j < 3; j += 1)
			{
				if (i == j)
					modelview[i * 4 + j] = 1.0f;
				else
					modelview[i * 4 + j] = 0.0f;
			}

		// set the modelview with no rotations
		glLoadMatrixf(modelview);

		bBillBoardCheat = true;
}

///////////////////////////////// CALCULATE FRAME RATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function calculates the frame rate and time intervals between frames
/////
///////////////////////////////// CALCULATE FRAME RATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CalculateFrameRate()
{
	static float framesPerSecond    = 0.0f;		// This will store our fps
    static float lastTime			= 0.0f;		// This will hold the time from the last frame
	static char strFrameRate[100]	= {0};		// We will store the string here for the window title

	static float frameTime = 0.0f;				// This stores the last frame's time

	// Get the current time in seconds
	LONGLONG tmp;

	QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
	float currentTime = float(tmp / g_dFrequency);

	//float currentTime = timeGetTime() * 0.001f;

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
 	g_fFrameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// Increase the frame counter
    framesPerSecond += 1;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
    if(currentTime - lastTime > 1.0f)
    {
		// Here we set the lastTime to the currentTime
	    lastTime = currentTime;

		Engine->iFPS = (int)framesPerSecond;

		// Reset the frames per second
        framesPerSecond = 0.0f;
    }
}

//TAdditionalCheck CanDrawObject(TObjectData *od, CGraphicObject go)
//{
//	if(go.tAddidionalCheck.iCheckedBefore > Engine->iFCFramesAgo)
//	{
//		go.tAddidionalCheck.bResult = Frustum.sphereInFrustum(od->x + go.BoundingVolume.CX, od->y + go.BoundingVolume.CY, od->z + go.BoundingVolume.CZ, go.fRadius);
//		go.tAddidionalCheck.iCheckedBefore = 0;
//	}
//
//	go.tAddidionalCheck.iCheckedBefore += 1;
//
//	return go.tAddidionalCheck;
//}

float DistanceToCamera(CGraphicObject* obj)
{
	CVector3 camobj = g_Camera.Position();
	float dx = obj->mPosition.x - camobj.x;
	float dy = obj->mPosition.y - camobj.y;
	float dz = obj->mPosition.z - camobj.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

void Editor_ModeSwitch(EditorSpace::Mode_t mode)
{
	InsertIndex = 0;
	
	g_Manager_File.filelist.clear();

	char buffer[255] = "";

    switch(mode)
    {
	case EditorSpace::Selection:
		sprintf(g_sEditMode, "");
	    break;
    case EditorSpace::max3ds:
	    sprintf(g_sEditMode, "Mode-3DS");
	    sprintf(buffer, "%sObjects", g_sGameName);
	    g_Manager_File.Searchfiles(buffer, "3ds");
	    break;
    case EditorSpace::md2:
	    sprintf(g_sEditMode, "Mode-MD2");
	    sprintf(buffer, "%sModels", g_sGameName);
	    g_Manager_File.Searchfiles(buffer, "md2");
	    break;
	case EditorSpace::Static:
		sprintf(g_sEditMode, "Mode-STATIC");
	    sprintf(buffer, "%sStatic", g_sGameName);
	    g_Manager_File.Searchfiles(buffer, "static");
		break;
	case EditorSpace::Bot:
		sprintf(g_sEditMode, "Mode-BOT");
		sprintf(buffer, "%sModels", g_sGameName);
		g_Manager_File.Searchfiles(buffer, "md2");
		break;
	case EditorSpace::Particle:
		sprintf(g_sEditMode, "Mode-PARTICLE");
	    sprintf(buffer, "%sParticles", g_sGameName);
	    g_Manager_File.Searchfiles(buffer, "particle");
		break;
    case EditorSpace::Water:
	    sprintf(g_sEditMode, "Mode-WATER");
	    break;
    case EditorSpace::Fog:
	    sprintf(g_sEditMode, "Mode-FOG");
	    break;
    case EditorSpace::Light:
	    sprintf(g_sEditMode, "Mode-LIGHT");
	    break;
	case EditorSpace::Terrain:
	    sprintf(g_sEditMode, "Mode-TERRAIN");
	    sprintf(buffer, "%sTerrain", g_sGameName);
	    g_Manager_File.Searchfiles(buffer, "tga");
	    break;
    }

    if(g_Manager_File.filelist.size())
    {
	    basic_string<char> objectname = g_Manager_File.filelist.at(InsertIndex);

	    sprintf(buffer, "%s", objectname.data());
	    sprintf(g_sEditMode, "%s( %s )", g_sEditMode, buffer);
    }
}

POINT GetClientMousePosition()
{
	POINT mp;

	GetCursorPos(&mp);															// Gets The Current Cursor Coordinates (Mouse Coordinates)
    ScreenToClient(g_hWnd, &mp);

	Mouse.mp = mp;
	Mouse.gmp = mp;

	Mouse.gmp.y = Engine->Screen.Height - mp.y;

	return mp;
}

CVector3 GetClientMousePosition3D()
{
	POINT mp = GetClientMousePosition();										// Gets The Current Client Cursor Coordinates (Mouse Coordinates)

    return glPosition3D(mp.x, mp.y);
}

int GetLODLevel(CGraphicObject* obj)
{
	float d = DistanceToCamera(obj) - obj->fRadius;

	if(d > Engine->fvLODDistance[0])
	{
		if(d > Engine->fvLODDistance[1])
			return 2;
		else
			return 1;
	}
	else
		return 0;
}

void glErrorCheck()
{
	GLenum error;
	char buffer[512] = "";

	while((error = glGetError()) != GL_NO_ERROR)
	{
		sprintf(buffer, "[%s:%d] failed with error %s\n", __FILE__, __LINE__, gluErrorString(error));
		Console->Output(buffer);
	}
}

void glPrintText(float x, float y, char* text, ...)
{
	char	strText[255];
	va_list	argumentPtr;								// This will hold the pointer to the argument list

	va_start(argumentPtr, text);						// Parse the arguments out of the string

	vsprintf(strText, text, argumentPtr);				// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	Font.PrintText(strText, x, y);
}

void glPrintText(float x, float y, string* text, ...)
{
	char	strText[255];
	va_list	argumentPtr;														// This will hold the pointer to the argument list

	va_start(argumentPtr, text);												// Parse the arguments out of the string

	vsprintf(strText, text->c_str(), argumentPtr);								// Now add the arguments into the full string

	va_end(argumentPtr);														// This resets and frees the pointer to the argument list.

	Font.PrintText(strText, x, y);
}

CVector3 glPosition3D(int x, int y)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ );

    return CVector3( (GLfloat)posX, (GLfloat)posY, (GLfloat)posZ );
}

/*
 * Multiply the current ModelView-Matrix with a shadow-projetion
 * matrix.
 *
 * l is the position of the light source
 * e is a point on within the plane on which the shadow is to be
 *   projected.  
 * n is the normal vector of the plane.
 *
 * Everything that is drawn after this call is "squashed" down
 * to the plane. Hint: Gray or black color and no lighting 
 * looks good for shadows *g*
 */
void glShadowProjection(float* l, float* e, float* n)
{
  float d, c;
  float mat[16];

  d = n[0]*l[0] + n[1]*l[1] + n[2]*l[2];
  c = e[0]*n[0] + e[1]*n[1] + e[2]*n[2] - d;

  // Create the matrix. OpenGL uses column by column ordering

  mat[0]  = l[0]*n[0]+c; 
  mat[4]  = n[1]*l[0]; 
  mat[8]  = n[2]*l[0]; 
  mat[12] = -l[0]*c-l[0]*d;
  
  mat[1]  = n[0]*l[1];        
  mat[5]  = l[1]*n[1]+c;
  mat[9]  = n[2]*l[1]; 
  mat[13] = -l[1]*c-l[1]*d;
  
  mat[2]  = n[0]*l[2];        
  mat[6]  = n[1]*l[2]; 
  mat[10] = l[2]*n[2]+c; 
  mat[14] = -l[2]*c-l[2]*d;
  
  mat[3]  = n[0];        
  mat[7]  = n[1]; 
  mat[11] = n[2]; 
  mat[15] = -d;

  // Finally multiply the matrices together
  glMultMatrixf(mat);
}

/*
	Function -- makeBGR
	
	Converts a BGR buffer to RGB
*/
void makeBGR(unsigned char *p, int size)
{
	unsigned char temp;
	int i;

	for(i = 0; i < size * 3; i += 3)
	{
		temp = p[i];
		p[i] = p[i + 2];
		p[i + 2] = temp;
	}
}

/*
	Function -- screenShot
	
	Read the current image data from the frame buffer and write it to a file
*/
bool makeScreenShot(char *fName, int winW, int winH)
{
	unsigned char *fBuffer;

	//fBuffer = (unsigned char*)malloc(winW * winH * 3);
	fBuffer = (unsigned char*)calloc(winW * winH * 3, sizeof(unsigned char));

	// no memory allocated for image data
	if(fBuffer == NULL)
		return false;

	// read our image data from the frame buffer
	glReadPixels(0, 0, winW, winH, GL_RGB, GL_UNSIGNED_BYTE, fBuffer);

	// write the image data to a .tga file
	writeTGA(fName, fBuffer, winW, winH);

	free(fBuffer);

	return true;
}

inline int mathsNormalize(float *v)
{
	float d = (sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2])));
	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;

	return 0;
}

inline float mathsSqr(float a)
{
	return a * a;
}

void PreProcessSkyBox(float x, float y, float z, float width, float height, float length)
{
	// Destroy display list if already exists.
	if(glIsList(MeshID_SkyBox))
		glDeleteLists(MeshID_SkyBox, 1);

	// Ask OpenGL for a new display list number
	MeshID_SkyBox = glGenLists(1);
	if(MeshID_SkyBox == 0)
	{
		//TODO : throw an exception
		return;
	}

	glNewList(MeshID_SkyBox, GL_COMPILE);

		// This centers the sky box around (x, y, z)
		//x = x - width  / 2;
		//y = y - height / 2;
		//z = z - length / 2;

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);									// Turn on texture mapping if it's not already

		glDisable(GL_TEXTURE_GEN_S);								// Enable Texture Coord Generation For S (NEW)
		glDisable(GL_TEXTURE_GEN_T);								// Enable Texture Coord Generation For T (NEW)

		// Bind the BACK texture of the sky map to the BACK side of the cube
		g_Manager_Texture->SetActiveTexture("Back.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		glEnd();

		// Bind the FRONT texture of the sky map to the FRONT side of the box
		g_Manager_Texture->SetActiveTexture("Front.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glEnd();

		// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
		g_Manager_Texture->SetActiveTexture("Bottom.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glEnd();

		// Bind the TOP texture of the sky map to the TOP side of the box
		g_Manager_Texture->SetActiveTexture("Top.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		glEnd();

		// Bind the LEFT texture of the sky map to the LEFT side of the box
		g_Manager_Texture->SetActiveTexture("Left.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		glEnd();

		// Bind the RIGHT texture of the sky map to the RIGHT side of the box
		g_Manager_Texture->SetActiveTexture("Right.bmp");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
		glEnd();

	glEndList();
}

void RenderBullets()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.7f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	float a, b, c, d;

	for(int i = g_Manager_Bullet.Count - 1; i >= 0; i -= 1)
    {
		if(g_Manager_Bullet.Bullet[i].iFrom == g_iLocalPlayer)
		{
			a = mathsSqr(g_Manager_Bullet.Bullet[i].mPosition.x - g_Manager_Player.Player[g_Manager_Bullet.Bullet[i].iFrom].mPosition.x);
			b = mathsSqr(g_Manager_Bullet.Bullet[i].mPosition.y - g_Manager_Player.Player[g_Manager_Bullet.Bullet[i].iFrom].mPosition.y);
			c = mathsSqr(g_Manager_Bullet.Bullet[i].mPosition.z - g_Manager_Player.Player[g_Manager_Bullet.Bullet[i].iFrom].mPosition.z);
			d = sqrt(a + b + c);

			if(d <= 20.0f)
				continue;
		}

	    glPushMatrix();
			glTranslatef(g_Manager_Bullet.Bullet[i].mPosition.x, g_Manager_Bullet.Bullet[i].mPosition.y, g_Manager_Bullet.Bullet[i].mPosition.z);

			if(g_Manager_Bullet.Bullet[i].iModel >= 0)
			{
				g_Manager_3DSObject.Mesh[g_Manager_Bullet.Bullet[i].iModel]->Render();
			}
			else
			{
				Texture_SetActiveID(g_Manager_Player.Player[g_Manager_Bullet.Bullet[i].iFrom].Weapon.iTexture_Bullet);
				glColor3f(1.0f, 1.0f, 1.0f);

				// For testing purposes only - BEGIN
				float cam[3];
				cam[0] = g_Camera.Position().x;
				cam[1] = g_Camera.Position().y;
				cam[2] = g_Camera.Position().z;

				float worldPos[3];
				worldPos[0] = g_Manager_Bullet.Bullet[i].mPosition.x;
				worldPos[1] = g_Manager_Bullet.Bullet[i].mPosition.y;
				worldPos[2] = g_Manager_Bullet.Bullet[i].mPosition.z;

				BillBoard_Spherical_Begin(cam, worldPos);
					glBegin(GL_QUADS);
						glTexCoord2f(0.0f, 0.0f);
						glVertex3f(-8.0f, -8.0f, 0.0f);
						glTexCoord2f(1.0f, 0.0f);
						glVertex3f(8.0f, -8.0f, 0.0f);
						glTexCoord2f(1.0f, 1.0f);
						glVertex3f(8.0f, 8.0f, 0.0f);
						glTexCoord2f(0.0f, 1.0f);
						glVertex3f(-8.0f, 8.0f, 0.0f);
					glEnd();
				BillBoard_End();
				// For testing purposes only - END
			}
		glPopMatrix();
    }

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	glColor3f(Map.fAmbientColor[0], Map.fAmbientColor[1], Map.fAmbientColor[2]);
}

void RenderGrass()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.3f);

	glEnable(GL_TEXTURE_2D);

	static float fsinus = 0.0f;
	if(Map.vWind.x != 0.0f)
		fsinus += 0.003f * sin((float)timeGetTime() / 750.0f);
	else
		fsinus = 0.0f;

	static float fsinus2 = 0.0f;
	if(Map.vWind.z != 0.0f)
		fsinus2 += 0.003f * sin((float)timeGetTime() / 750.0f);
	else
		fsinus2 = 0.0f;

	Texture_SetActive("Grass.tga");

	for(unsigned int x = 15; x > 0; x -= 3)
	{
		for(unsigned int z = 15; z > 0; z -= 3)
		{
			glPushMatrix();
				glTranslatef(50.0f + x, QuadTree.getHeight(50.0f + x, 50.0f + z) + 0.1f, 50.0f + z);

				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-2.0f, 0.0f, 0.0f);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f(2.0f, 0.0f, 0.0f);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f(2.0f + fsinus, 4.0f, 0.0f + fsinus2);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-2.0f + fsinus, 4.0f, 0.0f + fsinus2);

					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(0.0f, 0.0f, -2.0f);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f(0.0f, 0.0f, 2.0f);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f(0.0f + fsinus, 4.0f, 2.0f + fsinus2);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(0.0f + fsinus, 4.0f, -2.0f + fsinus2);
				glEnd();

			glPopMatrix();
		}
	}

	glDisable(GL_ALPHA_TEST);
}

void RenderImage2D(char FName[255], float left, float top, float width, float height)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();								// Push on a new matrix so that we can just pop it off to go back to perspective mode
		glLoadIdentity();						// Reset the current matrix to our identify matrix

		// Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).
		// The last 2 parameters are the near and far planes.
		glOrtho(0, Engine->Screen.Width, 0, Engine->Screen.Height, 0, 10);

		glEnable(GL_TEXTURE_2D);

		Texture_SetActive(FName);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(left, top);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(left + width, top);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(left + width, top + height);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(left, top + height);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();           
	glMatrixMode(GL_MODELVIEW);					// Go back to our model view matrix like normal
}

void RenderImageAtFullScreen(char FName[255])
{
	RenderImage2D(FName, 0.0f, 0.0f, (float)Engine->Screen.Width, (float)Engine->Screen.Height);
}

void RenderInterface()
{
	//sprintf_s(g_sWindowText, "x = %f, y = %f, z = %f", g_Manager_Player.mPlayer[g_iLocalPlayer].vPosition.x, g_Manager_Player.mPlayer[g_iLocalPlayer].vPosition.y, g_Manager_Player.mPlayer[g_iLocalPlayer].vPosition.z);
	//SetWindowText(g_hWnd, g_sWindowText);

	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);

	char buffer[255] = "";

	// Render FPS - Begin
	if(Engine->iFPS >= 60)
		glColor3f(0.0f, 1.0f, 0.0f);
	if(Engine->iFPS < 60 && Engine->iFPS >= 30)
		glColor3f(1.0f, 1.0f, 0.0f);
	if(Engine->iFPS < 30)
		glColor3f(1.0f, 0.0f, 0.0f);

	glPrintText(-0.95f, 0.9f, "FPS: %i", Engine->iFPS);
	// Render FPS - End

	if(Game.bPlayMode)
	{
		//glPrintText(-0.95f, 0.84f, "%d, %d", Mouse.mp.x, Mouse.mp.y);
		Font.drawString("Hallo", -0.95f, 0.84f);

		if(Console->IsVisible())
		{
			Console->Render();
		}
		else if(GUI.isactive(&Gameform))
		{
			if(!Game.bScriptSequence)
			{
				// Player Stats
				glColor3f(1.0f, 0.0f, 0.0f);
				glPrintText(-0.95f, -0.9f, "%i", Game.mPlayer->iHealth);
				glColor3f(1.0f, 1.0f, 1.0f);
				glPrintText(-0.85f, -0.9f, "|");
				glColor3f(0.0f, 0.0f, 1.0f);
				glPrintText(-0.8f, -0.9f, "%i", Game.mPlayer->iAmor);
				glColor3f(0.0f, 1.0f, 0.0f);
				glPrintText(0.85f, -0.9f, "%i", Game.mPlayer->Weapon.iBullets);


				if(Engine->bShowStats)
				{
					glPrintText(-0.35f, 0.45f, "-----------------------------------------------------------");
					glPrintText(-0.35f, 0.40f, "| Player");
					glPrintText(-0.05f, 0.40f, "Kills");
					glPrintText( 0.15f, 0.40f, "Deaths");
					glPrintText( 0.35f, 0.40f, "Team");
					glPrintText( 0.50f, 0.40f, "Id");
					glPrintText( 0.55f, 0.40f, "|");

					for(int i = 0; i < g_Manager_Statistic.Count; i += 1)
					{
						glPrintText(-0.35f, 0.3f - i * 0.1f, g_Manager_Statistic.Statistic[i].cText);

						// DEBUG - for testing purposes only - Begin
						glPrintText(-0.95f, 0.83f - i * 0.09f, "%s %i X=%.2f/Z=%.2f", g_Manager_Player.Player[i].getName(), g_Manager_Player.Player[i].iHealth, g_Manager_Player.Player[i].mPosition.x, g_Manager_Player.Player[i].mPosition.z);
						//glPrintText(-0.95f, 0.83f - i * 0.18f - 0.09f, "%s OX=%d/OZ=%d", g_Manager_Player.Player[i].Name, g_Manager_Player.Player[i].iOctreeX, g_Manager_Player.Player[i].iOctreeZ);
						// DEBUG - for testing purposes only - End
					}

					glPrintText(-0.35f, 0.35f -  g_Manager_Player.Count * 0.1f, "-----------------------------------------------------------");
				}
			}
		}
	}

	if(!Game.bPlayMode)
	{
		glColor3f(1.0f, 0.0f, 0.0f);

		glPrintText(-0.95f, 0.9f, g_sEditMode);

		switch(g_iEditMode)
		{
		case EditorSpace::Selection:
			sprintf(buffer, "Axis:");

			if(Editor->bAllowMoveX)
				sprintf(buffer, "%s X", buffer);
			if(Editor->bAllowMoveY)
				sprintf(buffer, "%s Y", buffer);
			if(Editor->bAllowMoveZ)
				sprintf(buffer, "%s Z", buffer);

			glPrintText(-0.95f, 0.85f, buffer);
			break;
		case EditorSpace::Water:
			glPrintText(-0.95f, 0.85f, "Water height: %f", Map.fWaterHeightBase);
			break;
		case EditorSpace::Fog:
			glPrintText(-0.95f, 0.85f, "Water height: %f", Map.fFogDensity);
			break;
		case EditorSpace::Light:
			{
				CVector3 pos = Light[g_iLightSource - 1].getPosition();
				glPrintText(-0.95f, 0.85f, "Light %i: %i %i %i %i", g_iLightSource, Light[g_iLightSource - 1].getEnabled(), (int)pos.x, (int)pos.y, (int)pos.z);
				break;
			}
		case EditorSpace::Terrain:
			glPrintText(-0.95f, 0.85f, "X, Y, Z: %.1f, %.1f, %.1f", Mouse.mp3d.x, Mouse.mp3d.y, Mouse.mp3d.z);
			glPrintText(-0.95f, 0.80f, "X: %d Z: %d", g_iOctreeX, g_iOctreeZ);
			glPrintText(-0.95f, 0.75f, "X: %d Z: %d", g_iVertexX, g_iVertexZ);
			break;
		}

		if(Engine->bDrawFloor)
			glPrintText(-0.95f, -0.95f, "F");
		if(Engine->bDrawRoof)
			glPrintText(-0.90f, -0.95f, "R");
		if(Engine->bLightModelTwoSided)
			glPrintText(-0.85f, -0.95f, "L");
		if(Engine->bColorMaterial)
			glPrintText(-0.80f, -0.95f, "C");
		if(Mouse.Button1 == MouseSpace::Down)
			glPrintText(-0.95f, -0.90f, "MB1");
		if(Mouse.Button2 == MouseSpace::Down)
			glPrintText(-0.90f, -0.90f, "MB2");
		if(Mouse.Button3 == MouseSpace::Down)
			glPrintText(-0.85f, -0.90f, "MB3");

		//if(Console->bVisible)
		//	Console->Draw();
	}

	if(Scene.iActive != -1)
	{
		glPrintText(-0.95f, 0.8f, "Object: %i", Scene.iActive);
	}

	//if(!g_Manager_Menu.bVisible)
	//	g_Manager_Chat.Render();

	glEnable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void RenderMouse(char MouseEvent[20])
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.1f);

	char buffer[40] = "";
	sprintf(buffer, "Mouse%s.tga", MouseEvent);
	
	POINT mp = GetClientMousePosition();		// Gets The Current Client Cursor Coordinates (Mouse Coordinates)

	RenderImage2D(buffer, (float)mp.x - 9.0f, (float)-mp.y + Engine->Screen.Height - 29.0f, 32.0f, 32.0f);
	
	glDisable(GL_ALPHA_TEST);
}

void RenderPlayers()
{
	glEnable(GL_TEXTURE_2D);

	for(int i = g_Manager_Player.Count - 1; i >= 0; i -= 1)		// counting down is faster than counting up
    {
	    if(		(Game.bPlayMode && g_iLocalPlayer != i && !g_Manager_Player.Player[i].bAI)
			||	(!Game.bPlayMode && g_iLocalPlayer == i)
			)
	    {
		    glPushMatrix();
				glTranslatef(g_Manager_Player.Player[i].mPosition.x, g_Manager_Player.Player[i].mPosition.y, g_Manager_Player.Player[i].mPosition.z);
				//glRotatef(g_Manager_Player.Player[i].mRotation.x, 1.0f, 0.0f, 0.0f);
				glRotatef(g_Manager_Player.Player[i].mRotation.y + 100.0f, 0.0f, 1.0f, 0.0f);
				//glRotatef(g_Manager_Player.Player[i].mRotation.z, 0.0f, 0.0f, 1.0f);

				Material_Apply(g_Manager_Entity.Object[g_Manager_Player.Player[i].iModel].iMaterial, false);
				g_Manager_Entity.Object[g_Manager_Player.Player[i].iModel].Render();

				// Shadows
				if(Engine->iShadows >= 2)
				{
					glPushMatrix();
						glDisable(GL_LIGHTING);
						glDisable(GL_TEXTURE_2D);

						//glRotatef(-g_Manager_Player.Player[i].mRotation.x, 1.0f, 0.0f, 0.0f);
						glRotatef(-g_Manager_Player.Player[i].mRotation.y, 0.0f, 1.0f, 0.0f);
						//glRotatef(-g_Manager_Player.Player[i].mRotation.z, 0.0f, 0.0f, 1.0f);

						CVector3 pos = Light[LIGHT_SUN].getPosition();
						l[0] = pos.x - g_Manager_Player.Player[i].mPosition.x;
						l[1] = pos.y - g_Manager_Player.Player[i].mPosition.y;
						l[2] = pos.z - g_Manager_Player.Player[i].mPosition.z;
						
						glShadowProjection(l, e, n);
						glColor3f(0.01f, 0.01f, 0.01f);

						g_Manager_Entity.Object[g_Manager_Player.Player[i].iModel].Render();
						
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_LIGHTING);
					glPopMatrix();
				}
			glPopMatrix();
	    }
    }

	//CELL Shading BEGIN
    if(Engine->bCellShading)
    {
		glDisable(GL_LIGHTING);
	              
		glEnable(GL_BLEND);									// Enable Blending ( NEW )
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);   // Set The Blend Mode ( NEW )
	              
		glPolygonMode(GL_FRONT, GL_LINE);					// Draw Backfacing Polygons As Wireframes ( NEW )
		glLineWidth(5.0f);									// Set The Line Width ( NEW )

		glCullFace(GL_BACK);								// Don't Draw Any Front-Facing Polygons ( NEW )

		glDepthFunc(GL_LEQUAL);								// Change The Depth Mode ( NEW )

		glColor3f(0.0f, 0.0f, 0.0f);						// Set The Outline Color ( NEW )

		for(int i = g_Manager_Player.Count - 1; i >= 0; i -= 1)		// counting down is faster than counting up
		{
			if(!Game.bPlayMode || g_iLocalPlayer != i)
			{
				glPushMatrix();
					glTranslatef(g_Manager_Player.Player[i].mPosition.x, g_Manager_Player.Player[i].mPosition.y, g_Manager_Player.Player[i].mPosition.z);
					glRotatef(g_Manager_Player.Player[i].mRotation.y + 100.0f, 0.0f, 1.0f, 0.0f);

					g_Manager_Entity.Object[g_Manager_Player.Player[i].iModel].Render();
				glPopMatrix();
			}
		}
	              
		glDepthFunc(GL_LESS);								// Reset The Depth-Testing Mode ( NEW )

		glPolygonMode(GL_FRONT, GL_FILL);					// Reset Back-Facing Polygon Drawing Mode ( NEW )

		glDisable(GL_BLEND);								// Disable Blending ( NEW )
		glEnable(GL_LIGHTING);
    }
    //CELL Shading END
}

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene()
{
	glDepthMask(GL_TRUE);

	if(!Map.bWaterEnabled)
	{
		glLoadIdentity();
		g_Camera.Look();
	}

	if(Gameform.IsVisible() && !Mainform.IsVisible() && !Console->IsVisible() && !g_bLoading && Map.bWaterEnabled)
	{
		// Render the screen to a texture for our reflection, refraction and depth.
		// The texture size is for our view port, and the water height is for clipping.
		if(Map.bWaterReflections && Frustum.volumeInFrustum(Water.BoundingVolume))
			Water.CreateReflectionTexture(Map.fWaterHeight, g_TextureSize);
		//if(Map.bWaterRefractions && Frustum.volumeInFrustum(Water.BoundingVolume))
		//	Water.CreateRefractionDepthTexture(Map.fWaterHeight, g_TextureSize);
	}

	if(Engine->bCellShading)													// Clear our buffer bits
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glPushMatrix();																// Push a new matrix
	glLoadIdentity();															// Reset our current matrix

	glEnable (GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glColor3f(1.0f, 1.0f, 1.0f);												// !BUG FIX!: Set current color to white

	Material_Apply(0, true);													// !BUG FIX!: Reset material to ensure correct rendering of menu and console

	if(!Game.bPlayMode)															// Render editor preview
	{
		glPushMatrix();
			glViewport(Engine->Screen.Width / 40, Engine->Screen.Height / 4 * 3 - 48, Engine->Screen.Width / 4, Engine->Screen.Height / 4);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//RenderImage2D("ConsoleBackground.bmp", 0.0f, 0.0f, (float)Engine->Screen.Width, (float)Engine->Screen.Height);

			CGraphicObject* go = NULL;

			switch(g_iEditMode)
			{
			case EditorSpace::max3ds:
				if(InsertIndex < (int)g_Manager_3DSObject.Mesh.size())
					go = g_Manager_3DSObject.Mesh[InsertIndex];
				break;
			case EditorSpace::md2:
				if(InsertIndex < g_Manager_Entity.Count)
					go = &g_Manager_Entity.Object[InsertIndex];
				break;
			case EditorSpace::Static:
				if(InsertIndex < g_Manager_StaticObject.Count)
					go = &g_Manager_StaticObject.Object[InsertIndex];
				break;
			case EditorSpace::Bot:
				if(InsertIndex < g_Manager_Bot.Count)
					go = g_Manager_Bot.Bot[InsertIndex].Player;
				break;
			case EditorSpace::Particle:
				if(InsertIndex < g_Manager_ParticleSystem.Count)
					go = &g_Manager_ParticleSystem.Object[InsertIndex];
				break;
			}

			if(go != NULL)
			{
				float width = sqrt(go->BoundingVolume.X1 * go->BoundingVolume.X1 + go->BoundingVolume.X2 * go->BoundingVolume.X2);
				float height = sqrt(go->BoundingVolume.Y1 * go->BoundingVolume.Y1 + go->BoundingVolume.Y2 * go->BoundingVolume.Y2);
				float depth = sqrt(go->BoundingVolume.Z1 * go->BoundingVolume.Z1 + go->BoundingVolume.Z2 * go->BoundingVolume.Z2);
				float ratio = ((width + height + depth) / 3) * 4.0f;

				//glTranslatef(0.0f, 0.0f, -ratio);
				glTranslatef(-go->BoundingVolume.CX, -go->BoundingVolume.CY, -go->BoundingVolume.CZ - ratio);

				static float roty = 0.0f;
				roty += 1.0f;
				glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
				glRotatef(roty, 0.0f, 1.0f, 0.0f);

				Material_Apply(go->iMaterial, false);

				if(g_iEditMode == EditorSpace::Particle)
				{
					//g_Manager_ParticleSystem.Object[InsertIndex].Render();
				}
				else
				{
					go->Render();
				}
			}
		glPopMatrix();
	}


	if(Game.bScriptSequence)													// Change viewport
	{
		glViewport(0, Engine->Screen.Height / 8, Engine->Screen.Width, Engine->Screen.Height / 8 * 6);
	}
	else
	{
		glViewport(0, 0, Engine->Screen.Width, Engine->Screen.Height);
	}


	if(Gameform.IsVisible() && !Mainform.IsVisible() && Game.bPlayMode && !g_bLoading && !Console->IsVisible() && g_Camera.iMode == CAMERA_MODE_FPS  && !Game.bScriptSequence)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, 0.1f);
		RenderImage2D("Crosshair0.tga", (float)Engine->Screen.Middle.x - 16.0f, (float)Engine->Screen.Middle.y - 16.0f, 32.0f, 32.0f);
		glDisable(GL_ALPHA_TEST);
	}
	//else if(!g_bLoading && (g_Manager_Menu.bVisible || g_Manager_Menu.bInGameMenu || g_Camera.iMode != CAMERA_MODE_FPS))
	//{
	//	RenderMouse("");
	//}

	if(g_bLoading)																// Render loading screen
	{
		RenderImageAtFullScreen("Screen_Loading.bmp");

		glColor3f(0.55f, 0.41f, 0.0f);
		glPrintText(-0.2f, 0.0f, g_sStatusMessage);
	}
	else
	{
		if(Console->IsVisible())													// Render console background
		{
			RenderImageAtFullScreen("ConsoleBackground.bmp");
			Console->Render();
		}

		// Post effects
		//if(Game.bScriptSequence)
		//{
		//	glEnable(GL_ALPHA_TEST);
		//	glAlphaFunc(GL_GEQUAL, 0.1f);
		//	RenderImageAtFullScreen("PostEffect_Film.tga");
		//	glDisable(GL_ALPHA_TEST);
		//}
	}

	if(Gameform.IsVisible() && !Mainform.IsVisible() && !Console->IsVisible())
	{
		if(Light[LIGHT_FLASHLIGHT].getEnabled())
			Light[LIGHT_FLASHLIGHT].setLight();

		// Set the current camera position and view
		SetCameraPosition();
		g_Camera.Look();
		Frustum.Update();
		Scene.setCameraPosition(g_Camera.Position());
	}


	if(Map.bFirstFrame)															// Ensure that the initial view is set according to the map data
	{
		Map.bFirstFrame = false;
		g_Camera.PositionCamera(
								Map.vSinglePlayerStartPoint.x,
								Map.vSinglePlayerStartPoint.y,
								Map.vSinglePlayerStartPoint.z,
								Map.vSinglePlayerStartAngle.x,
								Map.vSinglePlayerStartAngle.y,
								Map.vSinglePlayerStartAngle.z,
								0.0f,
								1.0f,
								0.0f
								);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_FOG);

	if(Gameform.IsVisible() && !Mainform.IsVisible() && !Console->IsVisible() && !g_bLoading)		// Render the caustics, terrain and the sky box and the rest
	{
		for(int i = 0; i < Engine->iMaxLights; i += 1)
		{
			if(Light[i].getEnabled())
				Light[i].setLight();
		}

		if(g_iLocalPlayer != -1 && Game.bPlayMode)									// Update local players view coords
		{
			Game.mPlayer->mRotation.x = g_fCameraAngleX;
			Game.mPlayer->mRotation.y = g_fCameraAngleY;
			Game.mPlayer->mRotation.z = g_fCameraAngleZ;
		}

		if(Map.bWaterEnabled)
		{
			if(g_Camera.Position().y > Map.fWaterHeight)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);
			
			Water.Render();
			
			if(g_Camera.Position().y > Map.fWaterHeight)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		}

		RenderWorld();
	}


	glPopMatrix();

	glEnable(GL_LIGHTING);

	if(Gameform.IsVisible() && !Mainform.IsVisible() && !Console->IsVisible() && !g_bLoading && g_iLocalPlayer != -1)		// Render local player's weapon
	{
		if ( Game.mPlayer->Weapon.iModel != -1 ) {
			g_Manager_Material.Apply(g_Manager_3DSObject.Mesh[Game.mPlayer->Weapon.iModel]->iMaterial, false);
			RenderWeapon();
		}
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);


	if(!g_bLoading && !Console->IsVisible())									// Render interface
	{
		GUI.render();
		RenderInterface();

		if(Engine->fMotionBlur > 0.0f)											// Do motion blur (if enabled)
		{
			glAccum(GL_MULT, Engine->fMotionBlur);
			glAccum(GL_ACCUM, 1.0f - Engine->fMotionBlur);
			glAccum(GL_RETURN, 1.0f);

			glFlush();
		}
	}

	// Bind the current scene to our "ingame" texture
	//glBindTexture(GL_TEXTURE_2D, g_Texture[INGAME_ID]);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, Engine->Screen.Width, Engine->Screen.Height);

	SwapBuffers(g_hDC);															// Swap the backbuffers to the foreground

	if(g_bLoading)
	{
		return;
	}

	if(g_bInsertInWorld)
	{
		if(Game.bPlayMode)
		{
			switch(Game.iGameMode_Command)
			{
			case MODE_GAME_COMMAND_STAYHERE:
				g_Manager_Bot.Bot[g_Manager_Bot.iActive].WayPointsClear();
				break;
			case MODE_GAME_COMMAND_SENDBOT:
				g_Manager_Bot.Bot[g_Manager_Bot.iActive].WayPointsClear();
				g_Manager_Bot.Bot[g_Manager_Bot.iActive].WayPointAdd(Mouse.mp3d.x, Mouse.mp3d.z);

				Game.iGameMode_Command = 0;
				break;
			}
		}

		g_bInsertInWorld = false;
	}
}

void RenderSkyBox()
{
	glCullFace(g_iSkyBoxFaceCulling);

	if(!Map.bFogAffectsSkybox)
		glDisable(GL_FOG);

	Material_Apply(Map.iMaterial, true);

	glCallList(MeshID_SkyBox);

	if(!Map.bFogAffectsSkybox)
		glEnable(GL_FOG);
}

void RenderSkyPlane()
{
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	Texture_SetActive("Sky.bmp");
	glBegin(GL_QUADS);
		glVertex3f(-50.0f, 100.0f, -50.0f);
		glVertex3f(50.0f, 100.0f, -50.0f);
		glVertex3f(50.0f, 100.0f, 50.0f);
		glVertex3f(-50.0f, 100.0f, 50.0f);
	glEnd();

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glEnable(GL_FOG);
}

void RenderStatusMessage(char* MessageText)
{
	g_sStatusMessage = MessageText;
	RenderScene();
}

void RenderTest1()
{
	glDisable(GL_CULL_FACE);

	int uniform = -1;

	glPushMatrix();
		glTranslatef(60.0f, QuadTree.getHeight(60.0f, 0.0f), 0.0f);

		Shader->Begin("BumpMap");
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D); 
			Texture_SetActive("bumpmap2_test1_normal.jpg");

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			Texture_SetActive("bumpmap2_test1_color.jpg");

			uniform = glGetUniformLocationARB(Shader->GetProgram(), "normalMap");
			glUniform1iARB(uniform, 0);

			uniform = glGetUniformLocationARB(Shader->GetProgram(), "colorMap"); 
			glUniform1iARB(uniform, 1);

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 0.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 50.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 0.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 50.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 50.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 50.0f);
			glEnd();

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);

			// Reset multi texture to first texture slot
			glActiveTextureARB(GL_TEXTURE0_ARB);
		Shader->End();
	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void RenderTest2(Texture &texture)
{
	glDisable(GL_CULL_FACE);

	glPushMatrix();
		glTranslatef(100.0f, QuadTree.getHeight(100.0f, 100.0f), 100.0f);

		Texture_SetActiveID(texture.getId());

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(50.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(50.0f, 50.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, 50.0f, 0.0f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 50.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.0f, 50.0f, 50.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, 50.0f, 0.0f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 50.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(50.0f, 0.0f, 50.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(50.0f, 50.0f, 50.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, 50.0f, 50.0f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(50.0f, 0.0f, 50.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(50.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(50.0f, 50.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(50.0f, 50.0f, 50.0f);
		glEnd();

	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void RenderTest3()
{
	glDisable(GL_CULL_FACE);

	int uniform = -1;

	glPushMatrix();
		glTranslatef(60.0f, QuadTree.getHeight(60.0f, 0.0f), 0.0f);

		Shader->Begin("Fresnel Reflection");
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D); 
			Texture_SetActive("bumpmap2_test1_normal.jpg");

			uniform = glGetUniformLocationARB(Shader->GetProgram(), "cubemap");
			glUniform1iARB(uniform, 0);

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 0.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 50.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 0.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 50.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, 50.0f, 50.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 50.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 50.0f);
			glEnd();

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);

			// Reset multi texture to first texture slot
			glActiveTextureARB(GL_TEXTURE0_ARB);
		Shader->End();
	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void RenderToTexture(Texture &texture)
{
	if ( !texture.getId() )
		return;

	glDepthMask(GL_TRUE);

	glViewport(0, 0, texture.getWidth(), texture.getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	g_Camera.Look();
	Frustum.Update();
	Scene.setCameraPosition(g_Camera.Position());

	glPushMatrix();																// Push a new matrix

		glEnable(GL_TEXTURE_2D);

		glColor3f(1.0f, 1.0f, 1.0f);											// !BUG FIX!: Set current color to white

		Material_Apply(0, true);												// !BUG FIX!: Reset material to ensure correct rendering of menu and console

		for(int i = 0; i < Engine->iMaxLights; i += 1)
		{
			if(Light[i].getEnabled())
				Light[i].setLight();
		}

		RenderWorld();

		glFlush();

		// Bind the current scene to our "ingame" texture
		glBindTexture(GL_TEXTURE_2D, texture.getId());
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texture.getWidth(), texture.getHeight());

	glPopMatrix();
}

void RenderWayPoints(CBot *bot)
{
	if(bot == NULL)
		return;
	if(!bot->Player == NULL || bot->iWayPointCount <= 0)
		return;
	if(!bot->Player->bAI)
		return;

	glLineWidth(3.0f);

	glBegin(GL_LINE);
		for(int i = 0; i < bot->iWayPointCount; i += 1)
		{
			glVertex3f(bot->vWayPoint[i].x, bot->vWayPoint[i].y + 3.0f, bot->vWayPoint[i].z);
		}
	glEnd();

	for(int i = 0; i < bot->iWayPointCount; i += 1)
	{
		DrawMousePoint(bot->vWayPoint[i].x, bot->vWayPoint[i].y, bot->vWayPoint[i].z);
	}

	glLineWidth(1.0f);
}

void RenderWeapon()
{
	if(Console->IsVisible())
		return;

	glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);

		if(Game.bPlayMode && Game.mPlayer->Weapon.iModel != -1)
		{
			static float movex = 0.0f;
			static float movey = 0.0f;

			// Lissajous - Begin
			if(Engine->Keybinding.kbRun.bPressed && (Engine->Keybinding.kbBackward.bPressed || Engine->Keybinding.kbForward.bPressed || Engine->Keybinding.kbStrafeLeft.bPressed || Engine->Keybinding.kbStrafeRight.bPressed))
			{
				// schnell
				movex = float(0.2f * sin((float)timeGetTime() / 250));
				movey = float(0.05f * sin(2 * (float)timeGetTime() / 250));
			}
			else if(Engine->Keybinding.kbBackward.bPressed || Engine->Keybinding.kbForward.bPressed || Engine->Keybinding.kbStrafeLeft.bPressed || Engine->Keybinding.kbStrafeRight.bPressed)
			{
				// mittel
				movex = 0.15f * sin((float)timeGetTime() / 1000);
				movey = 0.1f * sin(2 * (float)timeGetTime() / 1000);
			}
			else
			{
				// langsam
				movex = 0.05f * sin((float)timeGetTime() / 1000);
				movey = 0.025f * sin(2 * (float)timeGetTime() / 1000);
			}
			// Lissajous - End

			glDisable(GL_CULL_FACE);

			glPushMatrix();
				if(Game.mPlayer->bShoot)
				{
					glTranslatef(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex, Game.mPlayer->Weapon.vDrawPosition.y + movey, Game.mPlayer->Weapon.vDrawPosition.z);
	    			
					if(Game.mPlayer->dTimeBeginShoot + 200 > g_dCurTime)
					{
						if(Game.mPlayer->Weapon.iModel_Muzzle != -1)
						{
							glRotatef(Game.mPlayer->fWeaponFireAngle, 0.0f, 0.0f, 1.0f);
							g_Manager_3DSObject.Mesh[Game.mPlayer->Weapon.iModel_Muzzle]->Render();
						}
						else if(Game.mPlayer->Weapon.iTexture_Muzzle != -1)
						{
							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
							glActiveTextureARB(GL_TEXTURE0_ARB);
							Texture_SetActiveID(Game.mPlayer->Weapon.iTexture_Muzzle);
							//glBindTexture(GL_TEXTURE_2D, g_Manager_Texture->Textures[Game.mPlayer->Weapon.iTexture_Muzzle]->mId);

							glBegin(GL_QUADS);
								glTexCoord2f(0.0f, 0.0f);
								glVertex3f(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex -21.0f, Game.mPlayer->Weapon.vDrawPosition.y + movey -9.0f, Game.mPlayer->Weapon.vDrawPosition.z - 5.0f);
								glTexCoord2f(1.0f, 0.0f);
								glVertex3f(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex +11.0f, Game.mPlayer->Weapon.vDrawPosition.y + movey -9.0f, Game.mPlayer->Weapon.vDrawPosition.z - 5.0f);
								glTexCoord2f(1.0f, 1.0f);
								glVertex3f(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex +11.0f, Game.mPlayer->Weapon.vDrawPosition.y + movey +23.0f, Game.mPlayer->Weapon.vDrawPosition.z - 5.0f);
								glTexCoord2f(0.0f, 1.0f);
								glVertex3f(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex -21.0f, Game.mPlayer->Weapon.vDrawPosition.y + movey +23.0f, Game.mPlayer->Weapon.vDrawPosition.z - 5.0f);
							glEnd();

							glDisable(GL_BLEND);
						}
					}

					if(Game.mPlayer->dTimeBeginShoot + Game.mPlayer->Weapon.fTimePerShot < g_dCurTime)
					{
						Game.mPlayer->Shoot(false);		// Dauerfeuer
					}
				}

				glPopMatrix();
				glPushMatrix();
	    		
				glEnable(GL_LIGHTING);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);

				glTranslatef(Game.mPlayer->Weapon.vDrawPosition.x * Game.mPlayer->iWeaponSide + movex, Game.mPlayer->Weapon.vDrawPosition.y + movey, Game.mPlayer->Weapon.vDrawPosition.z);
				g_Manager_3DSObject.Mesh[Game.mPlayer->Weapon.iModel]->Render();

				glEnable(GL_CULL_FACE);


				//CELL Shading BEGIN
				if(Engine->bCellShading)
				{
					glDisable(GL_LIGHTING);
				              
					glEnable(GL_BLEND);									// Enable Blending ( NEW )
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Set The Blend Mode ( NEW )
				              
					glPolygonMode(GL_FRONT, GL_LINE);					// Draw Backfacing Polygons As Wireframes ( NEW )
					glLineWidth(5.0f);									// Set The Line Width ( NEW )

					glCullFace(GL_BACK);								// Don't Draw Any Front-Facing Polygons ( NEW )

					glDepthFunc(GL_LEQUAL);								// Change The Depth Mode ( NEW )

					glColor3f(0.0f, 0.0f, 0.0f);						// Set The Outline Color ( NEW )

					g_Manager_3DSObject.Mesh[Game.mPlayer->Weapon.iModel]->Render();
				              
					glDepthFunc(GL_LESS);								// Reset The Depth-Testing Mode ( NEW )

					glPolygonMode(GL_FRONT, GL_FILL);					// Reset Back-Facing Polygon Drawing Mode ( NEW )

					glCullFace(GL_FRONT);

					glDisable(GL_BLEND);								// Disable Blending ( NEW )
					glEnable(GL_LIGHTING);
				}
				//CELL Shading END
			glPopMatrix();
		}

	glPopMatrix();
}

///////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the terrain and caustic effects
/////
///////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderWorld()
{
	if(g_bLoading)
		return;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	RenderSkyBox();

	if(Map.bWaterEnabled)
	{
		if(Map.bWaterReflections && g_Camera.Position().y > Map.fWaterHeight)	// If reflections are enabled then turn a clipping plane on and assign our plane equation to it
		{
			glEnable(GL_CLIP_PLANE0);

			double TopTerrainPlane[4] = {0.0, 1.0, 0.0, -Map.fWaterHeight + 0.1f};
			glClipPlane(GL_CLIP_PLANE0, TopTerrainPlane);
		}
	}

	//if(bTeleport)
	//{
	//	glEnable(GL_CLIP_PLANE0);

	//	double TopTerrainPlane[4] = {0.0, 1.0, 0.0, g_Manager_Player.mPlayer[g_iLocalPlayer].GetPosition().y + 5.0f};
	//	glClipPlane(GL_CLIP_PLANE0, TopTerrainPlane);
	//}


	glEnable(GL_LIGHTING);

	QuadTree.Render();

	if(Scene.GetRenderWater() && Map.bWaterReflections)
		glCullFace(GL_BACK);
	else
		glCullFace(GL_FRONT);


	if(Engine->bCellShading)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, Texture_IndexOf("CellShading1.bmp"));

		Shader->Begin("Simple Toon 3");
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
	}

	Scene.Render();																// Render scene

	if(Engine->bCellShading)
	{
		Shader->End();
		glDisable(GL_TEXTURE_1D);

		Scene.BeginCellShading();

			Scene.Render();														// Render cell shaded scene

		Scene.EndCellShading();

		glEnable(GL_TEXTURE_2D);
	}

	if(Scene.GetRenderWater() && Map.bWaterReflections)
		glCullFace(GL_BACK);
	else
		glCullFace(GL_FRONT);

	if(Engine->bCellShading)
	{
		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, Texture_IndexOf("CellShading3.bmp"));

		//Shader->Begin("Cell shading");
		float color[4] = {0.75f, 0.95f, 1.0f, 1.0f};
		//float color[4] = {0.29f, 0.41f, 0.27f, 1.0f};
		Shader->Bind("color", color, 4);
		float g_light_position[4] = {5.0f, 5.0f, 5.0f, 0.0f};
		Shader->Bind("light_position", g_light_position, 4);
	}
	RenderPlayers();															// Render players
	if(Engine->bCellShading)
	{
		Shader->End();
		glDisable(GL_TEXTURE_1D);

		Scene.BeginCellShading();

			RenderPlayers();													// Render cell shaded players

		Scene.EndCellShading();
	}


	RenderTest1();


	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glCullFace(GL_BACK);

	RenderBullets();

	// Draw something at light's position...
	CVector3 pos = Light[g_iLightSource - 1].getPosition();
	DrawMousePoint(pos.x, pos.y, pos.z);

	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);

	RenderGrass();
	//g_Manager_Flora.Render();

	if(!Game.bPlayMode && g_Manager_Region.Count > 0)							// Render regions (if in editor)
	{
		for(int i = g_Manager_Region.Count - 1; i >= 0; i -= 1)
			g_Manager_Region.Region[i].Render();
	}

	if(Engine->bRenderWayPoints)												// Render active bot's waypoints
	{
		if(g_Manager_Bot.iActive != -1)
			RenderWayPoints(&g_Manager_Bot.Bot[g_Manager_Bot.iActive]);
	}

	glEnable(GL_CULL_FACE);

	if(Map.bWaterEnabled)
		glDisable(GL_CLIP_PLANE0);													// Turn our clipping plane off, just in case we've used it
}

double roundd(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

float roundf(float r)
{
	return (r > 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
}

void SetCameraPosition()
{
    // Get the current position of the camera
    CVector3 vPos		= g_Camera.Position();
    CVector3 vNewPos	= vPos;
    CVector3 vView		= g_Camera.View();

	g_Manager_Sound.SetListenerPosition(vPos.x, vPos.y, vPos.z);
	g_Manager_Sound.SetListenerOrientation(g_fCameraAngleX, g_fCameraAngleY, g_fCameraAngleZ, 0.0f, 1.0f, 0.0f);

	if(Game.bScriptSequence)
		return;
    if(g_iLocalPlayer == -1)
	    return;

    vNewPos.y = QuadTree.getHeight(vPos.x, vPos.z) + g_fCameraOverFloor;

	double dtmp = 0.0;
    switch(Game.mPlayer->iJump)
    {
	    case 1:
	    {
		    dtmp = (g_dCurTime - Game.mPlayer->dTimeBeginJump) / 400;

		    if(dtmp < (3.1415f))
			    vNewPos.y += (float)sin(dtmp) * Game.fJumpHeight;
		    else
		    {
			    Game.mPlayer->iJump = 0;
			    Game.mPlayer->dTimeBeginJump = g_dCurTime;
		    }
		    break;
	    }
    }
	
	if(Game.bPlayMode || Engine->bClipping)
	{
		vView.y += vNewPos.y - vPos.y;

		g_Camera.SetPosition(vNewPos);
		g_Camera.SetView(vView);

		vNewPos.y -= g_fCameraOverFloor;
		vView.y -= g_fCameraOverFloor;

		Game.mPlayer->SetPosition(vNewPos);
		Game.mPlayer->SetView(vView);

		Game.mPlayer->iOctreeX = int(Game.mPlayer->mPosition.x / 256);
		Game.mPlayer->iOctreeZ = int(Game.mPlayer->mPosition.z / 256);
	}
}

/*
	Function -- writeTGA
	
	Writes a buffer to a .tga file, buffer should be RGB
*/
void writeTGA(char *name, unsigned char *buff, int w, int h)
{
	unsigned char header[13] = "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned char info[6];
	FILE *pFile;

	char fname[255] = "";
	sprintf(fname, "%s_%s", Map.sMapName, name);
	fopen_s(&pFile, fname, "w");

	// write the header to the file
	fwrite(header, sizeof(char), 12, pFile);

	// image dimension information
	info[0] = (unsigned char)w;
	info[1] = (unsigned char)(w >> 8);
	info[2] = (unsigned char)h;
	info[3] = (unsigned char)(h >> 8);
	info[4] = 24;
	info[5] = 0;

	// write dimension info to file
	fwrite(&info, sizeof(char), 6, pFile);

	// since the frame buffer is RGB we need to convert it to BGR for a targa file
	makeBGR(buff, w * h);

	// dump the image data to the file
	fwrite(buff, sizeof(char), w * h * 3, pFile);

	fclose(pFile);
}