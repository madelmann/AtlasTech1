#include "Main.h"


CPhysicManager::CPhysicManager()
{
	Count = 0;
	mGravity = CVector3(0.0f, 0.0f, 0.0f);
	mObjects.clear();
}

CPhysicManager::~CPhysicManager()
{
	Count = 0;
	mObjects.clear();
}

void CPhysicManager::Add(CPhysicObject* pobj)
{
	pobj->SetGravity(mGravity);
	pobj->DoForce(Physic::None);

	mObjects.push_back(*pobj);

	Count = mObjects.size();
}

int CPhysicManager::Delete(CPhysicObject* pobj)
{
	for(vector<CPhysicObject>::iterator it = mObjects.begin(); it < mObjects.end(); it += 1)
	{
		if(&(*it) == pobj)
		{
			// TODO: implement me
		}
	}
}

bool CPhysicManager::IsAtGround(CPhysicObject* pobj)
{
	CVector3 pos = pobj->GetPosition();
	return pos.y <= QuadTree.getHeight(pos.x, pos.z);
}

void CPhysicManager::Manage()
{
	if(Count <= 0)
		return;

	for(vector<CPhysicObject>::iterator it = mObjects.begin(); it < mObjects.end(); it += 1)
	{
		if(!IsAtGround(&(*it)))
		{
			it->DoForce(Physic::Gravity);
		}
		
		it->DoForce(Physic::Push);
	}
}

void CPhysicManager::Reset()
{
	Count = 0;
	mObjects.clear();
}

void CPhysicManager::SetGravity(CVector3 gravity)
{
	mGravity = gravity;
}