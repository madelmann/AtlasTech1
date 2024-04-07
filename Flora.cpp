
// Library includes

// Project includes
#include "Flora.h"
#include "Globals.h"
#include "GLWrapper.h"
#include "QuadTree.h"
#include "Texture.h"
#include "Tools.h"


CFlora::CFlora()
{
	fRadius = 0.0f;
	fScale = 1.0f;
	ID = -1;
	iMeshID = -1;
	iTexture = -1;
	vPosition = CVector3(0.0f, 0.0f, 0.0f);
}

void CFlora::PreProcess()
{
	// Destroy display list if already exists.
	if(glIsList(iMeshID))
	{
		glDeleteLists(iMeshID, 1);
	}
	
	// Ask OpenGL for a new display list number
	iMeshID = glGenLists(1);
	if(iMeshID == 0)
	{
		//TODO : throw an exception
		return;
	}
	
	glNewList(iMeshID, GL_COMPILE);

		RenderWithoutDisplayList();

	glEndList();

	return;
}

void CFlora::Render()
{
	glCallList(iMeshID);
}

void CFlora::RenderWithoutDisplayList()
{
	g_Manager_Texture->SetActiveTextureID(iTexture);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(vPosition.x - 2.0f, vPosition.y, vPosition.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(vPosition.x + 2.0f, vPosition.y, vPosition.z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(vPosition.x + 2.0f, vPosition.y + 4.0f, vPosition.z);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(vPosition.x - 2.0f, vPosition.y + 4.0f, vPosition.z);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(vPosition.x, vPosition.y, vPosition.z - 2.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(vPosition.x, vPosition.y, vPosition.z + 2.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(vPosition.x, vPosition.y + 4.0f, vPosition.z + 2.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(vPosition.x, vPosition.y + 4.0f, vPosition.z - 2.0f);
	glEnd();
}

CFloraManager::CFloraManager()
{
	Count = 0;
	iMax = 0;
}

CFloraManager::~CFloraManager()
{
}

int CFloraManager::Add(float x, float z, char texturename[255])
{
	Flora[Count].iTexture = g_Manager_Texture->CreateTexture(texturename);
	Flora[Count].ID = Count;
	Flora[Count].vPosition = CVector3(x, QuadTree.getHeight(x, z), z);
	Flora[Count].PreProcess();

	Count++;

	return -1;
}

int CFloraManager::Delete(int index)
{
	return -1;
}

void CFloraManager::Render()
{
	if(Count <= 0)
		return;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.3f);
	glDisable(GL_CULL_FACE);

	for(unsigned int i = Count - 1; i > 0; i -= 1)
	{
		Flora[i].Render();
	}

	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
}

void CFloraManager::Spread()
{
	int x = -1;
	int z = -1;

	if(Count >= FLORA_MAX)
		return;

	srand(1);

	bool bPlaceOkay;

	for(int i = 0; i < FLORA_MAX; i += 1)
	{
		bPlaceOkay = false;

		//do
		//{
			do
			{
				x = (int)((double)rand() / (RAND_MAX + 1) * 512);
			}
			while(x < 0 && x > 512);

			do
			{
				z = (int)((double)rand() / (RAND_MAX + 1) * 512);
			}
			while(z < 0 && z < 512);

		//}
		//while(!bPlaceOkay);

		Add(float(x - 2048), float(z - 2048), "Grass.tga");
	}
}