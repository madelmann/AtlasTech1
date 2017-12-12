#ifndef _REGIONS_H
#define _REGIONS_H


#include "Main.h"

#define MAX_REGIONS 32


class CRegion
{
public:
	CRegion()
	{
		bActive = false;
		bLocalPlayerOnly = true;
		bLocked = false;
		sprintf(sScript, "");
		vLeftTop.x = 0.0f;
		vLeftTop.y = 0.0f;
		vLeftTop.z = 0.0f;
		vRightBottom.x = 0.0f;
		vRightBottom.y = 0.0f;
		vRightBottom.z = 0.0f;
	};
	~CRegion()
	{
		bActive = false;
		bLocalPlayerOnly = true;
		bLocked = false;
		sprintf(sScript, "");
		vLeftTop.x = 0.0f;
		vLeftTop.y = 0.0f;
		vLeftTop.z = 0.0f;
		vRightBottom.x = 0.0f;
		vRightBottom.y = 0.0f;
		vRightBottom.z = 0.0f;
	};
	void Execute();
	void Render();

	bool		bActive;
	bool		bLocalPlayerOnly;
	bool		bLocked;
	char		sScript[255];
	CVector3	vLeftTop;
	CVector3	vRightBottom;
};

class CRegionManager
{
public:
	CRegionManager()
	{
		Count = 0;
		iActive = -1;
	};
	~CRegionManager()
	{
		Count = 0;
		iActive = -1;
	};
	int Add(bool localplayer, float x1, float y1, float z1, float x2, float y2, float z2, char file[255]);
	void Check(CVector3 pos);
	void Clear();
	void LockActive();

	int		Count;
	int		iActive;
	CRegion	Region[MAX_REGIONS];
};

#endif