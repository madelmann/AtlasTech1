#ifndef _ENTITY_H
#define _ENTITY_H


#include "Main.h"
#include "Md2.h"								// Include the MD2 header file.


#define MAX_MD2OBJECTS 1000


class CEntity : public CGraphicObject
{
public:
	CEntity();
	~CEntity();

	int LowestPoint();
	void Render();
	void RenderWithoutDisplayList();
	float ReturnCurrentTime(CEntity *pEntity, int nextFrame);
	void SetAnimation(int animation);
	void SetBoundingBox();
	void Spawn(char name[255], float x, float y, float z, float scale);

	float			fGamma;
	long			iLowestPoint;
	float			flastAnimTime;
	t3DModel		Model;								// This holds the 3D Model info that we load in
	float			Speed;
};

class CEntityManager
{
public:
	CEntityManager();
	int Add(char Filename[255], char Texname[255]);
	void Clear();
	int IndexOf(char* entityname);
	int IndexOfID(long id);
	int ObjectExists(char* entityname);
	
	int		Count;
	int		iActive;
	CEntity	Object[MAX_MD2OBJECTS];
};

#endif