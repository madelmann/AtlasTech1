
#pragma once

// Library includes

// Project includes


class CScriptManager
{
public:
	CScriptManager();

	int Execute(char Filename[255], bool bInScriptFolder, bool bExecute);
	int ExecuteCommand(char Command[255]);
};
