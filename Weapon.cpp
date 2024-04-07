
// Library includes

// Project includes
#include "Main.h"
#include "Weapon.h"


CWeapon::CWeapon()
{
	iVersion = WEAPON_VERSION_ACTUAL;
	sprintf(cBulletName, "");
	sprintf(cModelName, "");
	sprintf(cMuzzleName, "");
	sprintf(cName, "");
	fAngle = 0.0f;
	fRange = 0.0f;
	fTimePerShot = 1.0f;
	iBullets = 0;
	iDamagePerShot = 0;
	iMaxBullets = 0;
	iModel = -1;
	iModel_Bullet = -1;
	iModel_Muzzle = -1;
	iSound_Empty = -1;
	iSound_Fire = -1;
	iSound_Reload = -1;
	iTexture_Muzzle = -1;
	vDrawPosition.x = 0.0f;
	vDrawPosition.y = 0.0f;
	vDrawPosition.z = 0.0f;
}

CWeapon::~CWeapon()
{
}

//int CWeapon::Load(char Filename[255])
//{
//	FILE *pFile;
//	char fname[255] = "";
//
//	New();
//
//	Filename = RemoveFileExt(Filename);
//
//	sprintf(fname, "%sMaps\\%s\\Weapons\\%s\\weapon.wpn", g_sGameName, Game.sMapName, Filename);
//	
//	if(fopen_s(&pFile, fname, "rb") != NULL)
//	{
//		sprintf_s(fname, "%sWeapons\\%s\\weapon.wpn", g_sGameName, Filename);
//
//		if(fopen_s(&pFile, fname, "rb") != NULL)
//		{
//			char buffer[255] = "";
//			sprintf(buffer, "ERROR[Weapon.Load]: File %s not found!\n", fname);
//			Console->Output(buffer);
//
//			return -1;
//		}
//	}
//
//	fread(&iVersion, sizeof(int), 1, pFile);
//
//	switch(iVersion)
//	{
//	case WEAPON_VERSION_1:
//		fread(&fAngle, sizeof(float), 1, pFile);
//		fread(&iBullets, sizeof(int), 1, pFile);
//		fread(&iDamagePerShot, sizeof(int), 1, pFile);
//		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
//		fread(&iMaxBullets, sizeof(int), 1, pFile);
//		fread(&cModelName, sizeof(char [80]), 1, pFile);
//		fread(&cName, sizeof(char [80]), 1, pFile);
//		fread(&fTimePerShot, sizeof(float), 1, pFile);
//		fread(&cBulletName, sizeof(char [80]), 1, pFile);
//		fread(&cMuzzleName, sizeof(char [80]), 1, pFile);
//		break;
//	case WEAPON_VERSION_2:
//		fread(&fAngle, sizeof(float), 1, pFile);
//		fread(&iBullets, sizeof(int), 1, pFile);
//		fread(&iDamagePerShot, sizeof(int), 1, pFile);
//		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
//		fread(&iMaxBullets, sizeof(int), 1, pFile);
//		fread(&cModelName, sizeof(char [255]), 1, pFile);
//		fread(&cName, sizeof(char [255]), 1, pFile);
//		fread(&fTimePerShot, sizeof(float), 1, pFile);
//		fread(&cBulletName, sizeof(char [255]), 1, pFile);
//		fread(&cMuzzleName, sizeof(char [255]), 1, pFile);
//		break;
//	case WEAPON_VERSION_ACTUAL:
//		fread(&fAngle, sizeof(float), 1, pFile);
//		fread(&fRange, sizeof(float), 1, pFile);
//		fread(&iBullets, sizeof(int), 1, pFile);
//		fread(&iDamagePerShot, sizeof(int), 1, pFile);
//		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
//		fread(&iMaxBullets, sizeof(int), 1, pFile);
//		fread(&cModelName, sizeof(char [255]), 1, pFile);
//		fread(&cName, sizeof(char [255]), 1, pFile);
//		fread(&fTimePerShot, sizeof(float), 1, pFile);
//		fread(&cBulletName, sizeof(char [255]), 1, pFile);
//		fread(&cMuzzleName, sizeof(char [255]), 1, pFile);
//		break;
//	}
//
//	fclose(pFile);
//
//
//	iBullets = iMaxBullets;
//
//	iModel = g_Manager_3DSObject.Add(cModelName);
//
//	char buffer[255] = "";
//
//	sprintf(cBulletName, "%s\\bullet.3ds", RemoveFileExt(cModelName));
//	iModel_Bullet = g_Manager_3DSObject.Add(cBulletName);
//	if(iModel_Bullet == -1)
//	{
//		sprintf(buffer, "%s\\bullet.tga", RemoveFileExt(cModelName));
//		iTexture_Bullet = Texture_Add(buffer);
//	}
//
//	iModel_Muzzle = g_Manager_3DSObject.Add(cMuzzleName);
//	if(iModel_Muzzle == -1)
//	{
//		sprintf(buffer, "%s\\muzzle.tga", RemoveFileExt(cModelName));
//		iTexture_Muzzle = Texture_Add(buffer);
//	}
//
//
//	// Preload audio files - Begin
//	// Empty
//	sprintf(buffer, "Weapons\\%s\\empty.wav", cName);
//	iSound_Fire = g_Manager_Sound.Add(buffer, false);
//	// Fire
//	sprintf(buffer, "Weapons\\%s\\fire.wav", cName);
//	iSound_Fire = g_Manager_Sound.Add(buffer, false);
//	// Reload
//	sprintf(buffer, "Weapons\\%s\\reload.wav", cName);
//	iSound_Reload = g_Manager_Sound.Add(buffer, false);
//	// Preload audio files - End
//
//	return 0;
//}

int CWeapon::Load(char Filename[255])
{
	FILE *pFile;
	char fname[255] = "";

	New();

	Filename = RemoveFileExt(Filename);

	sprintf(fname, "%sMaps\\%s\\Weapons\\%s.wpn", g_sGameName, Game.sMapName, Filename);
	
	if(fopen_s(&pFile, fname, "rb") != NULL)
	{
		sprintf_s(fname, "%sWeapons\\%s.wpn", g_sGameName, Filename);

		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Weapon.Load]: File %s not found!\n", fname);
			Console->Output(buffer);

			return -1;
		}
	}

	fread(&iVersion, sizeof(int), 1, pFile);

	switch(iVersion)
	{
	case WEAPON_VERSION_1:
		fread(&fAngle, sizeof(float), 1, pFile);
		fread(&iBullets, sizeof(int), 1, pFile);
		fread(&iDamagePerShot, sizeof(int), 1, pFile);
		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
		fread(&iMaxBullets, sizeof(int), 1, pFile);
		fread(&cModelName, sizeof(char [80]), 1, pFile);
		fread(&cName, sizeof(char [80]), 1, pFile);
		fread(&fTimePerShot, sizeof(float), 1, pFile);
		fread(&cBulletName, sizeof(char [80]), 1, pFile);
		fread(&cMuzzleName, sizeof(char [80]), 1, pFile);
		break;
	case WEAPON_VERSION_2:
		fread(&fAngle, sizeof(float), 1, pFile);
		fread(&iBullets, sizeof(int), 1, pFile);
		fread(&iDamagePerShot, sizeof(int), 1, pFile);
		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
		fread(&iMaxBullets, sizeof(int), 1, pFile);
		fread(&cModelName, sizeof(char [255]), 1, pFile);
		fread(&cName, sizeof(char [255]), 1, pFile);
		fread(&fTimePerShot, sizeof(float), 1, pFile);
		fread(&cBulletName, sizeof(char [255]), 1, pFile);
		fread(&cMuzzleName, sizeof(char [255]), 1, pFile);
		break;
	case WEAPON_VERSION_ACTUAL:
		fread(&fAngle, sizeof(float), 1, pFile);
		fread(&fRange, sizeof(float), 1, pFile);
		fread(&iBullets, sizeof(int), 1, pFile);
		fread(&iDamagePerShot, sizeof(int), 1, pFile);
		fread(&vDrawPosition, sizeof(CVector3), 1, pFile);
		fread(&iMaxBullets, sizeof(int), 1, pFile);
		fread(&cModelName, sizeof(char [255]), 1, pFile);
		fread(&cName, sizeof(char [255]), 1, pFile);
		fread(&fTimePerShot, sizeof(float), 1, pFile);
		fread(&cBulletName, sizeof(char [255]), 1, pFile);
		fread(&cMuzzleName, sizeof(char [255]), 1, pFile);
		break;
	}

	fclose(pFile);


	iBullets = iMaxBullets;

	iModel = g_Manager_3DSObject.Add(cModelName);

	char buffer[255] = "";

	sprintf(cBulletName, "%s_Bullet.3ds", RemoveFileExt(cModelName));
	iModel_Bullet = g_Manager_3DSObject.Add(cBulletName);
	if(iModel_Bullet == -1)
	{
		sprintf(buffer, "%s_Bullet.tga", RemoveFileExt(cModelName));
		iTexture_Bullet = g_Manager_Texture->CreateTGATexture(buffer);
	}

	iModel_Muzzle = g_Manager_3DSObject.Add(cMuzzleName);
	if(iModel_Muzzle == -1)
	{
		sprintf(buffer, "%s_Muzzle.tga", RemoveFileExt(cModelName));
		iTexture_Muzzle = g_Manager_Texture->CreateTGATexture(buffer);
	}


	// Preload audio files - Begin
	// Empty
	sprintf(buffer, "Weapons\\%s_empty.wav", cName);
	iSound_Fire = g_Manager_Sound.Add(buffer, false);
	// Fire
	sprintf(buffer, "Weapons\\%s_fire.wav", cName);
	iSound_Fire = g_Manager_Sound.Add(buffer, false);
	// Reload
	sprintf(buffer, "Weapons\\%s_reload.wav", cName);
	iSound_Reload = g_Manager_Sound.Add(buffer, false);
	// Preload audio files - End

	return 0;
}

void CWeapon::New()
{
	sprintf(cBulletName, "");
	sprintf(cModelName, "");
	sprintf(cMuzzleName, "");
	sprintf(cName, "");
	fAngle = 0.0f;
	fTimePerShot = 1.0f;
	iBullets = 0;
	iDamagePerShot = 0;
	iMaxBullets = 0;
	iModel = -1;
	iModel_Bullet = -1;
	iModel_Muzzle = -1;
	iSound_Empty = -1;
	iSound_Fire = -1;
	iSound_Reload = -1;
	iTexture_Muzzle = -1;
	vDrawPosition.x = 0.0f;
	vDrawPosition.y = 0.0f;
	vDrawPosition.z = 0.0f;
}