#ifndef _QUADTREE_H
#define _QUADTREE_H


#include "Main.h"

class CVertex;


#define MAX_GRADIANT			0.2
#define LOD0					1				// This is width and height of each triangle grid
#define LOD1					LOD0 + 1
#define LOD2					LOD0 + 3


class Node
{
public:
	Node() { }
	~Node() { }

	AABBVolume		BoundingVolume;
	bool			bOcclusionRender;
	int				iAffactedByLight;
	int				iMeshId;
	unsigned int	uiOcclusionCalculatedBeforeFPS;
};

class CQuadTree
{
public:
	CQuadTree();
	~CQuadTree();

	POINT CanMoveGradient(float oldx, float oldz, float newx, float newz);
	void Clear();
	float getHeight(float x, float z);
	float getRoofHeight(float x, float z);
	bool LoadFloor(char* Filename);
	bool LoadRoof(char* Filename);
	void Render();

	float		fTerrainXEnd;
	float		fTerrainXStart;
	float		fTerrainZEnd;
	float		fTerrainZStart;

private:

	void DrawBoundingBox(AABBVolume &bbox);
	void DrawNode(Node** &list, CVertex** &mesh, int X, int Z);
	void Process(Node** &list, CVertex** &mesh);
	void RenderPoints();
	void SetTextureCoord(float x, float z);

	Node**		mFloorList;
	CVertex**	mFloorMesh;
	int			mHeight;
	int			mNodeCount;
	Node**		mRoofList;
	CVertex**	mRoofMesh;
	int			mSizeX;
	int			mSizeZ;
	int			mWidth;
	int			mVertexCount;
};



#endif