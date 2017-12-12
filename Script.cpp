#include "Main.h"

CScriptManager::CScriptManager()
{	
}

int CScriptManager::Execute(char Filename[255], bool bInScriptFolder, bool bExecute)
{
	FILE *pFile;
	char FName[255] = "";

	//char* Ext = ExtractFileExt(Filename);
	char Ext[255] = "";
	sprintf(Ext, "%s", ExtractFileExt(Filename));

	if(strcmpi(Ext, "BAS") == 0)
	{
		g_hThread[g_iThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, (LPVOID)Filename, 0, &g_dwThreadId[g_iThreadCount]);
		g_iThreadCount++;

		return 0;
	}
	else
	{
		if(bInScriptFolder)
			sprintf(FName, "%sMaps\\%s\\SCRIPTS\\%s.script", g_sGameName, Game.sMapName, Filename);
		else
			sprintf(FName, "%sMaps\\%s\\%s", g_sGameName, Game.sMapName, Filename);
	}

	fopen_s(&pFile, FName, "rt");
	if(!pFile) 
	{
		if(bInScriptFolder)
			sprintf(FName, "%sScripts\\%s", g_sGameName, Filename);
		else
			sprintf(FName, "%s%s", g_sGameName, Filename);

		fopen_s(&pFile, FName, "rt");
		if(!pFile) 
		{
			// Display an error message and don't load anything if no file was found
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Script.Execute]: '%s' not found!\n", FName);
			Console->Output(buffer);
			return false;
		}
	}

	char oneline[255] = "";

	do
	{
		sprintf(oneline, "");
		readstr(pFile, oneline);

		if(bExecute)
			ProcessCommand(oneline);
		else
			Console->Output(oneline);

	} while(strcmp(oneline, "") != 0);

	fclose(pFile);

	return 0;
}

int CScriptManager::ExecuteCommand(char Command[255])
{
	return ProcessCommand(Command);
}