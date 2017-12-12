#ifndef _SCRIPT_H
#define _SCRIPT_H


#include "Main.h"


class CScriptManager
{
public:
	CScriptManager();
	int Execute(char Filename[255], bool bInScriptFolder, bool bExecute);
	int ExecuteCommand(char Command[255]);
};

#endif