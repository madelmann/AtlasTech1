
#pragma once

// Library includes

// Project includes
#include "Normals.h"
#include "Tools.h"


#define MAP_SIZE		256				// This is the size of our .raw height map
#define STEP_SIZE		1				// This is width and height of each triangle grid


float Magnitude2(CVector3 vNormal);

CVector3 Normalize2(CVector3 vVector);

void GetNormal(CVector3 v[3], CVector3 *normal);

bool VectorEqual(CVector3 a, CVector3 b);

// This returns the height (0 to 255) from a heightmap given an X and Y
int Height(BYTE *pHeightMap, int X, int Y);

//void CalculateFaceNormals(BYTE *pHeightMap, float posx, float posy, float posz);

void LoadNormalFile(LPSTR strName, int nSize);

// This loads a .raw file of a certain size from the file
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);

// This saves a .raw file of a certain size to the file
void SaveRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);

// This turns heightmap data into primitives and draws them to the screen
void RenderHeightMapXY(BYTE *pHeightMap, float posx, float posy, float posz);

void ModifyHeightMap(BYTE *pHeightMap, int X, int Y, int Delta);

