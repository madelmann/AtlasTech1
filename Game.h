
#pragma once

// Library includes

// Project includes
#include "Map.h"
#include "Player.h"


// Game types
#define GAMETYPE_CTF				0
#define GAMETYPE_DEATHMATCH			1
#define GAMETYPE_TEAMDEATHMATCH		2

// Game modes
#define MODE_GAME_NORMAL			0
#define MODE_GAME_COMMAND			1

// Game commands -> corresponds to MODE_GAME_COMMAND
#define MODE_GAME_COMMAND_STAYHERE	0
#define MODE_GAME_COMMAND_SENDBOT	1
#define MODE_GAME_COMMAND_FOLLOW	2


class CGame
{
public:
	CGame();

	void New();
	void Load(char FName[255]);
	void Save(char FName[255]);

	bool	bAllowEditor;
	bool	bFriendlyFire;
	bool	bIsServer;
	bool	bPlayMode;
	bool	bRunning;
	bool	bScriptSequence;
	float	fJumpHeight;
	int		iCrosshair;
	int		iGameMode;
	int		iGameMode_Command;
	int		iGameType;
	int		iVersion;															// The version number of the file
	CMap	Map;
	char	sMapName[255];
	char	sPlayerModel[255];
	char	sPlayerName[255];
	char	sStartMap[255];

	CPlayer	*mPlayer;
};
