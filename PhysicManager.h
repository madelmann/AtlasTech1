
#pragma once

// Library includes
#include <vector>

// Project includes
#include "Object.h"
#include "Tools.h"


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
	CVector3 				mGravity;
	std::vector<CPhysicObject>	mObjects;
};
