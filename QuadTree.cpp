
// Library includes

// Project includes
#include "EngineUtils.h"
#include "QuadTree.h"
#include "Tools.h"


CQuadTree::CQuadTree()
: mFloorList(NULL),
  mFloorMesh(NULL),
  mHeight(0),
  mNodeCount(0),
  mRoofList(NULL),
  mRoofMesh(NULL),
  mSizeX(0),
  mSizeZ(0),
  mVertexCount(0),
  mWidth(0)
{
}

CQuadTree::~CQuadTree()
{
	Clear();
}

POINT CQuadTree::CanMoveGradient(float oldx, float oldz, float newx, float newz)
{
	POINT Result = {0, 0};
	float oldy, newy;

	oldy = getHeight(oldx, oldz);
	newy = getHeight(newx, oldz);

	if(newy - oldy > MAX_GRADIANT)
		Result.x = -1;
	else if(newy - oldy < 0.0f)
		Result.x = 0;

	oldy = getHeight(oldx, oldz);
	newy = getHeight(oldx, newz);

	if(newy - oldy > MAX_GRADIANT)
		Result.y = -1;
	else if(newy - oldy < 0.0f)
		Result.y = 0;

	return Result;
}

void CQuadTree::Clear()
{
	if(mHeight && mWidth)
	{
		int width16 = (mWidth - 2) / 16;
		int height16 = (mHeight - 2) / 16;

		for(int x = 0; x < width16; x += 1)
		{
			for(int y = 0; y < height16; y += 1)
			{
				if(mFloorList)
					glDeleteLists(mFloorList[x][y].iMeshId, 1);
				if(mRoofList)
					glDeleteLists(mRoofList[x][y].iMeshId, 1);
			}
		}

		for(int i = 0; i < mWidth; i += 1)
		{
			if(mFloorMesh)
				delete [] mFloorMesh[i];
			if(mRoofMesh)
				delete [] mRoofMesh[i];
		}

		for(int i = 0; i < width16; i += 1)
		{
			if(mFloorList)
				delete [] mFloorList[i];
			if(mRoofList)
				delete [] mRoofList[i];
		}

		if(mFloorMesh)
			delete [] mFloorMesh;
		if(mFloorList)
			delete [] mFloorList;
		if(mRoofMesh)
			delete [] mRoofMesh;
		if(mRoofList)
			delete [] mRoofList;
	}

	mFloorList = NULL;
	mFloorMesh = NULL;
	mRoofList = NULL;
	mRoofMesh = NULL;
}

void CQuadTree::DrawBoundingBox(AABBVolume &bbox)
{
	float X1, X2, Y1, Y2, Z1, Z2;
	X1 = bbox.X1;
	X2 = bbox.X2;
	Y1 = bbox.Y1;
	Y2 = bbox.Y2;
	Z1 = bbox.Z1;
	Z2 = bbox.Z2;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINE_LOOP);
		glVertex3f(X1, Y1, Z1);
		glVertex3f(X1, Y1, Z2);
		glVertex3f(X2, Y1, Z2);
		glVertex3f(X2, Y1, Z1);
		glVertex3f(X1, Y1, Z1);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3f(X1, Y2, Z1);
		glVertex3f(X1, Y2, Z2);
		glVertex3f(X2, Y2, Z2);
		glVertex3f(X2, Y2, Z1);
		glVertex3f(X1, Y2, Z1);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(X1, Y1, Z1);
		glVertex3f(X1, Y2, Z1);

		glVertex3f(X1, Y1, Z2);
		glVertex3f(X1, Y2, Z2);

		glVertex3f(X2, Y1, Z2);
		glVertex3f(X2, Y2, Z2);

		glVertex3f(X2, Y1, Z1);
		glVertex3f(X2, Y2, Z1);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void CQuadTree::DrawNode(Node** &list, CVertex** &mesh, int X, int Z)
{
	// We want to render triangle strips
	for(int u = X; u < X + 16; u += 1)
	{
		for(int v = Z; v < Z + 16; v += 1)
		{
			if(mesh[u - 1][v - 1].Vertex.y > list[(X - 1) / 16][(Z - 1) / 16].BoundingVolume.Y2)
				list[(X - 1) / 16][(Z - 1) / 16].BoundingVolume.Y2 = mesh[u - 1][v - 1].Vertex.y;
			if(mesh[u - 1][v - 1].Vertex.y < list[(X - 1) / 16][(Z - 1) / 16].BoundingVolume.Y1)
				list[(X - 1) / 16][(Z - 1) / 16].BoundingVolume.Y1 = mesh[u - 1][v - 1].Vertex.y;

			glBegin(GL_TRIANGLES);
				glNormal3f(mesh[u - 1][v - 1].vNormal.x, mesh[u - 1][v - 1].vNormal.z, mesh[u - 1][v - 1].vNormal.y);
				SetTextureCoord(mesh[u - 1][v - 1].Vertex.x, mesh[u - 1][v - 1].Vertex.z);
				glVertex3f(mesh[u - 1][v - 1].Vertex.x, mesh[u - 1][v - 1].Vertex.y, mesh[u - 1][v - 1].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u - 1][v - 1].Vertex.y);

				glNormal3f(mesh[u][v - 1].vNormal.x, mesh[u][v - 1].vNormal.z, mesh[u][v - 1].vNormal.y);
				SetTextureCoord(mesh[u][v - 1].Vertex.x, mesh[u][v - 1].Vertex.z);
				glVertex3f(mesh[u][v - 1].Vertex.x, mesh[u][v - 1].Vertex.y, mesh[u][v - 1].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u][v - 1].Vertex.y);

				glNormal3f(mesh[u - 1][v].vNormal.x, mesh[u - 1][v].vNormal.z, mesh[u - 1][v].vNormal.y);
				SetTextureCoord(mesh[u - 1][v].Vertex.x, mesh[u - 1][v].Vertex.z);
				glVertex3f(mesh[u - 1][v].Vertex.x, mesh[u - 1][v].Vertex.y, mesh[u - 1][v].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u - 1][v].Vertex.y);

				glNormal3f(mesh[u - 1][v].vNormal.x, mesh[u - 1][v].vNormal.z, mesh[u - 1][v].vNormal.y);
				SetTextureCoord(mesh[u - 1][v].Vertex.x, mesh[u - 1][v].Vertex.z);
				glVertex3f(mesh[u - 1][v].Vertex.x, mesh[u - 1][v].Vertex.y, mesh[u - 1][v].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u - 1][v].Vertex.y);

				glNormal3f(mesh[u][v - 1].vNormal.x, mesh[u][v - 1].vNormal.z, mesh[u][v - 1].vNormal.y);
				SetTextureCoord(mesh[u][v - 1].Vertex.x, mesh[u][v - 1].Vertex.z);
				glVertex3f(mesh[u][v - 1].Vertex.x, mesh[u][v - 1].Vertex.y, mesh[u][v - 1].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u][v - 1].Vertex.y);

				glNormal3f(mesh[u][v].vNormal.x, mesh[u][v].vNormal.z, mesh[u][v].vNormal.y);
				SetTextureCoord(mesh[u][v].Vertex.x, mesh[u][v].Vertex.z);
				glVertex3f(mesh[u][v].Vertex.x, mesh[u][v].Vertex.y, mesh[u][v].Vertex.z);
				// Set the fog coordinate for this vertex
				//SetFogCoord(Map.fFogDepth, octreemap->OctreeMap[x][z].Vertices[u][v].Vertex.y);
			glEnd();
		}
	}
}

float CQuadTree::getHeight(float x, float z)
{
	if(!mFloorMesh)
		return 0.0f;

	// Check if position is inside of heightmap
	if(x < mFloorMesh[0][0].Vertex.x || x > mFloorMesh[mWidth - 1][0].Vertex.x || z < mFloorMesh[0][0].Vertex.z || z > mFloorMesh[0][mHeight - 1].Vertex.z)
		return 0.0f;

	// Calculate indices for terrain array
	int X = int(x / g_iHeightMapScaleX);
	int Z = int(z / g_iHeightMapScaleZ);

	// Calculate deltas
	float dx = abs(x - int(X * g_iHeightMapScaleX));
	float dz = abs(z - int(Z * g_iHeightMapScaleZ));

	// Do the thing
	CVector3 a;
	CVector3 b;
	CVector3 c;

	CVector3 ab;
	CVector3 ac;

	if(dx > dz)
	{
		a = mFloorMesh[X][Z].Vertex;
		b = mFloorMesh[X + 1][Z].Vertex;
		c = mFloorMesh[X + 1][Z + 1].Vertex;

		ab = b - a;
		ac = c - a;
	}
	else
	{
		a = mFloorMesh[X][Z].Vertex;
		b = mFloorMesh[X + 1][Z + 1].Vertex;
		c = mFloorMesh[X][Z + 1].Vertex;

		ab = a - b;
		ac = c - b;
	}

	CVector3 normalv;

	normalv.x = ab.y * ac.z - ab.z * ac.y;
	normalv.y = -(ab.x * ac.z - ab.z * ac.x);
	normalv.z = ab.x * ac.y - ab.y * ab.x;

	return (normalv.x * (a.x - x) + normalv.z * (a.z - z) + normalv.y * a.y) / normalv.y;
}

float CQuadTree::getRoofHeight(float x, float z)
{
	if(!mRoofMesh)
		return 0.0f;

	// Check if position is inside of heightmap
	if(x < mRoofMesh[0][0].Vertex.x || x > mRoofMesh[mWidth - 1][0].Vertex.x || z < mRoofMesh[0][0].Vertex.z || z > mRoofMesh[0][mHeight - 1].Vertex.z)
		return 0.0f;

	// Calculate indices for terrain array
	int X = int(x / g_iHeightMapScaleX);
	int Z = int(z / g_iHeightMapScaleZ);

	// Calculate deltas
	float dx = abs(x - int(X * g_iHeightMapScaleX));
	float dz = abs(z - int(Z * g_iHeightMapScaleZ));

	// Do the thing
	CVector3 a;
	CVector3 b;
	CVector3 c;

	CVector3 ab;
	CVector3 ac;

	if(dx > dz)
	{
		a = mRoofMesh[X][Z].Vertex;
		b = mRoofMesh[X + 1][Z].Vertex;
		c = mRoofMesh[X + 1][Z + 1].Vertex;

		ab = b - a;
		ac = c - a;
	}
	else
	{
		a = mRoofMesh[X][Z].Vertex;
		b = mRoofMesh[X + 1][Z + 1].Vertex;
		c = mRoofMesh[X][Z + 1].Vertex;

		ab = a - b;
		ac = c - b;
	}

	CVector3 normalv;

	normalv.x = ab.y * ac.z - ab.z * ac.y;
	normalv.y = -(ab.x * ac.z - ab.z * ac.x);
	normalv.z = ab.x * ac.y - ab.y * ab.x;

	return (normalv.x * (a.x - x) + normalv.z * (a.z - z) + normalv.y * a.y) / normalv.y;
}

bool CQuadTree::LoadFloor(char* Filename)
{
	Filename = RemoveFileExt(Filename);

	FILE* pFile;
	char buffer[255] = "";
	TGAImg Img;

	sprintf(buffer, "%sMaps\\%s\\Terrain\\%s.tga", g_sGameName, Game.sMapName, Filename);

	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTerrain\\%s.tga", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[LoadQuadTree]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return false;
		}
	}
	fclose(pFile);
 

	// Load our Texture
	if(Img.Load(buffer) != IMG_OK)
	{
		// Display an error message saying the file was not found, then return NULL
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' could not be opened!\n", Filename);
		Console->Output(buffer);
		return false;
	}


	int iBPP = Img.GetBPP() / 8;
	if(iBPP != 4)
	{
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' does not have 4 channels.\n", Filename);
		Console->Output(buffer);
		return false;
	}


	mHeight = Img.GetHeight();
	mWidth = Img.GetWidth();
	int width = (mWidth - 2);
	int height = (mHeight - 2);
	int width16 = (mWidth - 2) / 16;
	int height16 = (mHeight - 2) / 16;

	if( width % 16 != 0 || height % 16 != 0 )
	{
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' size is not acceptable!\n", Filename);
		Console->Output(buffer);
		return false;
	}
	else
	{
		mNodeCount = (width16 * height16);
		mSizeX = width16;
		mSizeZ = height16;
	}


	unsigned char* pImage = Img.GetImg();

	// Process Texture
	int nSize = mHeight * mWidth;
	mVertexCount = nSize - (width * 2 + height * 2);
	int d = 0, x = 0, z = 0;

	mFloorMesh = new CVertex*[mWidth];
	for(int i = 0; i < mWidth; i++)
		mFloorMesh[i] = new CVertex[mHeight];

	mFloorList = new Node*[width16];
	for(int i = 0; i < width16; i++)
		mFloorList[i] = new Node[height16];

	for(int i = 0; i < nSize; i += 1)
	{
		z = (int)(i / mHeight);
		x = i - z * mWidth;
		d = i * iBPP;

		int value;

		// Normals
		value = pImage[d++];
		mFloorMesh[x][z].vNormal.x = float(value) / 256.0f;
		value = pImage[d++];
		mFloorMesh[x][z].vNormal.y = float(value) / 256.0f;
		value = pImage[d++];
		mFloorMesh[x][z].vNormal.z = float(value) / 256.0f;
		// Heightmap
		mFloorMesh[x][z].Vertex.x = x * g_iHeightMapScaleX;
		value = pImage[d++];
		mFloorMesh[x][z].Vertex.y = float(value) * g_iHeightMapScaleY + Map.fFloorOffset;
		mFloorMesh[x][z].Vertex.z = z * g_iHeightMapScaleZ;
	}

	Img.~TGAImg();

	Process(mFloorList, mFloorMesh);

	fTerrainXEnd = mFloorMesh[mWidth - 1][mHeight - 1].Vertex.x;
	fTerrainXStart = mFloorMesh[0][0].Vertex.x;
	fTerrainZEnd = mFloorMesh[mWidth - 1][mHeight - 1].Vertex.z;
	fTerrainZStart = mFloorMesh[0][0].Vertex.z;

	return true;
}

bool CQuadTree::LoadRoof(char* Filename)
{
	Filename = RemoveFileExt(Filename);

	FILE* pFile;
	char buffer[255] = "";
	TGAImg Img;

	sprintf(buffer, "%sMaps\\%s\\Terrain\\%s.tga", g_sGameName, Game.sMapName, Filename);

	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTerrain\\%s.tga", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[LoadQuadTree]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return false;
		}
	}
	fclose(pFile);
 

	// Load our Texture
	if(Img.Load(buffer) != IMG_OK)
	{
		// Display an error message saying the file was not found, then return NULL
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' could not be opened!\n", Filename);
		Console->Output(buffer);
		return false;
	}


	int iBPP = Img.GetBPP() / 8;
	if(iBPP != 4)
	{
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' does not have 4 channels.\n", Filename);
		Console->Output(buffer);
		return false;
	}


	mHeight = Img.GetHeight();
	mWidth = Img.GetWidth();
	int width = (mWidth - 2) / 16;
	int height = (mHeight - 2) / 16;

	if((mHeight - 2) % 16 != 0 || (mWidth - 2) % 16 != 0)
	{
		sprintf(buffer, "ERROR[LoadQuadTree]: '%s' size is not acceptable!\n", Filename);
		Console->Output(buffer);
		return false;
	}
	else
	{
		mNodeCount = ((mHeight - 2) / 16) * ((mWidth - 2) / 16);

		mSizeX = (mWidth - 2) / 16;
		mSizeZ = (mHeight - 2) / 16;
	}


	unsigned char* pImage = Img.GetImg();

	// Process Texture
	int nSize = mHeight * mWidth;
	mVertexCount = nSize - (mHeight * 2 + mWidth * 2 - 4);
	int d = 0, x = 0, z = 0;

	mRoofMesh = new CVertex*[mWidth];
	for(int i = 0; i < mWidth; i += 1)
		mRoofMesh[i] = new CVertex[mHeight];

	mRoofList = new Node*[(mWidth - 2) / 16];
	for(int i = 0; i < width; i += 1)
		mRoofList[i] = new Node[height];

	for(int i = 0; i < nSize; i += 1)
	{
		z = (int)(i / mHeight);
		x = i - z * mWidth;
		d = i * iBPP;

		int value;

		// Normals
		value = pImage[d++];
		mRoofMesh[x][z].vNormal.x = float(value) / 256.0f;
		value = pImage[d++];
		mRoofMesh[x][z].vNormal.y = float(value) / 256.0f;
		value = pImage[d++];
		mRoofMesh[x][z].vNormal.z = float(value) / 256.0f;
		// Heightmap
		mRoofMesh[x][z].Vertex.x = x * g_iHeightMapScaleX;
		value = pImage[d++];
		mRoofMesh[x][z].Vertex.y = float(value) * g_iHeightMapScaleY + Map.fRoofOffset;
		mRoofMesh[x][z].Vertex.z = z * g_iHeightMapScaleZ;
	}

	Img.~TGAImg();

	Process(mRoofList, mRoofMesh);

	fTerrainXEnd = mFloorMesh[mWidth - 1][mHeight - 1].Vertex.x;
	fTerrainXStart = mFloorMesh[0][0].Vertex.x;
	fTerrainZEnd = mFloorMesh[mWidth - 1][mHeight - 1].Vertex.z;
	fTerrainZStart = mFloorMesh[0][0].Vertex.z;

	return true;
}

void CQuadTree::Process(Node** &list, CVertex** &mesh)
{
	float fRadiusX = (g_iHeightMapScaleX * 16) / 2;								// Scale * Nodegröße, davon die Hälfte

	for(int X = 0; X < mSizeX; X += 1)
	{
		for(int Z = 0; Z < mSizeZ; Z += 1)
		{
			list[X][Z].BoundingVolume.X1 = mesh[X * 16][Z * 16].Vertex.x;
			list[X][Z].BoundingVolume.Y1 = list[X][Z].BoundingVolume.Y2 = mesh[X * 16][Z * 16].Vertex.y;
			list[X][Z].BoundingVolume.Z1 = mesh[X * 16][Z * 16].Vertex.z;

			// Destroy display list if already exists.
			if(glIsList(list[X][Z].iMeshId))
			{
				glDeleteLists(list[X][Z].iMeshId, 1);
			}

			list[X][Z].iMeshId = glGenLists(1);
			if(list[X][Z].iMeshId == 0)
			{
				//TODO : throw an exception
				return;
			}

			float fRadiusY = list[X][Z].BoundingVolume.Y2 - list[X][Z].BoundingVolume.CY;
			list[X][Z].BoundingVolume.Radius = sqrt(fRadiusX * fRadiusX + fRadiusY * fRadiusY + fRadiusX * fRadiusX);
			
			glNewList(list[X][Z].iMeshId, GL_COMPILE);
				// Set ambient color
				glColor3f(Map.fAmbientColor[0], Map.fAmbientColor[1], Map.fAmbientColor[2]);
   
				// Second Texture
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glEnable(GL_TEXTURE_2D);
				Texture_SetActive(Map.sSecondTexture);

				// Base texture
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glEnable(GL_TEXTURE_2D);
				Texture_SetActive(Map.sBaseTexture);

				// Alpha map
				glActiveTextureARB(GL_TEXTURE2_ARB);
				glEnable(GL_TEXTURE_2D);
				Texture_SetActive(Map.sAlphaMap);

				// COMBINATION OF ALPHA AND 2 GROUNDTEXTURES
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,	GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,	GL_INTERPOLATE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,	GL_TEXTURE0);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,	GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,	GL_TEXTURE1);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,	GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB,	GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB,	GL_SRC_COLOR);

				// Shadows
				glActiveTextureARB(GL_TEXTURE3_ARB);
				glEnable(GL_TEXTURE_2D);
				Texture_SetActiveID(0);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,	GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,	GL_MODULATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,	GL_PREVIOUS_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,	GL_SRC_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,	GL_PRIMARY_COLOR_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,	GL_SRC_COLOR);

				DrawNode(list, mesh, X * 16 + 1, Z * 16 + 1);

				// Shadows
				glActiveTextureARB(GL_TEXTURE3_ARB);
				glDisable(GL_TEXTURE_2D);

				// Alpha map
				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);
				
				// Base texture
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				
				// Second Texture
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);
			glEndList();

			list[X][Z].BoundingVolume.X2 = mesh[X * 16 + 16][Z * 16 + 16].Vertex.x;
			list[X][Z].BoundingVolume.Z2 = mesh[X * 16 + 16][Z * 16 + 16].Vertex.z;

			list[X][Z].BoundingVolume.CX = abs(list[X][Z].BoundingVolume.X2 - list[X][Z].BoundingVolume.X1) / 2 + list[X][Z].BoundingVolume.X1;
			list[X][Z].BoundingVolume.CY = abs(list[X][Z].BoundingVolume.Y2 - list[X][Z].BoundingVolume.Y1) / 2 + list[X][Z].BoundingVolume.Y1;
			list[X][Z].BoundingVolume.CZ = abs(list[X][Z].BoundingVolume.Z2 - list[X][Z].BoundingVolume.Z1) / 2 + list[X][Z].BoundingVolume.Z1;
		}
	}
}

void CQuadTree::Render()
{
	Material_Apply(Map.iMaterial, true);

	AABBVolume aabb;

	bool playerOverRoof = Game.mPlayer->mPosition.y >= getRoofHeight(Game.mPlayer->mPosition.x, Game.mPlayer->mPosition.z);

	for(int x = Game.mPlayer->iOctreeX - g_iViewWidth; x <= Game.mPlayer->iOctreeX + g_iViewWidth; x += 1)
	{
		for(int z = Game.mPlayer->iOctreeZ - g_iViewWidth; z <= Game.mPlayer->iOctreeZ + g_iViewWidth; z += 1)
		{
			if(x >= 0 && x < mSizeX && z >= 0 && z < mSizeZ)
			{
				if(Engine->bDrawFloor)
				{
					aabb = mFloorList[x][z].BoundingVolume;
					if(Scene.GetRenderWater())
					{
						aabb.CY = -mFloorList[x][z].BoundingVolume.CY;
					}
					else
					{
						glCullFace(GL_FRONT);
						aabb.CY = mFloorList[x][z].BoundingVolume.CY;
					}

					if(Frustum.sphereInFrustum(aabb))
						glCallList(mFloorList[x][z].iMeshId);
				}

				if(Engine->bDrawRoof)
				{
					if(Scene.GetRenderWater())
					{
						aabb.CY = -mRoofList[x][z].BoundingVolume.CY;
					}
					else
					{
						if(playerOverRoof)
							glCullFace(GL_FRONT);
						else
							glCullFace(GL_BACK);
						aabb.CY = mRoofList[x][z].BoundingVolume.CY;
					}

					if(Frustum.sphereInFrustum(aabb))
						glCallList(mRoofList[x][z].iMeshId);
				}
			}
		}
	}

	//if(!Game.bPlayMode && Editor->GetMode() == EditorSpace::Terrain)
	//{
	//	RenderPoints();
	//}
}

void CQuadTree::RenderPoints()
{
	if(Mouse.mp3d.x < 0.0f || Mouse.mp3d.z < 0.0f)
		return;

	glPushMatrix();
		// This is how will our point sprite's size will be modified by 
		// distance from the viewer.
		float quadratic[] =  { 1.0f, 0.0f, 0.01f };
		glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

		// The alpha of a point is calculated to allow the fading of points 
		// instead of shrinking them past a defined threshold size. The threshold 
		// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
		// the minimum and maximum point sizes.
		glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);

		float m_fMaxPointSize = 0.0f;

		// Query for the max point size supported by the hardware
		glGetFloatv(GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize);

		glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
		glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, m_fMaxPointSize);

		// Specify point sprite texture coordinate replacement mode for each texture unit
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

		//
		// Render point sprites...
		//

		glDisable(GL_LIGHTING);
		glEnable(GL_POINT_SPRITE_ARB);

		glPointSize(50.0f);

		glColor3f(1.0f, 1.0f, 0.0f);

		int iQuadTreeX = int(Mouse.mp3d.x / mSizeX);
		int iQuadTreeZ = int(Mouse.mp3d.z / mSizeZ);

		int iVertexX = int((Mouse.mp3d.z - mFloorMesh[iQuadTreeX][iQuadTreeZ].Vertex.x) / 16);
		int iVertexZ = int((Mouse.mp3d.x - mFloorMesh[iQuadTreeX][iQuadTreeZ].Vertex.z) / 16);

		glBegin(GL_POINTS);
			for(int u = 0; u < 16; u += 1)
			{
				for(int v = 0; v < 16; v += 1)
				{
					if(u == iVertexX && v == iVertexZ)
						glColor3f(1.0f, 0.0f, 0.0f);
					else
						glColor3f(1.0f, 1.0f, 0.0f);

					glVertex3f(
						mFloorMesh[iQuadTreeX + u][iQuadTreeZ + v].Vertex.x,
						mFloorMesh[iQuadTreeX + u][iQuadTreeZ + v].Vertex.y + 0.1f,
						mFloorMesh[iQuadTreeX + u][iQuadTreeZ + v].Vertex.z
						);
				}
			}
		glEnd();

		glDisable(GL_POINT_SPRITE_ARB);
		glEnable(GL_LIGHTING);

	glPopMatrix();
}

void CQuadTree::SetTextureCoord(float x, float z)
{
	// Find the (u, v) coordinate for the current vertex
	float u = x / mHeight;
	float v = z / mWidth;

	// Give OpenGL the current terrain texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u * 4, v * 4);

	// Give OpenGL the current detail texture coordinate for our height map
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u * 4, v * 4);

	// Alpha map
	glMultiTexCoord2fARB(GL_TEXTURE2_ARB, u / 16, v / 16);

	// Shadow map
	glMultiTexCoord2fARB(GL_TEXTURE3_ARB, u * 4, v * 4);

	// Caustics
	glMultiTexCoord2fARB(GL_TEXTURE4_ARB, u, v);

}