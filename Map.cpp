#include <direct.h>
//#include "Map.h"

#include "Main.h"
#include "QuadTree.h"


CMap::CMap()
{
	m_Header.magic = 1;
	m_Header.version = MAP_VERSION_ACTUAL;

	bFogAffectsSkybox = true;
	bSinglePlayer = true;
	bWaterEnabled = true;
	bWaterNormals = true;
	bWaterReflections = true;
	bWaterRefractions = true;
	fAmbientColor[0] = 1.0f;
	fAmbientColor[1] = 1.0f;
	fAmbientColor[2] = 1.0f;
	fFloorOffset = 0.0f;
	fFogDensity = 0.0f;
	fFogDepth = 0.0f;
	fRoofOffset = 0.0f;
	fWaterFlow = 0.0001f;
	fWaterHeight = 0.0f;
	fWaterHeightBase = 0.0f;
	fWaveHeight = 0.0f;
	iMaterial = -1;
	iObjectCount = 0;
	iVersion = 1;
	iWaveInterval = 1000;
	sprintf(sAlphaMap, "");
	sprintf(sBaseTexture, "");
	sprintf(sFloorMap, "");
	sprintf(sRoofMap, "");
	sprintf(sSecondTexture, "");
	vGravity = CVector3(0.0f, -9.81f, 0.0f);
	vSinglePlayerStartAngle.x = 0.0f;
	vSinglePlayerStartAngle.y = 0.0f;
	vSinglePlayerStartAngle.z = 0.0f;
	vSinglePlayerStartPoint.x = 0.0f;
	vSinglePlayerStartPoint.y = 0.0f;
	vSinglePlayerStartPoint.z = 0.0f;
	vWaterColor = CVector3(1.0f, 1.0f, 1.0f);
	vWind = CVector3(0.0f, 0.0f, 0.0f);
}

void CMap::AddObject(int objtype, char* objname, float x, float y, float z, float scale)
{
	if ( strlen(objname) == 0 )
		return;

	Object[iObjectCount].iObjectType = objtype;
	sprintf_s(Object[iObjectCount].Objectname, "%s", objname);
	sprintf_s(Object[iObjectCount].Texturename, "%s", objname);

	Object[iObjectCount].fScale = scale;
	Object[iObjectCount].x = x;
	Object[iObjectCount].y = y;
	Object[iObjectCount].z = z;

	iObjectCount++;
}

void CMap::ClearObjectData()
{
	if(iObjectCount < 1)
		return;

	for(int i = 0; i < iObjectCount; i++)
	{
		Object[i] = TObjectData();
	}

	iObjectCount = 0;
}

void CMap::CreateFolders(char Filename[255])
{
	char FName[255] = "";
	sprintf(FName, "%sMaps\\%s", g_sGameName, Filename);
	mkdir(FName);

	sprintf(Filename, "%s\\Audio", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Materials", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Models", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Objects", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Particles", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Scripts", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Static", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Terrain", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Textures", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Videos", FName);
	mkdir(Filename);
	sprintf(Filename, "%s\\Weapons", FName);
	mkdir(Filename);
}

bool CMap::DeleteIndex(long id)
{
	if(id < 0 || id >= iObjectCount)
		return false;

	if(id == iObjectCount - 1)
	{
		sprintf(Object[id].Objectname, "");
		iObjectCount--;
	}
	else
	{
		for(int i = id; i < iObjectCount - 2; i += 1)
		{
			Object[i] = Object[i + 1];
		}

		sprintf(Object[iObjectCount - 1].Objectname, "");
		iObjectCount--;
	}

	return true;
}

bool CMap::Load(char Filename[255])
{
	if(strlen(Filename) == 0)
		return false;

	char oldheightmap[255] = "";

	g_bLoading = true;
	ShowCursor(FALSE);

	if(strlen(Game.sMapName) > 0)
	{
		RenderStatusMessage("Unloading Data...");

		sprintf(oldheightmap, "%s", sFloorMap);

		New();
	}


	Filename = RemoveFileExt(Filename);
	sprintf(Game.sMapName, "%s", Filename);
	sprintf(sMapName, "%s", Filename);
	

	char buffer[255] = "";
	char FName[255] = "";
	sprintf(FName, "%sMaps\\%s\\Map.data", g_sGameName, Filename);

	FILE *pFile = NULL;
	fopen_s(&pFile, FName, "rb");

	if(!pFile)																	// Make sure we have a valid file pointer (we found the file)
	{
		sprintf(FName, "%sMaps\\%s\\Map.map", g_sGameName, Filename);
		fopen_s(&pFile, FName, "rb");

		if(!pFile)																// Display an error message and don't load anything if no file was found
		{
			sprintf(buffer, "ERROR[Map::Load]: '%s' not found!\n", FName);
			Console->Output(buffer);

			g_bLoading = false;
			return false;
		}
	}

	RenderStatusMessage("Loading Map Data...");

	fread(&m_Header, 1, sizeof(TMapHeader), pFile);								// Read the header data and store it in our m_Header member variable

	switch(m_Header.version)
	{
	case MAP_VERSION_1:
		fread(&sFloorMap, sizeof(char [80]), 1, pFile);
		fread(&fWaterHeightBase, sizeof(float), 1, pFile);
		fread(&fFogDensity, sizeof(float), 1, pFile);
		fread(&iObjectCount, sizeof(int), 1, pFile);
		fread(&Object, sizeof(TObjectData), iObjectCount, pFile);
		break;
	case MAP_VERSION_2:
		fread(&fFogDensity, sizeof(float), 1, pFile);
		fread(&fWaterHeightBase, sizeof(float), 1, pFile);
		fread(&iObjectCount, sizeof(int), 1, pFile);
		fread(&sFloorMap, sizeof(char [80]), 1, pFile);
		fread(&vSinglePlayerStartAngle.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.z, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.z, sizeof(float), 1, pFile);
		fread(&Object, sizeof(TObjectData), iObjectCount, pFile);

		g_Camera.SetView(vSinglePlayerStartAngle);
		g_Camera.SetPosition(vSinglePlayerStartPoint);
		break;
	case MAP_VERSION_3:
		fread(&fFogDensity, sizeof(float), 1, pFile);
		fread(&fWaterHeightBase, sizeof(float), 1, pFile);
		fread(&iObjectCount, sizeof(int), 1, pFile);
		fread(&sFloorMap, sizeof(char [255]), 1, pFile);
		//fread(&sRoofMap, sizeof(char [255]), 1, pFile);
		fread(&vSinglePlayerStartAngle.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.z, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.z, sizeof(float), 1, pFile);
		fread(&Object, sizeof(TObjectData), iObjectCount, pFile);

		g_Camera.SetView(vSinglePlayerStartAngle);
		g_Camera.SetPosition(vSinglePlayerStartPoint);
		break;
	case MAP_VERSION_ACTUAL:
		fread(&fFogDensity, sizeof(float), 1, pFile);
		fread(&fWaterHeightBase, sizeof(float), 1, pFile);
		fread(&iObjectCount, sizeof(int), 1, pFile);
		fread(&sFloorMap, sizeof(char [255]), 1, pFile);
		//fread(&sRoofMap, sizeof(char [255]), 1, pFile);
		fread(&vSinglePlayerStartAngle.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartAngle.z, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.x, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.y, sizeof(float), 1, pFile);
		fread(&vSinglePlayerStartPoint.z, sizeof(float), 1, pFile);
		fread(&Object, sizeof(TObjectData), iObjectCount, pFile);

		g_Camera.SetView(vSinglePlayerStartAngle);
		g_Camera.SetPosition(vSinglePlayerStartPoint);
		break;
	default:
		Console->Output("Map format unknown!");
		return false;
		break;
	}

	fclose(pFile);

	glFogf(GL_FOG_DENSITY, Map.fFogDensity);
	fWaterHeight = fWaterHeightBase;

	Game.Map = Map;


	RenderStatusMessage("Executing Startup Scripts...");

	g_Manager_Script.Execute("Map.autoexec", false, true);


	RenderStatusMessage("Loading Floor...");
	Engine->bDrawFloor = false;
	if(strlen(sFloorMap) > 0)
	{
		Engine->bDrawFloor = true;
		QuadTree.LoadFloor(sFloorMap);
	}

	Water.SetBoundingBox();

	RenderStatusMessage("Loading Roof...");

	Engine->bDrawRoof = false;
	if(strlen(sRoofMap) > 0)
	{
		QuadTree.LoadRoof(sRoofMap);
		Engine->bDrawRoof = true;
	}


	RenderStatusMessage("Loading Skybox...");

	LoadSkybox();
	PreProcessSkyBox(
		-2048.0f, 
		-2048.0f, 
		-2048.0f, 
		4096.0f + QuadTree.fTerrainXEnd, 
		4096.0f, 
		4096.0f + QuadTree.fTerrainZEnd
		);


	RenderStatusMessage("Loading Materials...");

	sprintf_s(buffer, "Terrain.material");
	iMaterial = Material_Add(buffer);


	RenderStatusMessage("Loading Objects...");

	int id = -1;

	for(int i = 0; i < iObjectCount; i += 1)
	{
		if(iObjectCount >= MAP_MAXOBJECTS)
			break;
		if(strlen(Object[i].Objectname) == 0)
			continue;

		switch(Object[i].iObjectType)
		{
		case EditorSpace::max3ds:
			id = g_Manager_3DSObject.Add(Object[i].Objectname);

			if ( id == -1) {
				break;
			}

			g_Manager_3DSObject.Mesh[id]->Spawn(Object[i].Objectname, Object[i].x, Object[i].y, Object[i].z, Object[i].fScale);

			g_Manager_Physic.Add(g_Manager_3DSObject.Mesh[id]);
			Scene.Add(g_Manager_3DSObject.Mesh[id]);
			break;
		case EditorSpace::md2:
			id = g_Manager_Entity.Add(Object[i].Objectname, Object[i].Texturename);
			g_Manager_Entity.Object[id].Spawn(Object[i].Objectname, Object[i].x, Object[i].y, Object[i].z, Object[i].fScale);

			g_Manager_Physic.Add(&g_Manager_Entity.Object[id]);
			Scene.Add(&g_Manager_Entity.Object[id]);
			break;
		case EditorSpace::Static:
			id = g_Manager_StaticObject.Add(Object[i].Objectname);
			g_Manager_StaticObject.Object[id].Spawn(Object[i].Objectname, Object[i].x, Object[i].y, Object[i].z, Object[i].fScale);

			Scene.Add(&g_Manager_StaticObject.Object[id]);
			break;
		case EditorSpace::Bot:
			g_Manager_Bot.Add(Object[i].Objectname);
			g_pActiveBot->Player->Spawn(Object[i].Objectname, true, Object[i].x, Object[i].y, Object[i].z, Object[i].fScale);

			Scene.Add(g_pActiveBot->Player);
			break;
		case EditorSpace::Particle:
			id = g_Manager_ParticleSystem.Add(Object[i].Objectname);
			g_Manager_ParticleSystem.Object[id].SetPosition(CVector3(Object[i].x, Object[i].y, Object[i].z));
			
			Scene.Add(&g_Manager_ParticleSystem.Object[id]);
			break;
		case EDITMODE_SCENE_NODE:
			Scene.ObjectNodeAttach((int)Object[i].y, (int)Object[i].x);											// Add scene nodes
			break;
		}

		Scene.ObjectSetRotation(CVector3(Object[i].fAngleX, Object[i].fAngleY, Object[i].fAngleZ));				// Rotate object
	}

	Scene.iActive = -1;				// Reset object selection
	

	PreLoadMaterials();
	PreLoadModels();
	PreLoadObjects();
	PreLoadParticleSystems();
	PreLoadStaticObjects();
	//PreLoadTextures();

	RenderStatusMessage("Inserting Player(s)...");

	g_iLocalPlayer = g_Manager_Player.Add(Game.sPlayerName);
	g_Manager_Player.Player[g_iLocalPlayer].Weapon.iModel = -1;
	g_Manager_Player.Player[g_iLocalPlayer].Spawn(
													Game.sPlayerModel,
													false,
													Map.vSinglePlayerStartPoint.x,
													Map.vSinglePlayerStartPoint.y,
													Map.vSinglePlayerStartPoint.z
												);
	g_Manager_Player.Player[g_iLocalPlayer].SetView(CVector3(Map.vSinglePlayerStartAngle.x, Map.vSinglePlayerStartAngle.y, Map.vSinglePlayerStartAngle.z));
	g_Manager_Player.Player[g_iLocalPlayer].iTeam = 0;
	Game.mPlayer = &g_Manager_Player.Player[g_iLocalPlayer];

	g_Camera.PositionCamera(
							Map.vSinglePlayerStartPoint.x,
							Map.vSinglePlayerStartPoint.y,
							Map.vSinglePlayerStartPoint.z,
							Map.vSinglePlayerStartAngle.x,
							Map.vSinglePlayerStartAngle.y,
							Map.vSinglePlayerStartAngle.z,
							0,
							1,
							0
							);

	bFirstFrame = true;
	Console->setVisible(false);
	g_bLoading = false;

	return true;
}

void CMap::LoadSkybox()
{
	char buffer[200] = "";

	sprintf(buffer, "Skybox\\Back.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Back.bmp");		// Load the Sky box Back texture

	sprintf(buffer, "Skybox\\Bottom.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Bottom.bmp");		// Load the Sky box Front texture

	sprintf(buffer, "Skybox\\Front.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Front.bmp");		// Load the Sky box Bottom texture

	sprintf(buffer, "Skybox\\Left.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Left.bmp");		// Load the Sky box Top texture

	sprintf(buffer, "Skybox\\Right.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Right.bmp");		// Load the Sky box Left texture

	sprintf(buffer, "Skybox\\Top.bmp");
	if(g_Manager_Texture->CreateTexture(buffer) == (GLuint)-1)
		g_Manager_Texture->CreateTexture("Skybox\\Top.bmp");			// Load the Sky box Right texture
}

void CMap::New()
{
	bool bWasLoading = g_bLoading;

	g_bLoading = true;
	Editor->DeInit();
	
	RenderStatusMessage("Unloading Data...");

	UnLoad();

	//if ( Editor ) {
	//	if(Game.bAllowEditor && !Editor->bInitialisized)
	//	{
	//		RenderStatusMessage("Starting Editor...");
	//		Editor->Init();

	//		g_bLoading = true;
	//	}
	//}

	Engine->Reset();

	m_Header.magic = 1;
	m_Header.version = MAP_VERSION_ACTUAL;

	bFogAffectsSkybox = true;
	bSinglePlayer = true;
	bWaterEnabled = true;
	bWaterReflections = true;
	bWaterRefractions = false;
	fAmbientColor[0] = 1.0f;
	fAmbientColor[1] = 1.0f;
	fAmbientColor[2] = 1.0f;
	fFloorOffset = 0.0f;
	fFogDensity = 0.0f;
	fFogDepth = 0.0f;
	fRoofOffset = 0.0f;
	fWaterFlow = 0.0001f;
	fWaterHeight = 0.0f;
	fWaterHeightBase = 0.0f;
	fWaveHeight = 0.0f;
	iMaterial = -1;
	iObjectCount = 0;
	iVersion = 1;
	iWaveInterval = 1000;
	sprintf(sAlphaMap, "");
	sprintf(sBaseTexture, "");
	sprintf(sFloorMap, "");
	sprintf(sRoofMap, "");
	sprintf(sSecondTexture, "");
	vGravity = CVector3(0.0f, -9.81f, 0.0f);
	vSinglePlayerStartAngle.x = 0.0f;
	vSinglePlayerStartAngle.y = 0.0f;
	vSinglePlayerStartAngle.z = 0.0f;
	vSinglePlayerStartPoint.x = 0.0f;
	vSinglePlayerStartPoint.y = 0.0f;
	vSinglePlayerStartPoint.z = 0.0f;
	vWaterColor = CVector3(1.0f, 1.0f, 1.0f);
	vWind = CVector3(0.0f, 0.0f, 0.0f);

	if(!bWasLoading)
		g_bLoading = false;
}

void CMap::PreLoadMaterials()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Materials", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "material");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Material.Add(buffer);
	}
}

void CMap::PreLoadModels()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Models", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "md2");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Entity.Add(buffer, buffer);
	}
}

void CMap::PreLoadObjects()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Objects", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "3ds");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_3DSObject.Add(buffer);
	}
}

void CMap::PreLoadParticleSystems()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Particles", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "particle");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_ParticleSystem.Add(buffer);
	}
}

void CMap::PreLoadStaticObjects()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Static", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "static");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_StaticObject.Add(buffer);
	}
}

void CMap::PreLoadTextures()
{
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Textures", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "bmp");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Texture->CreateBMPTexture(buffer);
	}

	sprintf(buffer, "%sMaps\\%s\\Textures", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "jpg");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Texture->CreateJPGTexture(buffer);
	}

	sprintf(buffer, "%sMaps\\%s\\Textures", g_sGameName, sMapName);
	g_Manager_File.Searchfiles(buffer, "tga");

	for(unsigned int i = 0; i < g_Manager_File.filelist.size(); i += 1)
	{
		basic_string<char> objectname = g_Manager_File.filelist.at(i);
		sprintf(buffer, "%s", objectname.data());
		
		g_Manager_Texture->CreateTGATexture(buffer);
	}
}

void CMap::ProcessWaves()
{
	fWaterHeight += fWaveHeight * sin(float(g_dCurTime / iWaveInterval));
}

bool CMap::Save(char Filename[255])
{
	sprintf(Game.sMapName, "%s", Filename);

	char FName[200];
	sprintf(FName, "%sMaps\\%s\\Map.data", g_sGameName, Filename);

	FILE *pFile = NULL;
	fopen_s(&pFile, FName, "wb");

	if(!pFile) 
	{
		CreateFolders(Filename);
		fopen_s(&pFile, FName, "wb");

		if(!pFile) 
		{
			// Display an error message and don't store anything if no file was found
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Map.Save]: Could not store '%s'!\n", FName);
			Console->Output(buffer);
			return false;
		}
	}

	Scene.SortByMaterial();
	UpdateDataFromScene();
	SortByMaterial();

	m_Header.version = MAP_VERSION_ACTUAL;

	vSinglePlayerStartAngle = g_Camera.View();
	vSinglePlayerStartPoint = g_Camera.Position();

	// Write the header data and store it in our m_Header member variable
	fwrite(&m_Header, sizeof(TMapHeader), 1, pFile);
	fwrite(&fFogDensity, sizeof(float), 1, pFile);
	fwrite(&fWaterHeightBase, sizeof(float), 1, pFile);
	fwrite(&iObjectCount, sizeof(int), 1, pFile);
	fwrite(&sFloorMap, sizeof(char [255]), 1, pFile);
	//fwrite(&sRoofMap, sizeof(char [255]), 1, pFile);
	fwrite(&vSinglePlayerStartAngle.x, sizeof(float), 1, pFile);
	fwrite(&vSinglePlayerStartAngle.y, sizeof(float), 1, pFile);
	fwrite(&vSinglePlayerStartAngle.z, sizeof(float), 1, pFile);
	fwrite(&vSinglePlayerStartPoint.x, sizeof(float), 1, pFile);
	fwrite(&vSinglePlayerStartPoint.y, sizeof(float), 1, pFile);
	fwrite(&vSinglePlayerStartPoint.z, sizeof(float), 1, pFile);
	fwrite(&Object, sizeof(TObjectData), iObjectCount, pFile);

	fclose(pFile);

	return true;
}

void CMap::SetGravity(CVector3 grav)
{
	vGravity = grav;
}

void CMap::SortByMaterial()
{
	if(iObjectCount < 2)
		return;

	int index = -1;
	CMaterial actualmat, oldmat;

	int round = 0;
	int i = 0;

	while(round < iObjectCount)
	{
		switch(Object[0].iObjectType)
		{
		case EditorSpace::max3ds:
			index = g_Manager_3DSObject.IndexOf(Object[0].Objectname);

			actualmat = g_Manager_Material.Materials[g_Manager_3DSObject.Mesh[index]->iMaterial];
			oldmat = actualmat;
			break;
		case EditorSpace::md2:
			index = g_Manager_Entity.IndexOf(Object[0].Objectname);

			actualmat = g_Manager_Material.Materials[g_Manager_Entity.Object[index].iMaterial];
			oldmat = actualmat;
			break;
		case EditorSpace::Static:
			index = g_Manager_StaticObject.IndexOf(Object[0].Objectname);

			actualmat = g_Manager_Material.Materials[g_Manager_StaticObject.Object[index].iMaterial];
			oldmat = actualmat;
			break;
		default:
			oldmat = g_Manager_Material.Materials[0];
			break;
		}

		i = 1;

		while(i < iObjectCount)
		{
			switch(Object[i].iObjectType)
			{
			case EditorSpace::max3ds:
				index = g_Manager_3DSObject.IndexOf(Object[i].Objectname);

				actualmat = g_Manager_Material.Materials[g_Manager_3DSObject.Mesh[index]->iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					Object[iObjectCount] = Object[i - 1];
					Object[i - 1] = Object[i];
					Object[i] = Object[iObjectCount];
					Object[iObjectCount] = TObjectData();
				}

				oldmat = actualmat;
				break;
			case EditorSpace::md2:
				index = g_Manager_Entity.IndexOf(Object[i].Objectname);

				actualmat = g_Manager_Material.Materials[g_Manager_Entity.Object[index].iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					Object[iObjectCount] = Object[i - 1];
					Object[i - 1] = Object[i];
					Object[i] = Object[iObjectCount];
					Object[iObjectCount] = TObjectData();
				}

				oldmat = actualmat;
				break;
			case EditorSpace::Static:
				index = g_Manager_StaticObject.IndexOf(Object[i].Objectname);

				actualmat = g_Manager_Material.Materials[g_Manager_StaticObject.Object[index].iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					Object[iObjectCount] = Object[i - 1];
					Object[i - 1] = Object[i];
					Object[i] = Object[iObjectCount];
					Object[iObjectCount] = TObjectData();
				}

				oldmat = actualmat;
				break;
			}

			i += 1;
		}

		round += 1;
	}
}

void CMap::UnLoad()
{
	ClearObjectData();

	g_Manager_Texture->UnLoadTextures(Texture_IndexOf("Dummy.bmp"));
	g_Manager_3DSObject.Clear();
	g_Manager_Bot.Clear();
	g_Manager_Bullet.Clear();
	g_Manager_Chat.Clear();
	g_Manager_Entity.Clear();
	g_Manager_Material.Clear();
	g_Manager_ParticleSystem.Clear();
	g_Manager_Player.Clear();
	g_Manager_Region.Clear();
	g_Manager_Sound.Clear();
	g_Manager_StaticObject.Clear();
	QuadTree.Clear();
	Scene.Clear();
	
	Material_Add("default.material");
}

void CMap::UpdateDataFromScene()
{
	ClearObjectData();

	// SceneObjects
	for(list<CSceneObject>::iterator so = Scene.SceneObjects.begin(); so != Scene.SceneObjects.end(); so++)
	{
		if ( so->bMarkedForDeletion || !so->go->getName() )
			continue;

		AddObject(so->go->iType, so->go->getName(), so->go->mPosition.x, so->go->mPosition.y, so->go->mPosition.z, so->go->fScale);
	}

	// Scene graph nodes
	for(list<CSceneObject>::iterator so = Scene.SceneObjects.begin(); so != Scene.SceneObjects.end(); so++)
	{
		if ( so->bMarkedForDeletion || !so->go->getName() )
			continue;

		if(so->Nodes.size() > 0)
		{
			for(vector<CSceneID>::iterator i = so->Nodes.begin(); i != so->Nodes.end(); i++)
			{
				char buffer[255] = "";
				sprintf(buffer, "SceneNode_%d_%d", so->SID.getId(), i->getId());

				AddObject(EDITMODE_SCENE_NODE, buffer, (float)so->SID.getId(), (float)i->getId(), 0.0f, 0.0f);
			}
		}
	}
}