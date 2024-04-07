
// Library includes

// Project includes
#include "Frustum.h"
#include "Object.h"


inline float fastSquareRoot(float x)
{
  __asm{
    fld x;
    fsqrt;
    fstp x;
  }
  return x;
}


CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

bool CFrustum::boxInFrustum(const AABBVolume &aabb) const
{
	int p = 0;

	for(int i = 0; i < 6; i += 1)
	{
		if(pointInFrustum(aabb.X1, aabb.Y1, aabb.Z1))
			p += 1;
		if(pointInFrustum(aabb.X2, aabb.Y1, aabb.Z1))
			p += 1;
		if(pointInFrustum(aabb.X2, aabb.Y1, aabb.Z2))
			p += 1;
		if(pointInFrustum(aabb.X1, aabb.Y1, aabb.Z2))
			p += 1;
		if(pointInFrustum(aabb.X1, aabb.Y2, aabb.Z1))
			p += 1;
		if(pointInFrustum(aabb.X2, aabb.Y2, aabb.Z1))
			p += 1;
		if(pointInFrustum(aabb.X2, aabb.Y2, aabb.Z2))
			p += 1;
		if(pointInFrustum(aabb.X1, aabb.Y2, aabb.Z2))
			p += 1;
	}

	return (p > 0);
}

bool CFrustum::checkFrustum(const AABBVolume &aabb) const
{
	switch(aabb.volumeType)
	{
	case BoundingVolume::Box:
		return boxInFrustum(aabb);
		break;
	case BoundingVolume::Cylinder:
		return cylinderInFrustum(aabb);
		break;
	case BoundingVolume::Sphere:
		return sphereInFrustum(aabb);
		break;
	}

	// if no bounding volume type is set:
	return true;
}

bool CFrustum::cylinderInFrustum(const AABBVolume &aabb) const
{
	int o = 0;
	// Go through all the sides of the frustum
	for(int i = 5; i >= 0; i -= 1)
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( frustumPlanes[i].a * aabb.CX + frustumPlanes[i].b * aabb.CY + frustumPlanes[i].c * aabb.CZ + frustumPlanes[i].d <= -aabb.Radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			//return false;
			o += 1;
		}
	}

	if(o == 6)
		return false;
	else
		return true;
}

/*
 *This retrieves our distance from a frustum plane
 */
float CFrustum::getDistanceToPlane(int i, const CVector3 &center) const
{
	return	frustumPlanes[i].a * center.x +
			frustumPlanes[i].b * center.y +
			frustumPlanes[i].c * center.z + 
			frustumPlanes[i].d;
}

bool CFrustum::pointInFrustum(const CVector3 &center) const
{
	return pointInFrustum(center.x, center.y, center.z);
}

inline bool CFrustum::pointInFrustum(float x, float y, float z) const
{
	// Go through all the sides of the frustum
	for(int i = 5; i >= 0; i -= 1)
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(frustumPlanes[i].a * x + frustumPlanes[i].b * y + frustumPlanes[i].c * z + frustumPlanes[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}

inline bool CFrustum::sphereInFrustum(const AABBVolume &aabb) const
{
	int o = 0;
	// Go through all the sides of the frustum
	for(int i = 5; i >= 0; i -= 1)
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( frustumPlanes[i].a * aabb.CX + frustumPlanes[i].b * aabb.CY + frustumPlanes[i].c * aabb.CZ + frustumPlanes[i].d <= -aabb.Radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			//return false;
			o += 1;
		}
	}

	if(o == 6)
		return false;
	else
		return true;
}

bool CFrustum::volumeInFrustum(const AABBVolume &aabb)
{
	return checkFrustum(aabb);
}

void CFrustum::Update()
{
	static Matrix4f proj,								// This will hold our projection matrix
					modl,								// This will hold our modelview matrix
					comboMatrix;						// This will hold the clipping planes

	FrustumPlane *p;

	// glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	comboMatrix  = proj;
	comboMatrix *= modl;

	// Right clipping plane
	p = &frustumPlanes[RIGHT];
	p->a = comboMatrix[3] - comboMatrix[0];
	p->b = comboMatrix[7] - comboMatrix[4];
	p->c = comboMatrix[11] - comboMatrix[8];
	p->d = comboMatrix[15] - comboMatrix[12];
	// Left clipping plane
	p = &frustumPlanes[LEFT];
	p->a = comboMatrix[3] + comboMatrix[0];
	p->b = comboMatrix[7] + comboMatrix[4];
	p->c = comboMatrix[11] + comboMatrix[8];
	p->d = comboMatrix[15] + comboMatrix[12];
	// Bottom clipping plane
	p = &frustumPlanes[BOTTOM];
	p->a = comboMatrix[3] + comboMatrix[1];
	p->b = comboMatrix[7] + comboMatrix[5];
	p->c = comboMatrix[11] + comboMatrix[9];
	p->d = comboMatrix[15] + comboMatrix[13];
	// Top clipping plane
	p = &frustumPlanes[TOP];
	p->a = comboMatrix[3] - comboMatrix[1];
	p->b = comboMatrix[7] - comboMatrix[5];
	p->c = comboMatrix[11] - comboMatrix[9];
	p->d = comboMatrix[15] - comboMatrix[13];
	// Far clipping plane
	p = &frustumPlanes[BACK];
	p->a = comboMatrix[3] - comboMatrix[2];
	p->b = comboMatrix[7] - comboMatrix[6];
	p->c = comboMatrix[11] - comboMatrix[10];
	p->d = comboMatrix[15] - comboMatrix[14];
	// Near clipping plane
	p = &frustumPlanes[FRONT];
	p->a = comboMatrix[3] + comboMatrix[2];
	p->b = comboMatrix[7] + comboMatrix[6];
	p->c = comboMatrix[11] + comboMatrix[10];
	p->d = comboMatrix[15] + comboMatrix[14];
	
	float magnitude = 0.0f;
	for(int i = 5; i >= 0; i -= 1)
	{
		magnitude = fastSquareRoot(	frustumPlanes[i].a * frustumPlanes[i].a + 
									frustumPlanes[i].b * frustumPlanes[i].b + 
									frustumPlanes[i].c * frustumPlanes[i].c );
		if(magnitude)
		{
			frustumPlanes[i].a /= magnitude;
			frustumPlanes[i].b /= magnitude;
			frustumPlanes[i].c /= magnitude;
			frustumPlanes[i].d /= magnitude;
		}
	}
}