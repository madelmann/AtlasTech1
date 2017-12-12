#ifndef _PLAYER_H
#define _PLAYER_H


//#include "Engine.h"
#include "Main.h"
#include "Weapon.h"


#define MAX_WAYPOINTS	8
#define pSpeed	20.0f


class CPlayer : public CGraphicObject
{
public:
	CPlayer();
	~CPlayer();

	void ChangeModel(char* model);
	CVector3 GetView();
	void Move(float speed);
	void SetView(CVector3 view);
	bool Reload();
	void Render();
	void RenderWithoutDisplayList();
	void Shoot(bool Remote);
	void Spawn(char model[255], bool ai, float x, float y, float z);
	void Spawn(char model[255], bool ai, float x, float y, float z, float scale);
	void SpawnRandom();
	void SpawnRandomArea(CVector3 centre, float radius);
	void Strafe(float speed);
	void WeaponSwitch(char* FName);

	SystemAddress	Address;
	bool			bAI;
	bool			bInGame;
	bool			bLocalPlayer;
	bool			bShoot;
	char			cModel[255];
	double			dTimeBeginJump;
	double			dTimeBeginShoot;
	float			fFOV;
	float			fHearingRange;
	float			fSpeed;
	float			fViewDepth;
	float			fWeaponFireAngle;
	int				iAmor;
	int				iDeaths;
	int				iJump;
	int				iKills;
	int				iTeam;
	int				iTileX;
	int				iTileZ;
	int				iWeaponSide;
	CPlayer*		pObject;
	CVector3		vStrafe;
	//CVector3		vUpVector;
	CVector3		vView;
	CWeapon			Weapon;
};


class CPlayerManager
{
public:
	CPlayerManager();

	int Add(char PlayerName[255]);
	void Clear();
	void Delete(int Index);
	int GetPlayerByAddress(SystemAddress address);
	int GetPlayerByID(int id);
	int Hit(int Who, int From, bool Remote);
	void Manage();

	int		Count;
	int		iActive;
	int		iMaxPlayers;
	CPlayer	Player[MAX_CLIENTS];
};

#endif