#include "Main.h"


CBot::CBot()
{
	Enemy = NULL;
	iMode = MODE_AI_PATROL;
	iWayPointCount = 0;
	iWayPointIndex = -1;
	lHearingList.clear();
	Player = NULL;
}

void CBot::FollowPlayer()
{
	if(g_Manager_Player.Player[g_iLocalPlayer].iTeam != Player->iTeam)
		return;

	CVector3 vtmppos = g_Manager_Player.Player[g_iLocalPlayer].GetPosition();

	Player->SetView(vtmppos);
	Normalize(Player->vView);

	float speed = Player->fSpeed * g_fFrameInterval;

	Player->Move(speed);
}

void CBot::FollowWayPoints()
{
	if(iWayPointCount <= 0)
		return;

	if(		Player->mPosition.x >= vWayPoint[iWayPointIndex].x - 1.0f	&&	Player->mPosition.x <= vWayPoint[iWayPointIndex].x + 1.0f 
		&&	Player->mPosition.z >= vWayPoint[iWayPointIndex].z - 1.0f	&&	Player->mPosition.z <= vWayPoint[iWayPointIndex].z + 1.0f)
	{
		iWayPointIndex++;
		
		if(iWayPointIndex >= iWayPointCount)
			iWayPointIndex = 0;
	}
	else
	{
		Player->SetView(vWayPoint[iWayPointIndex]);
		Normalize(Player->vView);

		float speed = Player->fSpeed * g_fFrameInterval;

		Player->Move(speed);
	}
}

bool CBot::Hear()
{
	lHearingList.clear();

	for(int i = g_Manager_Sound.ActiveSounds.size() - 1; i >= 0; i -= 1)
	{
		if(InCircle(g_Manager_Sound.ActiveSounds[i].vPosition, Player->fHearingRange))
			lHearingList.push_back(i);
	}

	if(lHearingList.size() <= 0)
		return false;
	else
		return true;
}

bool CBot::InCircle(CVector3 center, float radius)
{
	if(		center.x >= Player->mPosition.x - radius && center.x <= Player->mPosition.x + radius
		&&	center.z >= Player->mPosition.z - radius && center.z <= Player->mPosition.z + radius)
		return true;
	else
		return false;
}

bool CBot::IsEnemyDead()
{
	if(Enemy == NULL)
		return true;

	if(Enemy->iHealth > 0)
		return false;
	else
		return true;
}

void CBot::Manage()
{
	if(!Game.bPlayMode)
		return;

	Movement();

	Sense();

	if(!IsEnemyDead())
		Shoot();
}

void CBot::Movement()
{
	if(!Game.bIsServer)
		return;

	switch(iMode)
	{
	case MODE_AI_NONE:
		break;
	case MODE_AI_PATROL:
		FollowWayPoints();
		break;
	case MODE_AI_FOLLOW:
		FollowPlayer();
		break;
	default:
		FollowWayPoints();
		break;
	}
}

bool CBot::OutOfAmmo()
{
	if(Player->Weapon.iBullets > 0)
		return false;
	else
		return true;
}

bool CBot::Reload()
{
	if(Player->Weapon.iModel != -1)
		return Player->Reload();
	else
		return false;
}

bool CBot::See()
{
	//...

	Enemy = NULL;

	return false;
}

void CBot::Sense()
{
	See();

	//Hear();					// Funktioniert nicht weil Sounds noch nicht gelöscht werden
}

void CBot::Shoot()
{
	Player->Shoot(false);

	if(OutOfAmmo())
		Reload();
}

void CBot::TurnTo(CVector3 pos)
{
	Player->SetView(pos);
}

int CBot::WayPointAdd(float wx, float wz)
{
	if(iWayPointCount >= MAX_WAYPOINTS)
		return -1;

	vWayPoint[iWayPointCount].x = wx;
	vWayPoint[iWayPointCount].z = wz;

	iWayPointCount++;

	if(iWayPointIndex == -1)
		iWayPointIndex = 0;

	return 0;
}

void CBot::WayPointsClear()
{
	iWayPointCount = 0;
	iWayPointIndex = -1;
}


CBotManager::CBotManager()
{
	iActive = -1;
	Count = 0;
}

CBotManager::~CBotManager()
{
	for(int i = Count - 1; i >= 0; i -= 1)
		Bot[i].~CBot();

	iActive = -1;
	Count = 0;
}

int CBotManager::Add(char* Filename)
{
	if(strlen(Filename) == 0)
		return -1;

	Filename = RemoveFileExt(Filename);
	
	Bot[Count].Player = Player_Add(Filename);

	//Bot[Count].Player->ID = Count;
	Bot[Count].Player->iType = EditorSpace::Bot;
	//sprintf(Bot[Count].Player->Name, "%s", Filename);
	Bot[Count].Player->WeaponSwitch("92fs.wpn");

	iActive = Count;
	g_pActiveBot = &Bot[iActive];

	Count++;

	return Count;
}

void CBotManager::Clear()
{
	iActive = -1;
	Count = 0;
}

void CBotManager::Manage()
{
	if(Count <= 0)
		return;

	for(int i = Count - 1; i >= 0; i -= 1)
		Bot[i].Manage();
}