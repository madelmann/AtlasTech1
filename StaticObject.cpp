
// Library includes

// Project includes
#include "Editor.h"
#include "EngineUtils.h"
#include "GLWrapper.h"
#include "Normals.h"
#include "StaticObject.h"


#define FILENAMELENGTH	13


CStaticObject::CStaticObject()
{
	BoundingVolumeType = BoundingVolume::Box;
	mCollisionVolumeType = BoundingVolume::Box;
	iLowestPoint =	0;
}

CStaticObject::~CStaticObject()
{
}

void CStaticObject::CalcVertexNormals()
{
	CVector3 vektorarray[3];
	CVector3 normalvector;
	 
	for(int i = 0; i < Model.numtriangles; i += 1)
	{
		normalvector.x = 0.0f;
		normalvector.y = 0.0f;
		normalvector.z = 0.0f;

		vektorarray[0].x = Model.triangle[i].vertex[0].x;
		vektorarray[0].y = Model.triangle[i].vertex[0].y;
		vektorarray[0].z = Model.triangle[i].vertex[0].z;

		vektorarray[1].x = Model.triangle[i].vertex[1].x;
		vektorarray[1].y = Model.triangle[i].vertex[1].y;
		vektorarray[1].z = Model.triangle[i].vertex[1].z;

		vektorarray[2].x = Model.triangle[i].vertex[2].x;
		vektorarray[2].y = Model.triangle[i].vertex[2].y;
		vektorarray[2].z = Model.triangle[i].vertex[2].z;

		GetNormal(vektorarray, &normalvector);

		normalvector = Normalize(normalvector);

		Model.triangle[i].normal.x = normalvector.x;
		Model.triangle[i].normal.y = normalvector.y;
		Model.triangle[i].normal.z = normalvector.z;
	}
}

int CStaticObject::DeepCollision(CVector3 vColPos, float radius)
{
	int result = 0;
	double fr = 0.0f;
	double c;

	for(int i = Model.numtriangles - 1; i >= 0; i -= 1)
	{
		c = -(Model.triangle[i].normal.x * (Model.triangle[i].vertex[0].x + mPosition.x) + Model.triangle[i].normal.y * (Model.triangle[i].vertex[0].y + mPosition.y) + Model.triangle[i].normal.z * (Model.triangle[i].vertex[0].z + mPosition.z));
		fr = CVector3::dotProduct(CVector3(Model.triangle[i].normal.x, Model.triangle[i].normal.y, Model.triangle[i].normal.z), CVector3(vColPos.x, vColPos.y, vColPos.z)) + c;
		
		if(fr > -radius && fr <= radius)
			result += 1;
	}

	return result;
}

int CStaticObject::LowestPoint()
{
	int indexj = -1;

	for(int j = 0; j < Model.numtriangles; j += 1)
	{
		if(indexj == -1)
		{
			indexj = j;
		}
		else
		{
			if(Model.triangle[j].vertex[0].y < Model.triangle[indexj].vertex[0].y)
			{
				indexj = j;
			}
		}
	}

	return indexj;
}

void CStaticObject::PreProcess()
{
	// Destroy display list if already exists.
	if( glIsList(iMeshID) )
	{
		glDeleteLists(iMeshID, 1);
	}
	
	// Ask OpenGL for a new display list number
	iMeshID = glGenLists(1);
	if( iMeshID == 0 )
	{
		//TODO : throw an exception
		return;
	}
	
	glNewList( iMeshID, GL_COMPILE );

	RenderWithoutDisplayList();

	glEndList();
}

void CStaticObject::Render()
{
	if(iMeshID)
		glCallList(iMeshID);
	else
		RenderWithoutDisplayList();
}

void CStaticObject::RenderWithoutDisplayList()
{
	glDisable(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Process Each Triangle
	for(int loop_m = 0; loop_m < Model.numtriangles; loop_m += 1)
	{
		Material_Apply(Model.triangle[loop_m].texindex, false);

		glBegin(GL_TRIANGLES);
			glNormal3f(Model.triangle[loop_m].normal.x, Model.triangle[loop_m].normal.y, Model.triangle[loop_m].normal.z);

			glTexCoord2f(Model.triangle[loop_m].vertex[0].u, Model.triangle[loop_m].vertex[0].v);
			glVertex3f(Model.triangle[loop_m].vertex[0].x, Model.triangle[loop_m].vertex[0].y, Model.triangle[loop_m].vertex[0].z);
			
			glTexCoord2f(Model.triangle[loop_m].vertex[1].u, Model.triangle[loop_m].vertex[1].v);
			glVertex3f(Model.triangle[loop_m].vertex[1].x, Model.triangle[loop_m].vertex[1].y, Model.triangle[loop_m].vertex[1].z);
			
			glTexCoord2f(Model.triangle[loop_m].vertex[2].u, Model.triangle[loop_m].vertex[2].v);
			glVertex3f(Model.triangle[loop_m].vertex[2].x, Model.triangle[loop_m].vertex[2].y, Model.triangle[loop_m].vertex[2].z);
		glEnd();
	}

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, Engine->bLightModelTwoSided);
	glEnable(GL_CULL_FACE);
}

void CStaticObject::SetBoundingBox()
{
	BoundingVolume.X1 = 0.0f;
	BoundingVolume.Y1 = 0.0f;
	BoundingVolume.Y1 = 0.0f;
	BoundingVolume.X2 = 0.0f;
	BoundingVolume.Y2 = 0.0f;
	BoundingVolume.Y2 = 0.0f;
	BoundingVolume.CX = 0.0f;
	BoundingVolume.CY = 0.0f;
	BoundingVolume.CZ = 0.0f;

	for(int t = Model.numtriangles - 1; t >= 0; t -= 1)
	{
		for(int v = 2; v >= 0; v -= 1)
		{
			if(Model.triangle[t].vertex[v].x < BoundingVolume.X1)
				BoundingVolume.X1 = Model.triangle[t].vertex[v].x;
			if(Model.triangle[t].vertex[v].x > BoundingVolume.X2)
				BoundingVolume.X2 = Model.triangle[t].vertex[v].x;

			if(Model.triangle[t].vertex[v].y < BoundingVolume.Y1)
				BoundingVolume.Y1 = Model.triangle[t].vertex[v].y;
			if(Model.triangle[t].vertex[v].y > BoundingVolume.Y2)
				BoundingVolume.Y2 = Model.triangle[t].vertex[v].y;

			if(Model.triangle[t].vertex[v].z < BoundingVolume.Z1)
				BoundingVolume.Z1 = Model.triangle[t].vertex[v].z;
			if(Model.triangle[t].vertex[v].z > BoundingVolume.Z2)
				BoundingVolume.Z2 = Model.triangle[t].vertex[v].z;
		}
	}

	BoundingVolume.X1 *= fScale;
	BoundingVolume.Y1 *= fScale;
	BoundingVolume.Y1 *= fScale;
	BoundingVolume.X2 *= fScale;
	BoundingVolume.Y2 *= fScale;
	BoundingVolume.Y2 *= fScale;
	
	BoundingVolume.CX = abs(BoundingVolume.X2 - BoundingVolume.X1) / 2 + BoundingVolume.X1;
	BoundingVolume.CY = abs(BoundingVolume.Y2 - BoundingVolume.Y1) / 2 + BoundingVolume.Y1;
	BoundingVolume.CZ = abs(BoundingVolume.Z2 - BoundingVolume.Z1) / 2 + BoundingVolume.Z1;

	float fRadiusX = BoundingVolume.X2 - BoundingVolume.CX;
	float fRadiusY = BoundingVolume.Y2 - BoundingVolume.CY;
	float fRadiusZ = BoundingVolume.Z2 - BoundingVolume.CZ;
	fRadius = sqrt(fRadiusX * fRadiusX + fRadiusY * fRadiusY + fRadiusZ * fRadiusZ);
}

void CStaticObject::SetSelectionIndicator()
{
	int indexj = -1;

	for(int j = 0; j < Model.numtriangles; j += 1)
	{
		if(indexj == -1)
		{
			indexj = j;
		}
		else
		{
			if(Model.triangle[j].vertex[0].y > Model.triangle[indexj].vertex[0].y)
			{
				indexj = j;
			}
		}
	}

	vSelectionIndicator.x = Model.triangle[indexj].vertex[0].x + mPosition.x;
	vSelectionIndicator.y = Model.triangle[indexj].vertex[0].y + mPosition.y;
	vSelectionIndicator.z = Model.triangle[indexj].vertex[0].z + mPosition.z;
}

void CStaticObject::Spawn(char* name, float x, float y, float z, float scale)
{
	bSpawned	 = true;
	iLowestPoint = LowestPoint();
	setName(name);
	
	SetPosition(CVector3(x, y, z));

	fScale	= scale;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CStaticObjectManager::CStaticObjectManager()
{
	Count = 0;
	iActive = -1;
}

CStaticObjectManager::~CStaticObjectManager()
{
}

int CStaticObjectManager::Add(char Filename[255])
{
	if(strlen(Filename) <= 0)
		return -1;

	//Filename = RemoveFileExt(Filename);

	int index = ObjectExists(Filename);

	if(index != -1)
	{
		Object[Count] = Object[index];
		Object[Count].bDoCollision = true;
		Object[Count].setId(Count);
		Object[Count].iType = EditorSpace::Static;
	}
	else
	{
		float x, y, z, u, v;
		int numtriangles;
		FILE *pFile;
		char oneline[255];
		char texname[FILENAMELENGTH];
		char fulltexname[255];

		char fname[255] = "";
		sprintf(fname, "%sMaps\\%s\\Static\\%s", g_sGameName, Game.sMapName, Filename);
		
		if(fopen_s(&pFile, fname, "rt") != NULL)
		{
			sprintf(fname, "%sStatic\\%s", g_sGameName, Filename);

			if(fopen_s(&pFile, fname, "rt") != NULL)
			{
				char buffer[255] = "";
				sprintf(buffer, "File %s not found!", fname);
				Console->Output(buffer);

				return -1;
			}
		}

		readstr(pFile, oneline);
		sscanf_s(oneline, "NUMPOLLIES %d\n", &numtriangles);

		Object[Count].Model.triangle = new TRIANGLE[numtriangles];
		Object[Count].Model.numtriangles = numtriangles;

		for (int loop = 0; loop < Object[Count].Model.numtriangles; loop += 1)
		{
			for (int vert = 0; vert < 3; vert += 1)
			{
				readline(pFile, oneline);
				sscanf(oneline, "%f %f %f %f %f %s", &x, &y, &z, &u, &v, texname);
				Object[Count].Model.triangle[loop].vertex[vert].x = x;
				Object[Count].Model.triangle[loop].vertex[vert].y = y;
				Object[Count].Model.triangle[loop].vertex[vert].z = z;
				Object[Count].Model.triangle[loop].vertex[vert].u = u;
				Object[Count].Model.triangle[loop].vertex[vert].v = v;

				sprintf(fulltexname, "%s", texname);
				
				// Material
				if(strlen(fulltexname) > 0)
				{
					Object[Count].Model.triangle[loop].texindex = Material_Add(fulltexname);
				}
				else
				{
					Object[Count].Model.triangle[loop].texindex = -1;
				}

				//if(strlen(fulltexname) > 0)
				//{
				//	if(g_Manager_Texture->IndexOf(fulltexname) == -1)
				//	{
				//		g_Manager_Texture->CreateBMPTexture(fulltexname);
				//	}

				//	Object[Count].Model.triangle[loop].texindex = g_Manager_Texture->IndexOf(fulltexname);
				//}
				//else
				//{
				//	Object[Count].Model.triangle[loop].texindex = -1;
				//}
			}
		}

		fclose(pFile);
	}

	if(index == -1)
	{
		Object[Count].setId(Count);
		Object[Count].iType = EditorSpace::Static;
		Object[Count].setName(Filename);
		Object[Count].CalcVertexNormals();
		Object[Count].PreProcess();
		Object[Count].SetBoundingBox();
	}

	iActive = Count;
	Count++;

	return Count - 1;
}

void CStaticObjectManager::Clear()
{
	if(Count <= 0)
		return;

	for(int i = Count - 1; i >= 0; i -= 1)
	{
		Object[i].bSpawned = false;
		Count--;
	}
}

int CStaticObjectManager::IndexOf(char* objectname)
{
	for(int i = 0; i < Count; i += 1)
	//for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(strcmp(Object[i].getName(), objectname) == 0)
		{
			return i;
		}
	}

	return -1;
}

int CStaticObjectManager::IndexOfID(long id)
{
	for(int i = 0; i < Count; i += 1)
	//for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(Object[i].getId() == id)
		{
			return i;
		}
	}

	return -1;
}

int CStaticObjectManager::ObjectExists(char* objectname)
{
	return IndexOf(objectname);
}