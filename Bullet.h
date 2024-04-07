
#pragma once

// Library includes

// Project includes
#include "Object.h"
#include "Tools.h"


class CBullet : public CGraphicObject
{
public:
	CBullet();

	int Collision();
	void Move();
	
	float	fSpeed;
	int		iDamage;
	int		iFrom;
	long	lLastTime;
	long	lStartTime;
	CVector3 vDirection;
};

class CBulletManager
{
public:
	CBulletManager();
	~CBulletManager();

	int Add(int type, CVector3 pos, CVector3 dir, int From);
	int Add(CGraphicObject gobj, CVector3 pos, CVector3 dir, int From);
	void Clear();
	int Delete(int index);
	void Manage();

	int Count;
	CBullet Bullet[1000];
};
