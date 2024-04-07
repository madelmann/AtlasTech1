
// Library includes
#include <vector>

// Project includes
#include "EngineUtils.h"
#include "Entity.h"
#include "Tools.h"


CEntity::CEntity()
{
	BoundingVolumeType = BoundingVolume::Sphere;
	mCollisionVolumeType = BoundingVolume::Cylinder;
	fGamma = 0.0f;
	flastAnimTime = 0.0f;
	iLowestPoint = 0;
	Speed = 5.0f;
	VisibleVolumeType = BoundingVolume::Box;
}

CEntity::~CEntity()
{
	for(std::vector<t3DObject>::iterator it = Model.pObject.begin(); it != Model.pObject.end(); it += 1)
	{
		//delete [] it->pVerts;
		//delete [] it->pNormals;
		//delete [] it->pTexVerts;
		//delete [] it->pFaces;

		SAFE_DELETE_ARRAY ( it->pVerts );
		SAFE_DELETE_ARRAY ( it->pNormals );
		SAFE_DELETE_ARRAY ( it->pTexVerts );
		SAFE_DELETE_ARRAY ( it->pFaces );
	}

	Model.pObject.clear();
}

int CEntity::LowestPoint()
{
	int indexj = -1;

	int frame = Model.pAnimations[Model.currentAnim].startFrame;
	for(int j = 0; j < Model.pObject[frame].numOfVerts; j += 1)
	{
		if(indexj == -1)
			indexj = j;
		else
		{
			if(Model.pObject[frame].pVerts[j].y < Model.pObject[frame].pVerts[indexj].y)
				indexj = j;
		}
	}

	return indexj;
}

void CEntity::Render()
{
	if ( mShader ) {
		mShader->begin();

		if ( iNormalMap ) {
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D); 
			Texture_SetActiveID(iNormalMap);
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);

		int uniform = glGetUniformLocationARB(mShader->GetProgram(), "normalMap");
		glUniform1iARB(uniform, 0);

		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "colorMap"); 
		glUniform1iARB(uniform, 1);
	}

	Texture_SetActiveID(Model.pMaterials[0].texureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	RenderWithoutDisplayList();

	if ( mShader ) {
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);

		mShader->end();
	}
}

void CEntity::RenderWithoutDisplayList()
{
	// Make sure we have valid objects just in case. (size() is in the vector class)
	if(Model.pObject.size() <= 0)
		return;

	// Here we grab the current animation that we are on from our model's animation list
	tAnimationInfo pAnim = Model.pAnimations[Model.currentAnim];

	int nextFrame = (Model.currentFrame + 1) % pAnim.endFrame;

	// If the next frame is zero, that means that we need to start the animation over.
	// To do this, we set nextFrame to the starting frame of this animation.
	if(nextFrame == 0) 
		nextFrame =  pAnim.startFrame;

	// Get the current key frame we are on
	t3DObject pFrame = Model.pObject[Model.currentFrame];

	// Get the next key frame we are interpolating too
	t3DObject pNextFrame = Model.pObject[nextFrame];

	// Get the first key frame so we have an address to the texture and face information
	t3DObject pFirstFrame = Model.pObject[0];

	float t = ReturnCurrentTime(this, nextFrame);

	// Start rendering lines or triangles, depending on our current rendering mode (Lft Mouse Btn)
	glBegin(GL_TRIANGLES);
		// Go through all of the faces (polygons) of the current frame and draw them
		for(int j = 0; j < pFrame.numOfFaces; j += 1)
		{
			// Go through each corner of the triangle and draw it.
			for(int whichVertex = 0; whichVertex < 3; whichVertex += 1)
			{
				// Get the index for each point of the face
				int vertIndex = pFirstFrame.pFaces[j].vertIndex[whichVertex];

				// Get the index for each texture coordinate for this face
				int texIndex  = pFirstFrame.pFaces[j].coordIndex[whichVertex];
						
				// Make sure there was a UVW map applied to the object.  Notice that
				// we use the first frame to check if we have texture coordinates because
				// none of the other frames hold this information, just the first by design.
				if(pFirstFrame.pTexVerts) 
				{
					// Pass in the texture coordinate for this vertex
					glTexCoord2f(pFirstFrame.pTexVerts[ texIndex ].x, pFirstFrame.pTexVerts[ texIndex ].y);
				}
				
				// Store the current and next frame's vertex
				CModelVector3 vPoint1 = pFrame.pVerts[ vertIndex ];
				CModelVector3 vPoint2 = pNextFrame.pVerts[ vertIndex ];

				// By using the equation: p(t) = p0 + t(p1 - p0), with a time t
				// passed in, we create a new vertex that is closer to the next key frame.
				glVertex3f(vPoint1.x * fScale + t * (vPoint2.x - vPoint1.x) * fScale + fTranslationX, // Find the interpolated X
						   vPoint1.y * fScale + t * (vPoint2.y - vPoint1.y) * fScale + fTranslationY, // Find the interpolated Y
						   vPoint1.z * fScale + t * (vPoint2.z - vPoint1.z) * fScale + fTranslationZ);// Find the interpolated Z
			}
		}
	glEnd();
}

///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns time t for the interpolation between the current and next key frame
/////
///////////////////////////////// RETURN CURRENT TIME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float CEntity::ReturnCurrentTime(CEntity *pEntity, int nextFrame)
{
	static float elapsedTime   = 0.0f;

	// This function is very similar to finding the frames per second.
	// Instead of checking when we reach a second, we check if we reach
	// 1 second / our animation speed. (1000 ms / pEntity->Speed).
	// That's how we know when we need to switch to the next key frame.
	// In the process, we get the t value for how we are at to going to the
	// next animation key frame.  We use time to do the interpolation, that way
	// it runs the same speed on any persons computer, regardless of their specs.
	// It might look chopier on a junky computer, but the key frames still be
	// changing the same time as the other persons, it will just be not as smooth
	// of a transition between each frame.  The more frames per second we get, the
	// smoother the animation will be.

	// Get the current time in milliseconds
	float time = (float)GetTickCount();

	// Find the time that has elapsed since the last time that was stored
	elapsedTime = time - pEntity->flastAnimTime;

	// To find the current t we divide the elapsed time by the ratio of 1 second / our anim speed.
	// Since we aren't using 1 second as our t = 1, we need to divide the speed by 1000
	// milliseconds to get our new ratio, which is a 5th of a second.
	float t = elapsedTime / (1000.0f / pEntity->Speed);
	
	// If our elapsed time goes over a 5th of a second, we start over and go to the next key frame
	if (elapsedTime >= (1000.0f / pEntity->Speed) )
	{
		// Set our current frame to the next key frame (which could be the start of the anim)
		pEntity->Model.currentFrame = nextFrame;

		// Set our last time to the current time just like we would when getting our FPS.
		pEntity->flastAnimTime = time;
	}

	// Return the time t so we can plug this into our interpolation.
	return t;
}

void CEntity::SetAnimation(int animation)
{ 
	if(Model.currentAnim > Model.numOfAnimations - 1)
		Model.currentAnim = 0;
	else
		Model.currentAnim = animation;

	Model.currentFrame = Model.pAnimations[Model.currentAnim].startFrame;
}

void CEntity::SetBoundingBox()
{
	float coordXL = 0.0f;
	float coordXR = 0.0f;
	float coordYL = 0.0f;
	float coordYR = 0.0f;
	float coordZL = 0.0f;
	float coordZR = 0.0f;

	if((int)Model.pAnimations.capacity() <= 0)
		return;

	int frame = Model.pAnimations[Model.currentAnim].startFrame;

	coordXL = Model.pObject[frame].pVerts[0].x;
	coordXR = Model.pObject[frame].pVerts[0].x;
	coordYL = Model.pObject[frame].pVerts[0].y;
	coordYR = Model.pObject[frame].pVerts[0].y;
	coordZL = Model.pObject[frame].pVerts[0].z;
	coordZR = Model.pObject[frame].pVerts[0].z;
	
	for(int j = 1; j < Model.pObject[frame].numOfVerts; j += 1)
	{
		if(Model.pObject[frame].pVerts[j].x < coordXL)
		{
				coordXL = Model.pObject[frame].pVerts[j].x;
		}
		if(Model.pObject[frame].pVerts[j].x > coordXR)
		{
				coordXR = Model.pObject[frame].pVerts[j].x;
		}
		if(Model.pObject[frame].pVerts[j].y < coordYL)
		{
				coordYL = Model.pObject[frame].pVerts[j].y;
		}
		if(Model.pObject[frame].pVerts[j].y > coordYR)
		{
				coordYR = Model.pObject[frame].pVerts[j].y;
		}
		if(Model.pObject[frame].pVerts[j].z < coordZL)
		{
				coordZL = Model.pObject[frame].pVerts[j].z;
		}
		if(Model.pObject[frame].pVerts[j].z > coordZR)
		{
				coordZR = Model.pObject[frame].pVerts[j].z;
		}
	}

	BoundingVolume.X1 = coordXL;
	BoundingVolume.X2 = coordXR;
	BoundingVolume.CX = abs(BoundingVolume.X2 - BoundingVolume.X1) / 2 + BoundingVolume.X1;
	BoundingVolume.Y1 = coordYL + fTranslationY;
	BoundingVolume.Y2 = coordYR + fTranslationY;
	BoundingVolume.CY = abs(BoundingVolume.Y2 - BoundingVolume.Y1) / 2 + BoundingVolume.Y1 + fTranslationY;
	BoundingVolume.Z1 = coordZL;
	BoundingVolume.Z2 = coordZR;
	BoundingVolume.CZ = abs(BoundingVolume.Z2 - BoundingVolume.Z1) / 2 + BoundingVolume.Z1;

	BoundingVolume.X1 *= fScale;
	BoundingVolume.X2 *= fScale;
	BoundingVolume.CX *= fScale;
	BoundingVolume.Y1 *= fScale;
	BoundingVolume.Y2 *= fScale;
	BoundingVolume.CY *= fScale;
	BoundingVolume.Z1 *= fScale;
	BoundingVolume.Z2 *= fScale;
	BoundingVolume.CZ *= fScale;
	//fRadius *= fScale;

	BoundingVolume.X1 += fTranslationX;
	BoundingVolume.X2 += fTranslationX;
	BoundingVolume.CX += fTranslationX;
	BoundingVolume.Y1 += fTranslationY;
	BoundingVolume.Y2 += fTranslationY;
	BoundingVolume.CY += fTranslationY;
	BoundingVolume.Z1 += fTranslationZ;
	BoundingVolume.Z2 += fTranslationZ;
	BoundingVolume.CZ += fTranslationZ;

	float fRadiusX = BoundingVolume.X2 - BoundingVolume.CX;
	float fRadiusY = BoundingVolume.Y2 - BoundingVolume.CY;
	float fRadiusZ = BoundingVolume.Z2 - BoundingVolume.CZ;
	fRadius = sqrt(fRadiusX * fRadiusX + fRadiusY * fRadiusY + fRadiusZ * fRadiusZ);
}

void CEntity::Spawn(char name[255], float x, float y, float z, float scale)
{
	bSpawned = true;
	flastAnimTime = (float)GetTickCount();
	Model.currentAnim  = 0;
	Model.currentFrame = 0;
	setName(name);
	
	//SetBoundingBox();
	SetPosition(CVector3(x, y, z));

	fScale = scale;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


CEntityManager::CEntityManager()
{
	Count = 0;
	iActive = -1;
}

int CEntityManager::Add(char Filename[255], char Texname[255])
{
	if(Count >= MAX_MD2OBJECTS)
		return -1;

	if(strlen(Filename) <= 0)
		return -1;

	Filename = RemoveFileExt(Filename);

	int index = ObjectExists(Filename);

	char buffer[255] = "";

	iActive = Count;

	if(index != -1)
	{
		Object[Count] = Object[index];
		Object[Count].setId(Count);
		Object[Count].iType = EditorSpace::md2;
		Object[Count].Model.currentAnim = 0;
		Object[Count].Model.currentFrame = 0;
	}
	else
	{
		FILE *pFile;
		char fname[255] = "";

		sprintf(fname, "%sMaps\\%s\\Models\\%s\\%s.md2", g_sGameName, Game.sMapName, Filename, Filename);
		
		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			sprintf_s(fname, "%sModels\\%s\\%s.md2", g_sGameName, Filename, Filename);

			if(fopen_s(&pFile, fname, "rb") != NULL)
			{
				sprintf(buffer, "ERROR[Entity.Add]:File %s not found!\n", fname);
				Console->Output(buffer);

				return -1;
			}
		}
		fclose(pFile);

		sprintf_s(buffer, "%s.material", Filename);
		Object[Count].iMaterial = Material_Add(buffer);
		
		if(Object[Count].iMaterial == -1)
		{
			sprintf_s(buffer, "default.material");
			Object[Count].iMaterial = Material_Add(buffer);
		}

		if(strlen(Texname) == 0)
			sprintf(Object[Count].cTexture, "Models\\%s\\%s.bmp", Filename, Filename);
		else
			sprintf(Object[Count].cTexture, "Models\\%s\\%s.bmp", Filename, Texname);

		CLoadMD2 loadMd2;
		loadMd2.ImportMD2(&Object[Count].Model, fname, Object[Count].cTexture);

		Object[Count].setId(Count);
		Object[Count].iType = EditorSpace::md2;
		Object[Count].setName(Filename);

		sprintf_s(fname, "Models\\%s\\%s.cfg", Filename, Filename);
		g_Manager_Script.Execute(fname, false, true);

		Object[Count].SetBoundingBox();
	}

	for(int i = 0; i < Object[Count].Model.numOfMaterials; i += 1)
	{
		// Set the texture ID for this material
		Object[Count].Model.pMaterials[i].texureId = Texture_Add(Object[Count].Model.pMaterials[i].strFile);
	}

	Count++;

	return Count - 1;
}

void CEntityManager::Clear()
{
	if(Count <= 0)
		return;

	for(int i = Count - 1; i >= 0; i -= 1)
	{
		// do some unloading stuff...
		Object[i].bSpawned = false;
		Count--;
	}

	iActive = -1;
}

int CEntityManager::IndexOf(char* entityname)
{
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(strcmp(Object[i].getName(), entityname) == 0)
		{
			return i;
		}
	}

	return -1;
}

int CEntityManager::IndexOfID(long id)
{
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(Object[i].getId() == id)
		{
			return i;
		}
	}

	return -1;
}

int CEntityManager::ObjectExists(char* entityname)
{
	int index = g_Manager_Entity.IndexOf(entityname);

	if(index != -1)
	{
		return index;
	}

	return -1;
}