
// Library includes

// Project includes
#include "Bullet.h"
#include "Globals.h"
#include "QuadTree.h"
#include "Player.h"


///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns time t for the interpolation between the current and next key frame
/////
///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float ReturnBulletTime(CBullet *bullet)
{
	static float elapsedTime   = 0.0f;

	// This function is very similar to finding the frames per second.
	// Instead of checking when we reach a second, we check if we reach
	// 1 second / our animation speed. (1000 ms / pEntity->Speed).
	// That's how we know when we need to switch to the next key frame.
	// In the process, we get the t value for how we are at to going to the
	// next animation key frame.  We use time to do the interpolation, that way
	// it runs the same speed on any persons computer, regardless of their specs.
	// It might look chopier on a junky computer, but the key frames still be
	// changing the same time as the other persons, it will just be not as smooth
	// of a transition between each frame.  The more frames per second we get, the
	// smoother the animation will be.

	// Get the current time in milliseconds
	float time = (float)GetTickCount();

	// Find the time that has elapsed since the last time that was stored
	elapsedTime = time - bullet->lLastTime;

	// To find the current t we divide the elapsed time by the ratio of 1 second / our anim speed.
	// Since we aren't using 1 second as our t = 1, we need to divide the speed by 1000
	// milliseconds to get our new ratio, which is a 5th of a second.
	float t = elapsedTime / (1000.0f / bullet->fSpeed);
	
	// If our elapsed time goes over a 5th of a second, we start over and go to the next key frame
	if (elapsedTime >= (1000.0f / bullet->fSpeed))
	{
		// Set our last time to the current time just like we would when getting our FPS.
		bullet->lLastTime = (long)time;
	}

	// Return the time t so we can plug this into our interpolation.
	return t;
}


CBullet::CBullet()
{
	fSpeed = 50.0f;
	iDamage = 0;
	iFrom = -1;
	iModel = -1;
	lLastTime = 0;
	lStartTime = 0;
	vDirection = CVector3(0.0f, 0.0f, 0.0f);
}

int CBullet::Collision()
{
	int result = -2;

	//int box = Scene.CollisionWithBox((CGraphicObject*)this);
	//if(box)
	//	return g_Manager_Script.Execute(Scene.SceneObjects[box].go->sTriggerScript, true, true);

	CVector3 p = mPosition;
	CVector3 s = Normalize(vDirection);

	for(int i = 0; i < g_Manager_Player->Count; i += 1)
	{
		if(i != iFrom)
		{
			CVector3 m = g_Manager_Player->Player[i].mPosition + CVector3(g_Manager_Player->Player[i].BoundingVolume.CX, g_Manager_Player->Player[i].BoundingVolume.CY, g_Manager_Player->Player[i].BoundingVolume.CZ);
			float r = g_Manager_Player->Player[i].fRadius;
			
			float b = (2 / (s.x * s.x + s.y * s.y + s.z * s.z)) * (s.x * (p.x - m.x) + s.y * (p.y - m.y) + s.z * (p.z - m.z));
			float c = (p.x * p.x + p.y * p.y + p.z * p.z + m.x * m.x + m.y * m.y + m.z * m.z - 2 * (p.x * m.x + p.y * m.y + p.z * m.z) - r * r) / (s.x * s.x + s.y * s.y + s.z * s.z);

			if((b / 2) * (b / 2) > c)
				result = i;
		}
	}

	return result;
}

void CBullet::Move()
{
	float time = ReturnBulletTime(this);

	mPositionOld = mPosition;

	CVector3 vVector = vDirection - mPosition;
	vVector = Normalize(vVector);

	mPosition.x += vVector.x * fSpeed * time;			// Add our acceleration to our position's X
	mPosition.y += vVector.y * fSpeed * time;			// Add our acceleration to our position's Y
	mPosition.z += vVector.z * fSpeed * time;			// Add our acceleration to our position's Z
	vDirection.x += vVector.x * fSpeed * time;			// Add our acceleration to our view's X
	vDirection.y += vVector.y * fSpeed * time;			// Add our acceleration to our view's Y
	vDirection.z += vVector.z * fSpeed * time;			// Add our acceleration to our view's Z

	//vPosition.x += vDirection.x * fSpeed * time;
	//vPosition.y += vDirection.y * fSpeed * time;
	//vPosition.z += vDirection.z * fSpeed * time;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CBulletManager::CBulletManager()
{
	Count = 0;
}

CBulletManager::~CBulletManager()
{
	for(int i = 0; i < Count; i += 1)
		Bullet[i].~CBullet();

	Count = 0;
}

int CBulletManager::Add(int type, CVector3 pos, CVector3 dir, int From)
{
	Bullet[Count] = CBullet::CBullet();

	Bullet[Count].bDoCollision = false;
	Bullet[Count].bSpawned = true;
	Bullet[Count].iFrom = From;
	Bullet[Count].iModel = type;
	Bullet[Count].lStartTime = (long)GetTickCount();
	Bullet[Count].lLastTime = Bullet[Count].lStartTime;
	Bullet[Count].vDirection = dir;
	Bullet[Count].mPosition = pos;
	//Bullet[Count].vPosition.y += g_fCameraOverFloor;

	Count++;

	return Count - 1;
}

int CBulletManager::Add(CGraphicObject gobj, CVector3 pos, CVector3 dir, int From)
{
	(CGraphicObject)Bullet[Count] = gobj;
	Bullet[Count] = CBullet::CBullet();

	Bullet[Count].bDoCollision = false;
	Bullet[Count].bSpawned = true;
	Bullet[Count].iFrom = From;
	Bullet[Count].lStartTime = (long)GetTickCount();
	Bullet[Count].lLastTime = Bullet[Count].lStartTime;
	Bullet[Count].vDirection = dir;
	Bullet[Count].mPosition = pos;

	Count++;

	return Count - 1;
}

void CBulletManager::Clear()
{
	if(Count <= 0)
		return;

	for(int i = Count - 1; i >= 0; i -= 1)
	{
		Bullet[i].bSpawned = false;
		Count -= 1;
	}
}

int CBulletManager::Delete(int index)
{
	if(index >= Count)
		return false;

	if(index == Count - 1)
	{
		Bullet[index].bSpawned = false;
		Count -= 1;
	}
	else
	{
		for(int i = index; i < Count - 1; i += 1)
		{
			Bullet[i] = Bullet[i + 1];
		}

		Bullet[Count - 1].bSpawned = false;
		Count -= 1;
	}

	return true;
}

void CBulletManager::Manage()
{
	if(Count <= 0)
		return;

	for(int i = Count - 1; i >= 0; i -= 1)
	{
		Bullet[i].Move();

		if(Bullet[i].mPosition.y < -10.0f || Bullet[i].mPosition.y > 2048.0f)
		{
			Delete(i);
		}
		else if((long)GetTickCount() - Bullet[i].lStartTime >= 10000)
		{
			Delete(i);
		}
		else
		{
			int iCollision = Bullet[i].Collision();
			if(iCollision >= 0)
			{
				g_Manager_Player->Hit(iCollision, Bullet[i].iFrom, false);
				Delete(i);
			}
			if(iCollision == -1)
			{
				Delete(i);
			}
		}
	}
}