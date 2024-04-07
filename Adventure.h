
#pragma once

// Library includes

// Project includes


class CAdventure
{
public:
	CAdventure();
	~CAdventure();

	void Load(char Filename[255]);

	int  iMaps;
	char sAdventure[255];
	char sTitle[255];
	char sWorldMap[255];
};
