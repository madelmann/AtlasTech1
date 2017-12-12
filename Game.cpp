#include "Main.h"


CGame::CGame()
: mPlayer(0)
{
	bAllowEditor = false;
	bFriendlyFire = false;
	bIsServer = true;
	bPlayMode = true;
	bRunning = false;
	bScriptSequence = false;
	fJumpHeight = g_fCameraOverFloor;
	iCrosshair = 0;
	iGameMode = MODE_GAME_NORMAL;
	iGameMode_Command = -1;
	iGameType = GAMETYPE_DEATHMATCH;
	iVersion = 1;
	sprintf(sMapName, "");
	sprintf(sPlayerModel, "Male");
	sprintf(sPlayerName, "LocalPlayer");
	sprintf(sStartMap, "");
}

void CGame::New()
{
	ShowCursor(FALSE);

	Engine->Keybinding.Load("Keybinding.key");
	//Engine->Keybinding.Save("Keybinding.key");

	CGame();
	CPlayer();
	CWeapon();

	char buffer[255] = "";
	sprintf(buffer, "MAP.LOAD %s", sStartMap);

	ProcessCommand(buffer);

	bRunning = true;
}

void CGame::Load(char FName[255])
{
	New();

	FILE *pFile = NULL;

	char oldmap[255];
	sprintf(oldmap, "%s", sMapName);

	char buffer[255];
	sprintf(buffer, "%sSave\\%s", g_sGameName, FName);

	if(fopen_s(&pFile, buffer, "rb") != NULL)
	{
		return;
	}

	fread(&iVersion, sizeof(int), 1, pFile);

	if(iVersion != 1)
		return;

	fread(&sMapName, sizeof(char[255]), 1, pFile);
	fread(&mPlayer, sizeof(CPlayer), 1, pFile);
	fread(&Map, sizeof(CMap), 1, pFile);

	fclose(pFile);

	Map.Load(sMapName);

	g_Camera.PositionCamera(mPlayer->mPosition.x, mPlayer->mPosition.y, mPlayer->mPosition.z,
							mPlayer->vView.x, mPlayer->vView.y, mPlayer->vView.z,
							0.0f, 1.0f, 0.0f);
							//mPlayer->vUpVector.x, mPlayer->vUpVector.y, mPlayer->vUpVector.z);

	g_Manager_Entity.Count = 0;
	g_Manager_StaticObject.Count = 0;

	for(int i = 0; i < Map.iObjectCount; i += 1)
	{
		switch(Map.Object[i].iObjectType)
		{
		case EditorSpace::max3ds:
			g_Manager_3DSObject.Add(Map.Object[i].Objectname);
			g_Manager_3DSObject.Mesh[g_Manager_3DSObject.Mesh.size()]->Spawn(Map.Object[i].Objectname, Map.Object[i].x, Map.Object[i].y, Map.Object[i].z, 1.0f);
			break;
		case EditorSpace::md2:
			g_Manager_Entity.Add(Map.Object[i].Objectname, Map.Object[i].Texturename);
			g_Manager_Entity.Object[g_Manager_Entity.Count - 1].Spawn(Map.Object[i].Objectname, Map.Object[i].x, Map.Object[i].y, Map.Object[i].z, 1.0f);
			break;
		case EditorSpace::Static:
			g_Manager_StaticObject.Add(Map.Object[i].Objectname);
			g_Manager_StaticObject.Object[g_Manager_StaticObject.Count - 1].Spawn(Map.Object[i].Objectname, Map.Object[i].x, Map.Object[i].y, Map.Object[i].z, 1.0f);
			break;
		case EditorSpace::Bot:
			g_Manager_Bot.Add(Map.Object[i].Objectname);
			g_pActiveBot->Player->Spawn(Map.Object[i].Objectname, true, Map.Object[i].x, Map.Object[i].y, Map.Object[i].z);
			break;
		}
	}
}

void CGame::Save(char FName[255])
{
	FILE *pFile = NULL;

	char buffer[255];

	sprintf(buffer, "%sSave\\%s", g_sGameName, FName);

	if(fopen_s(&pFile, buffer, "wb") != NULL)
	{
		return;
	}

	Map.UpdateDataFromScene();

	fwrite(&iVersion, sizeof(int), 1, pFile);
	fwrite(&sMapName, sizeof(char[255]), 1, pFile);
	fwrite(&mPlayer, sizeof(CPlayer), 1, pFile);
	fwrite(&Map, sizeof(CMap), 1, pFile);

	fclose(pFile);
}