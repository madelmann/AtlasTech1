#ifndef _PHYSIC_MANAGER_H
#define _PHYSIC_MANAGER_H


#include "Main.h"


class CPhysicManager
{
public:
	CPhysicManager();
	~CPhysicManager();
	void Add(CPhysicObject* pobj);
	int Delete(CPhysicObject* pobj);
	bool IsAtGround(CPhysicObject* pobj);
	void Manage();
	void Reset();
	void SetGravity(CVector3 gravity);

private:
	int						Count;
	CVector3				mGravity;
	vector<CPhysicObject>	mObjects;
};

#endif