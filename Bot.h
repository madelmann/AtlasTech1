#ifndef _BOTMANAGER_H
#define _BOTMANAGER_H


#include "Main.h"


#define MAX_BOTS		30
#define MODE_AI_NONE	0
#define MODE_AI_PATROL	1
#define MODE_AI_FOLLOW	2


class CBot
{
public:
	CBot();

	//bool Injured();
	void FollowPlayer();
	void FollowWayPoints();
	bool Hear();				// TODO: build a global sound list where this function "listens" to
	bool InCircle(CVector3 center, float radius);
	bool IsEnemyDead();
	virtual void Manage();
	void Movement();
	bool OutOfAmmo();
	bool Reload();
	bool See();
	void Sense();
	void Shoot();
	void TurnTo(CVector3 pos);
	int WayPointAdd(float wx, float wz);
	void WayPointsClear();

	CPlayer*	Enemy;
	int			iMode;
	int			iWayPointCount;
	int			iWayPointIndex;
	CBot*		pObject;
	CPlayer*	Player;
	CVector3	vViewField;
	CVector3	vWayPoint[MAX_WAYPOINTS];
	vector<int>	lHearingList;
};

class CBotManager
{
public: 
	CBotManager();
	~CBotManager();

	int Add(char* Filename);
	void Clear();
	void Manage();

	int		Count;
	int		iActive;
	CBot	Bot[MAX_BOTS];
};


#endif