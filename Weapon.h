
#pragma once

// Library includes

// Project includes
#include "Tools.h"


#define WEAPON_VERSION_1		8
#define WEAPON_VERSION_2		2
#define WEAPON_VERSION_3		3
#define WEAPON_VERSION_ACTUAL	WEAPON_VERSION_3

class CWeapon
{
public:
	CWeapon();
	~CWeapon();
	int Load(char Filename[255]);
	void New();

	char		cBulletName[255];
	char		cModelName[255];
	char		cMuzzleName[255];
	char		cName[255];
	float		fAngle;
	float		fRange;
	float		fTimePerShot;
	int			iBullets;
	int			iDamagePerShot;
	int			iMaxBullets;
	int			iModel;
	int			iModel_Bullet;
	int			iModel_Muzzle;
	int			iSound_Empty;
	int			iSound_Fire;
	int			iSound_Reload;
	int			iTexture_Bullet;
	int			iTexture_Muzzle;
	int			iVersion;
	CVector3	vDrawPosition;
};
