#ifndef _AABBOX_H
#define _AABBOX_H

#include "..\Tools.h"

namespace Misc
{

class CAABBox
{
public:

	CAABBox();
	~CAABBox();

	void setCenter(const CVector3 &center) { mCenter = center; }
	void setHalfSize(const CVector3 &hs) { mHalfSize = hs; }
	/// Set the size of the bbox (ie 2* the halfSize).
	//void setSize(const CVector3 &s) { HalfSize = s / 2; }

	void extend(const CVector3 &v);

	//CVector3 getMin() const {return Center - HalfSize;}
	//CVector3 getMax() const {return Center + HalfSize;}
	//void getMin(CVector3 &ret) const {ret= Center-HalfSize;}
	//void getMax(CVector3 &ret) const {ret= Center+HalfSize;}
	const CVector3 &getCenter() const  {return mCenter; }
	const CVector3 &getHalfSize() const { return mHalfSize; }
	/// Return the size of the bbox.
	//CVector3 getSize() const { return HalfSize * 2; }
	//void getSize(CVector3 &ret) const { ret = HalfSize * 2; }
	/// Return the radius of the bbox.
	float getRadius() const { return mHalfSize.normalize(); }

	///// Is the bbox partially in front of the plane??
	//bool clipFront(const CPlane &p) const;
	///// Is the bbox partially in back of the plane??
	//bool clipBack(const CPlane &p) const;
	/// Does the bbox include this point.
	bool include(const CVector3 &a) const;
	/// Does the bbox include entirely this bbox.
	bool include(const CAABBox &box) const;
	/// Does the bbox intersect the bbox box.
	bool intersect(const CAABBox &box) const;
	/// Does the bbox intersect the triangle ABC.
	bool intersect(const CVector3 &a, const CVector3 &b, const CVector3 &c) const;
	///// Does the bbox instersect the sphere s
	//bool intersect(const CBSphere &s) const;
	/// Does the bbox instersect the segment AB
	bool intersect(const CVector3 &a, const CVector3 &b) const;
	/// clip the segment by the bbox. return false if don't intersect. a and b are modified.
	bool clipSegment(CVector3 &a, CVector3 &b) const;

	/**
	* Compute the union of 2 bboxs, that is the aabbox that contains the 2 others.
	*/
	static CAABBox computeAABBoxUnion(const CAABBox &b1, const CAABBox &b2);

	/**
	* Compute the intersection of 2 bboxs.
	*	this methods suppose the intersection exist, and doesn't check it (use intersect() to check).
	*	If !intersect, *this is still modified and the result bbox is big shit.
	*/
	void computeIntersection(const CAABBox &b1, const CAABBox &b2);

private:
	/// The center of the bbox.
	CVector3 mCenter;
	/// The size / 2 of the bbox.
	CVector3 mHalfSize;
};


/*
class	CAABBoxExt : private CAABBox
{
protected:
	float RadiusMin, RadiusMax;

	//void updateRadius()
	//{
	//	// The bounding sphere.
	//	RadiusMax = CAABBox::getRadius();
	//	// The including sphere.
	//	RadiusMin = NLMISC::minof((float)fabs(HalfSize.x), (float)fabs(HalfSize.y), (float)fabs(HalfSize.z));
	//}

public:
	/// Empty bbox Constructor
	CAABBoxExt() {RadiusMin= RadiusMax=0;}
	/// Constructor from a normal BBox.
	CAABBoxExt(const CAABBox &o) {RadiusMin= RadiusMax=0; *this=o;}

	/// \name Builds.
	// @{
	void setCenter(const CVector3 &center) { Center = center; }
	//void setHalfSize(const CVector3 &hs) { HalfSize = hs; updateRadius(); }
	//void setSize(const CVector3 &s) { HalfSize = s/2;  updateRadius(); }
	///// Build the bbox, with a min/max style bbox.
	//void setMinMax(const CVector3 &bmin, const CVector3 &bmax)
	//{
	//	Center= (bmin+bmax)/2;
	//	HalfSize= bmax-Center;
	//	updateRadius();
	//}
	CAABBoxExt &operator=(const CAABBox &o)
	{
		Center = o.getCenter();
		HalfSize = o.getHalfSize();
		updateRadius();
		return (*this);
	}
	//@}


	/// \name Gets.
	// @{
	CVector3 getMin() const { return CAABBox::getMin(); }
	CVector3 getMax() const { return CAABBox::getMax(); }
	const CVector3 &getCenter() const { return Center; }
	const CVector3 &getHalfSize() const { return HalfSize; }
	/// Return the size of the bbox.
	CVector3 getSize() const { return HalfSize * 2; }
	/// Return the (stored!!) radius of the bbox.
	float getRadius() const { return RadiusMax; }
	/// Return a simple Axis Aligned Bounding Box (no radius inside)
	CAABBox getAABBox() const;
	// @}

	/// \name Clip
	// @{
	///// Is the bbox partially in front of the plane?? p MUST be normalized.
	//bool clipFront(const CPlane &p) const;
	///// Is the bbox partially in back of the plane?? p MUST be normalized.
	//bool clipBack(const CPlane &p) const;
	/// Does the bbox intersect the bbox box.
	bool intersect(const CAABBoxExt &box) const
		{return CAABBox::intersect(box);}
	/// Does the bbox intersect the triangle ABC.
	bool intersect(const CVector3 &a, const CVector3 &b, const CVector3 &c) const
		{ return CAABBox::intersect(a, b, c); }
	// @}
};
*/

}


#endif