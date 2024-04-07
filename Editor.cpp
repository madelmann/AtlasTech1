
// Library includes

// Project includes
#include "Main.h"


void Editor_ModeSwitch(EditorSpace::Mode_t mode);

namespace EditorSpace {


Editor::Editor(CTextureManager *texturemanager)
: mTextureManager(texturemanager)
{
	bAllowed = false;
	bAllowMoveX = true;
	bAllowMoveY = true;
	bAllowMoveZ = true;
	bEnabled = false;
	bInitialisized = false;

	mMode = Selection;

	//mTextureManager->CreateTexture("Editor\\TranslationX.bmp");
	//mTextureManager->CreateTexture("Editor\\TranslationY.bmp");
	//mTextureManager->CreateTexture("Editor\\TranslationZ.bmp");
}

void Editor::Allow()
{
	bAllowed = true;
}

void Editor::DeInit()
{
	bInitialisized = false;
}

Mode_t Editor::GetMode()
{
	return mMode;
}

void Editor::Init()
{
	if ( bInitialisized ) {
		return;
	}

	if(!bAllowed)
		return;

	bAllowMoveX = true;
	bAllowMoveY = true;
	bAllowMoveZ = true;

	if(PreLoadData())
		bInitialisized = true;
}

Mode_t Editor::NextMode()
{
	switch(mMode)
	{
	case Selection:
		SetMode(max3ds);
		break;
	case max3ds:
		SetMode(md2);
		break;
	case md2:
		SetMode(Static);
		break;
	case Static:
		SetMode(Particle);
		break;
	case Particle:
		SetMode(Bot);
		break;
	case Bot:
		SetMode(Water);
		break;
	case Water:
		SetMode(Fog);
		break;
	case Fog:
		SetMode(Light);
		break;
	case Light:
		SetMode(Terrain);
		break;
	default:
		SetMode(Selection);
		break;
	}

	return mMode;
}

bool Editor::PreLoadData()
{
	g_bLoading = true;

	RenderStatusMessage("Unloading Data...");
	g_Manager_Texture->UnLoadTextures(Texture_IndexOf("Dummy.bmp"));
	g_Manager_3DSObject.Clear();
	g_Manager_Bot.Clear();
	g_Manager_Bullet.Clear();
	g_Manager_Entity.Clear();
	g_Manager_Material.Clear();
	g_Manager_ParticleSystem.Clear();
	g_Manager_Player->Clear();
	g_Manager_Region.Clear();
	g_Manager_Sound.Clear();
	g_Manager_StaticObject.Clear();

	RenderStatusMessage("Loading Materials...");
	PreLoadMaterials();

	RenderStatusMessage("Loading Models...");
	//PreLoadModels();

	RenderStatusMessage("Loading Objects...");
	//PreLoadObjects();

	RenderStatusMessage("Loading Particle Systems...");
	PreLoadParticleSystems();

	RenderStatusMessage("Loading Static Objects...");
	PreLoadStaticObjects();

	RenderStatusMessage("Loading Textures...");
	//PreLoadTextures();

	g_bLoading = false;

	return true;
}

void Editor::PreLoadMaterials()
{
	char buffer[255] = "";
	char buffer2[255] = "";

	sprintf(buffer, "%sMaterials", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "material");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		sprintf(buffer2, "Loading Material (%s) ...", buffer);
		RenderStatusMessage(buffer2);
		Material_Add(buffer);
	}
}

void Editor::PreLoadModels()
{
	char buffer[255] = "";
	char buffer2[255] = "";

	sprintf(buffer, "%sModels", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "md2");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		sprintf(buffer2, "Loading Model (%s) ...", buffer);
		RenderStatusMessage(buffer2);
		g_Manager_Entity.Add(buffer, "");
	}
}

void Editor::PreLoadObjects()
{
	char buffer[255] = "";
	char buffer2[255] = "";

	sprintf(buffer, "%sObjects", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "3ds");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		sprintf(buffer2, "Loading Object (%s) ...", buffer);
		RenderStatusMessage(buffer2);
		g_Manager_3DSObject.Add(buffer);
	}
}

void Editor::PreLoadParticleSystems()
{
	char buffer[255] = "";
	char buffer2[255] = "";

	sprintf(buffer, "%sParticles", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "particle");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());

		sprintf(buffer2, "Loading Particle System (%s) ...", buffer);
		RenderStatusMessage(buffer2);
		g_Manager_ParticleSystem.Add(buffer);
	}
}

void Editor::PreLoadStaticObjects()
{
	char buffer[255] = "";
	char buffer2[255] = "";

	sprintf(buffer, "%sStatic", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "static");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());

		sprintf(buffer2, "Loading Static Object (%s) ...", buffer);
		RenderStatusMessage(buffer2);
		g_Manager_StaticObject.Add(buffer);
	}
}

void Editor::PreLoadTextures()
{
	char buffer[255] = "";
	//char buffer2[255] = "";

	sprintf(buffer, "%sTextures", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "bmp");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());

		g_Manager_Texture->CreateBMPTexture(buffer);
	}

	sprintf(buffer, "%sTextures", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "jpg");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Texture->CreateJPGTexture(buffer);
	}

	sprintf(buffer, "%sTextures", g_sGameName);
	g_Manager_File.Searchfiles(buffer, "tga");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Texture->CreateTGATexture(buffer);
	}
}

void Editor::SetMode(Mode_t mode)
{
	mMode = mode;
	Editor_ModeSwitch(mode);
}

}