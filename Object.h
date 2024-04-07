
#pragma once

// Library includes

// Project includes
#include "GLWrapper.h"
#include "Tools.h"


class CShader;


namespace Physic
{

	enum Force_e {
		None = 0,
		ConstantPush = 1,
		Gravity = 2,
		Push = 3
	};

}

namespace BoundingVolume
{

	enum BoundingVolumeType_t {
		None = 0,
		Box = 1,
		Cylinder = 2,
		Sphere = 3
	};

}

struct AABBVolume
{
	AABBVolume() { }
	AABBVolume(float width, float height, float depth)
	: X1(- width / 2)
	, X2(width / 2)
	, Y1(-height / 2)
	, Y2( height / 2 )
	, Z1(- depth / 2)
	, Z2( depth / 2)
	, volumeType( BoundingVolume::Box )
	{ }

	CVector3 getCenter()
	{
		return CVector3(CX, CY, CZ);
	}

	float X1{ 0.f };
	float X2{ 0.f };
	float Y1{ 0.f };
	float Y2{ 0.f };
	float Z1{ 0.f };
	float Z2{ 0.f };
	float CX{ 0.f };
	float CY{ 0.f };
	float CZ{ 0.f };
	float Radius{ 0.f };
	BoundingVolume::BoundingVolumeType_t volumeType;
};

struct TAdditionalCheck
{
	int		iCheckedBefore;
	bool	bResult;

	TAdditionalCheck()
	{
		iCheckedBefore = 0;
		bResult = true;
	}

	~TAdditionalCheck()
	{
		iCheckedBefore = 0;
		bResult = true;
	}
};


class CObject
{
public:
	CObject()
	: mId(-1),
	  mName("")
	{ }

	virtual ~CObject() { }

	int getId()
	{
		return mId;
	}
	void setId(int id)
	{
		mId = id;
	}
	char* getName()
	{
		return mName;
	}
	void setName(char* name)
	{
		mName = name;
	}


//private:
	int		mId;
	char*	mName;
};


class CPhysicObject : public CObject
{
public:
	CPhysicObject();
	CPhysicObject(float mass);
	virtual ~CPhysicObject() { }

	void ApplyForce(Physic::Force_e force, CVector3 vforce);
	void DoForce(Physic::Force_e force);
	void DoPhysic(bool dophysic);
	bool GetDoPhysic();
	CVector3 GetPosition();
	void SetGravity(CVector3 gravity);

//private:
	BoundingVolume::BoundingVolumeType_t	BoundingVolumeType;
	BoundingVolume::BoundingVolumeType_t	mCollisionVolumeType;
	bool				mApplyPhysic;
	CVector3			mGravity;
	double				mLastPhysic;
	float				mMass;
	CVector3			mPosition;
	CVector3			mPositionOld;
	CVector3			mRotation;
	CVector3			mVelocity;
	CVector3			mVelocityPush;
};


class CGraphicObject : public CPhysicObject
{
public:
	CGraphicObject();
	virtual ~CGraphicObject();

	bool BoxCollision(CVector3 vector, float radius);
	bool BoxCollision(CVector3 vector, float radius, bool force);
	bool CylinderCollision(CVector3 vector, float radius);
	bool CylinderCollision(CVector3 vector, float radius, bool force);
	virtual int DeepCollision(CVector3 vColPos, float radius) { return 1; };
	CVector3 GetPosition();
	virtual void Manage() {};
	virtual void PreProcess() {};
	virtual void RebuildBoundingBox();
	virtual void Render() {};
	void RenderBoundingVolume();
	virtual void RenderWithoutDisplayList() {};
	virtual void SetBoundingBox() {};
	virtual void SetBoundingBox(AABBVolume &box);
	virtual void setMaterial(GLuint material);
	virtual void setNormalMap(GLuint normalmap);
	virtual void setShader(CShader* shader);
	virtual void setTexture(GLuint texture);
	void SetPosition(CVector3 position);
	void SetSelectionIndicator();
	virtual void Spawn() {};
	bool SphereCollision(CVector3 vector, float radius);
	bool SphereCollision(CVector3 vector, float radius, bool force);
	bool SphereCollisionLine(CVector3 vector, float radius);
	bool SphereCollisionPoint(CVector3 vector);
	virtual int Update(float fElapsedTime) { return S_OK; };

	AABBVolume			BoundingVolume;
	//AABBVolume			BoundingVolume_original;
	bool				bDoCollision;
	bool				bPickable;
	bool				bSolidBoundingVolume;
	bool				bSpawned;
	bool				bVisible;
	char				cTexture[255];
	float				fRadius;
	float				fScale;
	float				fTranslationX;
	float				fTranslationY;
	float				fTranslationZ;
	int					iHealth;
	int					iMaterial;
	int					iMeshID;
	int					iModel;
	GLuint				iNormalMap;
	int					iOctreeX;
	int					iOctreeZ;
	GLuint				iTexture;
	int					iType;
	GLuint				gluivMeshID_LOD[3];
	char				sTriggerScript[255];
	TAdditionalCheck	tAddidionalCheck;
	BoundingVolume::BoundingVolumeType_t	VisibleVolumeType;
	CVector3			vSelectionIndicator;

protected:
	CShader*			mShader;
};

