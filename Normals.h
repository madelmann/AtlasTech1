#ifndef _NORMALS_H
#define _NORMALS_H


#include "Tools.h"


class CVertex
{
public:
	CVector3 Vertex;
	CVector3 vNormal;
};

class CFace
{
public:
	CVector3 v[3];
	CVector3 vNormal;
};


CVector3 Normalize(CVector3 vVector);

void GetNormal(CVector3 v[3], CVector3 *normal);

#endif