
#pragma once

// Library includes
#include <algorithm>
#include <list>
#include <vector>

// Project includes
//#include "Object.h"
#include "Tools.h"


class CGraphicObject;


class CSceneID
{
public:
	CSceneID()
	: mId(-1)
	{ }

	int getId()
	{
		return mId;
	}
	void setId(int id)
	{
		mId = id;
	}

	bool operator == (const CSceneID equal) { return (mId == equal.mId); }
	bool operator < (const CSceneID equal) { return (mId < equal.mId); }
	bool operator > (const CSceneID equal) { return (mId > equal.mId); }

//private:
	int mId;
};


class CTimedOperation : public CSceneID
{
public:
	CTimedOperation();

	int		iRepeat;
	char	sCommand[255];
};


class CMotion
{
public:
	CMotion()
	{
		mPosition = CVector3();
		mRotation = CVector3();
	}
	CMotion(CVector3 pos, CVector3 rot)
	{
		mPosition = pos;
		mRotation = rot;
	}
	~CMotion()
	{
	}

	CVector3 mPosition;
	CVector3 mRotation;
};


class CSceneObject
{
public:
	CSceneObject(CGraphicObject* obj);
	~CSceneObject();

	bool CanDraw();
	int  Hover(CVector3 p3d);
	void Move(CVector3 pos);
	bool NodeAttach(int child);
	bool NodeFreeChildren();
	bool NodeRemove(int child);
	void OcclusionQuery();
	int  RemoveParent();
	void Render();
	void Rotate(CVector3 rot);
	void SetParent(int parent);
	void SetPosition(CVector3 pos);
	void SetRotation(CVector3 rot);
	void SetScale(float scale);
	bool operator == (const CSceneObject &other);


	bool				bAllowDraw;
	bool				bLastDrawResult;
	bool				bMarkedForDeletion;
	CSceneID			SID;
	CSceneID			SIDParent;
	GLuint				gluiQuery;
	GLuint				gluiQueryResultAvailible;
	GLuint				gluiSamples;
	CGraphicObject*		go;
	std::list<CMotion*>		Motion;
	std::vector<CSceneID>	Nodes;
	unsigned int		uiOcclusionCalculatedBeforeFPS;

private:
	int					mLastRendered;
};


class CScene
{
public:
	CScene();
	~CScene();

	int  Add(CGraphicObject* obj);
	void BeginCellShading();
	void BeginRenderWater();
	void Clear();
	int  CollisionWithBox(CGraphicObject* obj);
	int  CollisionWithSphere(CGraphicObject* obj);
	int  Delete();
	void EndCellShading();
	void EndRenderWater();
	CSceneObject* GetObjectByID(int id);
	bool GetRenderWater();
	void Manage();
	int  ObjectHover(CVector3 p3d);
	void ObjectIDMove(int id, CVector3 pos);
	void ObjectIDRotate(int id, CVector3 rot);
	void ObjectIDSetPosition(int id, CVector3 pos);
	void ObjectIDSetRotation(int id, CVector3 rot);
	void ObjectMove(CVector3 pos);
	bool ObjectNodeAttach(int child, int parent);
	bool ObjectNodeRemove(int child, int parent);
	void ObjectRotate(CVector3 rot);
	void ObjectScale(float scale);
	int  ObjectSelect(CVector3 p3d);
	void ObjectSelectNext();
	void ObjectSelectPrevious();
	void ObjectSetPosition(CVector3 pos);
	void ObjectSetRotation(CVector3 rot);
	void ObjectSetScale(float scale);
	void Render();
	void Reset();
	void setCameraPosition(const CVector3 &pos);
	void SortByDistance();
	void SortByMaterial();
	void TimedOperation_Add(char* cmd, int repeat);
	void TimedOperation_Manage();

	bool	bCellShading;
	int		Count;
	int		iActive;
	int		iHoverObject;
	char	sHoverMessage[255];

	std::list<CSceneObject> SceneObjects;
	std::list<CTimedOperation> TimedOperations;

private:
	bool		bRenderWater;
	CVector3	mCameraPosition;

	std::list<CSceneObject>::iterator getInsertIndex(std::list<CSceneObject>::iterator it);
};

