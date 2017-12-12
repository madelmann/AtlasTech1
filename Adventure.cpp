#include "Adventure.h"

#include "Engine.h"


CAdventure::CAdventure()
{
	iMaps = 1;
}

CAdventure::~CAdventure()
{
}

void CAdventure::Load(char Filename[255])
{
	char fname[255] = "";
	sprintf(fname, "%sAdventures\\%s.adventure", g_sGameName, Filename);

	FILE* pFile;

	fopen_s(&pFile, fname, "rt");
	if(!pFile) 
	{
		// Display an error message and don't load anything if no file was found
		char buffer[255] = "";
		sprintf(buffer, "ERROR[LoadAdventure]: '%s' not found!\n", fname);
		Console->Output(buffer);
		return;
	}

	char oneline[255] = "";

	do
	{
		sprintf(oneline, "");
		readstr(pFile, oneline);

		//g_Manager_Octree.LoadOctree(oneline, g_iMapIndex);
		Map.Load(oneline);

		g_iMapIndex++;

	} while(strcmp(oneline, "") != 0);

	fclose(pFile);

	iMaps = g_iMapIndex - 1;
	g_iMapIndex = 0;
}