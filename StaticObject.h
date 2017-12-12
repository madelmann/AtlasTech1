#ifndef _STATICOBJECT_H
#define _STATICOBJECT_H


#include "Main.h"


typedef struct tagVERTEX
{
	float x, y, z;
	float u, v;
} VERTEX;

typedef struct tagTRIANGLE
{
	VERTEX vertex[3];
	CVector3 normal;
	int    texindex;
} TRIANGLE;

typedef struct tagSECTOR
{
	int numtriangles;
	TRIANGLE* triangle;
} SECTOR;


class CStaticObject : public CGraphicObject
{
private:
		int LowestPoint();
		void SetSelectionIndicator();
public:
		CStaticObject();
		~CStaticObject();
		void CalcVertexNormals();
		int DeepCollision(CVector3 vColPos, float radius);
		void PreProcess();
		void Render();
		void RenderWithoutDisplayList();
		void SetBoundingBox();
		void Spawn(char name[255], float x, float y, float z, float scale);

		long			iLowestPoint;
		SECTOR			Model;
};

class CStaticObjectManager
{
public:
		CStaticObjectManager();
		~CStaticObjectManager();
		int Add(char Filename[255]);
		void Clear();
		int IndexOf(char* objectname);
		int IndexOfID(long id);
		int ObjectExists(char* objectname);
		
		int				Count;
		int				iActive;
		CStaticObject	Object[1000];
};

#endif