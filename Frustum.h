#ifndef _FRUSTUM_H
#define _FRUSTUM_H


#include "Tuple2.h"
#include "Tuple3.h"
#include "Tuple4.h"
#include "Matrix4.h"
#include "Main.h"


class BoundsDescriptor;


#define RIGHT	0	// The RIGHT side of the frustum
#define LEFT	1	// The LEFT	 side of the frustum
#define BOTTOM	2	// The BOTTOM side of the frustum
#define TOP		3	// The TOP side of the frustum
#define BACK	4	// The BACK	side of the frustum
#define FRONT	5	// The FRONT side of the frustum


struct FrustumPlane
{
	float a, b, c, d;
};

class CFrustum
{
public:
	CFrustum();
	~CFrustum();
	bool boxInFrustum(const AABBVolume &aabb) const;
	bool checkFrustum(const AABBVolume &aabb) const;
	bool cylinderInFrustum(const AABBVolume &aabb) const;
    float getDistanceToPlane(int plane, const CVector3 &center) const;
	bool pointInFrustum(float x, float y, float z) const;
    inline bool pointInFrustum(const CVector3 &center) const;
	bool sphereInFrustum(const AABBVolume &aabb) const;
	bool volumeInFrustum(const AABBVolume &aabb);

	void Update();

	float		fFarPlane;
	float		fFOV;
	float		fNearPlane;
	float		fRatio;
	FrustumPlane	frustumPlanes[6];
};

#endif