#include "Main.h"


void CRegion::Execute()
{
	if(bLocked)
		return;

	bActive = true;
	g_Manager_Script.Execute(sScript, true, true);
}

void CRegion::Render()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS);
		glVertex3f(vLeftTop.x, vLeftTop.y, vLeftTop.z);
		glVertex3f(vLeftTop.x, vRightBottom.y, vLeftTop.z);
		glVertex3f(vRightBottom.x, vRightBottom.y, vLeftTop.z);
		glVertex3f(vRightBottom.x, vLeftTop.y, vLeftTop.z);

		glVertex3f(vLeftTop.x, vLeftTop.y, vLeftTop.z);
		glVertex3f(vLeftTop.x, vRightBottom.y, vLeftTop.z);
		glVertex3f(vLeftTop.x, vRightBottom.y, vRightBottom.z);
		glVertex3f(vLeftTop.x, vLeftTop.y, vRightBottom.z);

		glVertex3f(vRightBottom.x, vLeftTop.y, vLeftTop.z);
		glVertex3f(vRightBottom.x, vRightBottom.y, vLeftTop.z);
		glVertex3f(vRightBottom.x, vRightBottom.y, vRightBottom.z);
		glVertex3f(vRightBottom.x, vLeftTop.y, vRightBottom.z);

		glVertex3f(vLeftTop.x, vLeftTop.y, vRightBottom.z);
		glVertex3f(vLeftTop.x, vRightBottom.y, vRightBottom.z);
		glVertex3f(vRightBottom.x, vRightBottom.y, vRightBottom.z);
		glVertex3f(vRightBottom.x, vLeftTop.y, vRightBottom.z);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int CRegionManager::Add(bool localplayer, float x1, float y1, float z1, float x2, float y2, float z2, char file[255])
{
	if(Count >= MAX_REGIONS)
		return -1;

	Region[Count].bLocalPlayerOnly = localplayer;
	Region[Count].vLeftTop.x = x1;
	Region[Count].vLeftTop.y = y1;
	Region[Count].vLeftTop.z = z1;
	Region[Count].vRightBottom.x = x2;
	Region[Count].vRightBottom.y = y2;
	Region[Count].vRightBottom.z = z2;
	sprintf(Region[Count].sScript, "%s", file);

	iActive = Count;
	Count++;

	return Count - 1;
}

void CRegionManager::Check(CVector3 pos)
{
	if(Count <= 0)
		return;

	for(int i = 0; i < Count; i += 1)
	{
		if(
			pos.x > Region[i].vLeftTop.x && pos.x < Region[i].vRightBottom.x
		//&&	pos.y > Region[i].vLeftTop.y && pos.y < Region[i].vRightBottom.y
		&&	pos.z > Region[i].vLeftTop.z && pos.z < Region[i].vRightBottom.z
		)
		{
			if(!Region[i].bLocked && !Region[i].bActive)
			{
				iActive = i;
				Region[i].Execute();
			}
		}
		else
			Region[i].bActive = false;
	}
}

void CRegionManager::Clear()
{
	Count = 0;
	iActive = -1;

	for(int i = 0; i < MAX_REGIONS; i += 1)
	{
		Region[i].bActive = false;
		Region[i].bLocked = false;
		sprintf(Region[i].sScript, "");
		Region[i].vLeftTop.x = 0.0f;
		Region[i].vLeftTop.y = 0.0f;
		Region[i].vLeftTop.z = 0.0f;
		Region[i].vRightBottom.x = 0.0f;
		Region[i].vRightBottom.y = 0.0f;
		Region[i].vRightBottom.z = 0.0f;
	}
}

void CRegionManager::LockActive()
{
	Region[iActive].bLocked = true;
}