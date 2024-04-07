
// Library includes

// Project includes
#include "Main.h"
#include "QuadTree.h"


float RandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}


CPlayer::CPlayer()
{
	bAI	= false;
	bInGame = false;
	bLocalPlayer = false;
	fFOV = 50.0f;
	fHearingRange = 100.0f;
	fSpeed = pSpeed;
	fViewDepth = 500.0f;
	iAmor = 100;
	iDeaths = 0;
	iHealth = 100;
	iJump = 0;
	iKills = 0;
	iTeam = -1;
	iWeaponSide = 1;
}

CPlayer::~CPlayer()
{
}

void CPlayer::ChangeModel(char* model)
{
	sprintf(cModel, "%s", model);

	iModel = g_Manager_Entity.Add(cModel, cModel);
	BoundingVolume = g_Manager_Entity.Object[iModel].BoundingVolume;
	fRadius = g_Manager_Entity.Object[iModel].fRadius;
}

CVector3 CPlayer::GetView()
{
	return vView;
}

void CPlayer::Move(float speed)
{
	// Get the current view vector (the direction we are looking)
	CVector3 vVector = GetView() - GetPosition();
	vVector = Normalize(vVector);

	mPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	//vPosition.y += vVector.y * speed;		// Add our acceleration to our position's Y
	mPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
	vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	//vView.y += vVector.y * speed;			// Add our acceleration to our view's Y
	vView.z += vVector.z * speed;			// Add our acceleration to our view's Z

	mPosition.y = QuadTree.getHeight(mPosition.x, mPosition.z);
}

bool CPlayer::Reload()
{
	Weapon.iBullets = Weapon.iMaxBullets;
	g_Manager_Sound.PlaySoundByID(Weapon.iSound_Reload);

	return true;
}

void CPlayer::Render()
{
	RenderWithoutDisplayList();
}

void CPlayer::RenderWithoutDisplayList()
{
	g_Manager_Entity.Object[iModel].RenderWithoutDisplayList();
}

void CPlayer::SetView(CVector3 view)
{
	vView = view;
}

void CPlayer::Shoot(bool Remote)
{
	if(Weapon.iModel < 0)
		return;

	if(Weapon.iBullets <= 0)
	{
		if(g_Manager_Sound.PlaySoundByID(Weapon.iSound_Empty))
			g_Manager_Sound.ActiveSounds[g_Manager_Sound.ActiveSounds.size() - 1].SetPosition(mPosition);

		return;
	}

	if(dTimeBeginShoot + Weapon.fTimePerShot > g_dCurTime)
		return;

	bShoot = true;

	dTimeBeginShoot = g_dCurTime;

	Weapon.iBullets -= 1;

	fWeaponFireAngle += 37.0f;
	if(fWeaponFireAngle >= 360.0f)
		fWeaponFireAngle -= 360.0f;

	CVector3 vtmp;
	vtmp.x = mPosition.x;
	vtmp.y = mPosition.y + g_fCameraOverFloor + Weapon.vDrawPosition.y;
	vtmp.z = mPosition.z;

	g_Manager_Bullet.Add(Weapon.iModel_Bullet, vtmp, vView, getId());

	if(!Remote)
	{
		char buffer[255] = "";
		sprintf(buffer, "%i|%i", CLIENT_BULLET, g_iLocalPlayer);
		Net_SendMessage(buffer, true);
	}

	if(g_Manager_Sound.PlaySoundByID(Weapon.iSound_Fire))
		g_Manager_Sound.ActiveSounds[g_Manager_Sound.ActiveSounds.size() - 1].SetPosition(mPosition);
}

void CPlayer::Spawn(char model[255], bool ai, float x, float y, float z)
{
	if(strlen(model) == 0)
		sprintf(model, "Male");

	ChangeModel(model);
	bAI = ai;
	bSpawned = true;
	
	y = QuadTree.getHeight(x, z);
	SetPosition(CVector3(x, y, z));
	SetView(CVector3(x, y, 400));

	//WeaponSwitch("92fs.wpn");
}

void CPlayer::Spawn(char model[255], bool ai, float x, float y, float z, float scale)
{
	Spawn(model, ai, x, y, z);

	fScale = scale;
}

void CPlayer::SpawnRandom()
{
	bSpawned = true;

	mPosition.x = RandomMinMax(-2000.0f, 2000.0f);
	mPosition.z = RandomMinMax(-2000.0f, 2000.0f);
	mPosition.y = QuadTree.getHeight(mPosition.x, mPosition.z);

	SetPosition(mPosition);
}

void CPlayer::SpawnRandomArea(CVector3 centre, float radius)
{
	mPosition.x = RandomMinMax(-radius, radius) + centre.x;
	mPosition.z = RandomMinMax(-radius, radius) + centre.z;
	mPosition.y = QuadTree.getHeight(mPosition.x, mPosition.z);

	SetPosition(mPosition);
}

void CPlayer::Strafe(float speed)
{	
	// Add the strafe vector to our position
	mPosition.x += vStrafe.x * speed;
	mPosition.z += vStrafe.z * speed;

	// Add the strafe vector to our view
	vView.x += vStrafe.x * speed;
	vView.z += vStrafe.z * speed;
}

void CPlayer::WeaponSwitch(char* FName)
{
	if(strlen(FName) <= 0)
		return;

	Weapon.Load(FName);

	char buffer[255] = "";
	sprintf(buffer, "%i|%i|%s\0", CLIENT_CHANGE_WEAPON, g_iLocalPlayer, FName);
	Net_SendMessage(buffer, true);
}


//////////////////////////////////////////////////////////////////////////////////////////////


CPlayerManager::CPlayerManager()
{
	Count = 0;
	iActive = -1;
	iMaxPlayers = MAX_CLIENTS;
}


int CPlayerManager::Add(char PlayerName[255])
{
	if(Count >= iMaxPlayers)
		return -1;

	if(strlen(PlayerName) == 0)
	{
		sprintf(PlayerName, "AIPlayer%i", Count);
	}

	iActive = Count;
	Player[Count].bInGame = true;
	Player[Count].setId(iActive);
	Player[Count].setName(PlayerName);

	Count++;

	return iActive;
}

void CPlayerManager::Clear()
{
	for(int i = 0; i < Count; i += 1)
		Player[i].~CPlayer();

	Count = 0;
	iActive = -1;
}

void CPlayerManager::Delete(int Index)
{
	if(Index < 0 || Index >= Count)
		return;

	if(Index == Count - 1)
	{
		Player[Index].~CPlayer();
		Count--;

		return;
	}

	for(int i = Index; i < Count - 1; i += 1)
	{
		Player[i] = Player[i + 1];
	}

	Player[Count - 1].~CPlayer();	
	Count--;
}

int CPlayerManager::GetPlayerByAddress(SystemAddress address)
{
	for(int i = 0; i < Count; i += 1)
	{
		if(Player[i].Address == address)
			return i;
	}

	return -1;
}

int CPlayerManager::GetPlayerByID(int id)
{
	for(int i = 0; i < Count; i += 1)
	{
		if(Player[i].getId() == id)
			return i;
	}

	return -1;
}

int CPlayerManager::Hit(int Who, int From, bool Remote)
{
	if(!Game.bFriendlyFire)
	{
		bool bHit = false;

		switch(Game.iGameType)
		{
		case GAMETYPE_CTF:
			bHit = true;
			break;
		case GAMETYPE_DEATHMATCH:
			bHit = false;
			break;
		case GAMETYPE_TEAMDEATHMATCH:
			bHit = true;
			break;
		default:
			bHit = false;
			break;
		}

		if(bHit && Player[Who].iTeam == Player[From].iTeam)
			return 0;
	}

	Player[Who].iHealth -= Player[From].Weapon.iDamagePerShot;

	if(!Remote)
	{
		char buffer[255] = "";
		sprintf(buffer, "%i|%i|%i", CLIENT_GET_HIT, Who, From);
		Net_SendMessage(buffer, true);
	}
	
	if(Player[Who].iHealth <= 0)
	{
		Player[Who].iDeaths++;
		Player[Who].iHealth = 100;
		Player[From].iKills++;

		//Player[Who].Spawn(Player[Who].cModel, false, 0.0f, 0.0f, 0.0f);
		Player[Who].SpawnRandom();
		return -1;
	}

	return 1;
}

void CPlayerManager::Manage()
{
	if(Count <= 0)
		return;

	/*
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(Player[i].bAI)
			Player[i].AI_Compute();
	}
	*/
}