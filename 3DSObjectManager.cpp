
#include "Main.h"
#include "3dsloader.h"


CMesh3DS::CMesh3DS()
: CGraphicObject()
{
	BoundingVolumeType = BoundingVolume::Sphere;
	mCollisionVolumeType = BoundingVolume::Box;
	VisibleVolumeType = BoundingVolume::Box;
}

void CMesh3DS::CalcVertexNormals()
{
	CVector3 vectorarray[3];
	CVector3 normalvector;
	vertex_type vertex;
	polygon_type polygon;

	try {
		for ( int lod = 0; lod < MAX_LODLEVELS; lod += 1 ) {
			for ( unsigned int vertIt = 0; vertIt < Mesh[ lod ].vertex.size(); vertIt += 1 ) {

				Mesh[ lod ].normal[ vertIt ].x = 0.0f;
				Mesh[ lod ].normal[ vertIt ].y = 0.0f;
				Mesh[ lod ].normal[ vertIt ].z = 0.0f;

				for ( unsigned int polyIt = 0; polyIt < Mesh[ lod ].polygon.size(); polyIt += 1 ) {
					vertex = Mesh[ lod ].vertex[ vertIt ];
					polygon = Mesh[ lod ].polygon[ polyIt ];

					if ( polygon.a < 0 || polygon.b < 0 || polygon.c < 0 ) {
						continue;
					}

					if(
					  (vertex.x == Mesh[ lod ].vertex[ polygon.a ].x
					&& vertex.y == Mesh[ lod ].vertex[ polygon.a ].y
					&& vertex.z == Mesh[ lod ].vertex[ polygon.a ].z ) 
					||(vertex.x == Mesh[ lod ].vertex[ polygon.b ].x
					&& vertex.y == Mesh[ lod ].vertex[ polygon.b ].y
					&& vertex.z == Mesh[ lod ].vertex[ polygon.b ].z )
					||(vertex.x == Mesh[ lod ].vertex[ polygon.c ].x
					&& vertex.y == Mesh[ lod ].vertex[ polygon.c ].y
					&& vertex.z == Mesh[ lod ].vertex[ polygon.c ].z ))
					{
						normalvector.x = 0.0f;
						normalvector.y = 0.0f;
						normalvector.z = 0.0f;

						vectorarray[0].x = Mesh[ lod ].vertex[ polygon.a ].x;
						vectorarray[0].y = Mesh[ lod ].vertex[ polygon.a ].y;
						vectorarray[0].z = Mesh[ lod ].vertex[ polygon.a ].z;

						vectorarray[1].x = Mesh[ lod ].vertex[ polygon.b ].x;
						vectorarray[1].y = Mesh[ lod ].vertex[ polygon.b ].y;
						vectorarray[1].z = Mesh[ lod ].vertex[ polygon.b ].z;

						vectorarray[2].x = Mesh[ lod ].vertex[ polygon.c ].x;
						vectorarray[2].y = Mesh[ lod ].vertex[ polygon.c ].y;
						vectorarray[2].z = Mesh[ lod ].vertex[ polygon.c ].z;

						GetNormal(vectorarray, &normalvector);

						Mesh[ lod ].normal[ vertIt ].x += normalvector.x;
						Mesh[ lod ].normal[ vertIt ].y += normalvector.y;
						Mesh[ lod ].normal[ vertIt ].z += normalvector.z;
				   }
				}

				normalvector.x = Mesh[ lod ].normal[ vertIt ].x;
				normalvector.y = Mesh[ lod ].normal[ vertIt ].y;
				normalvector.z = Mesh[ lod ].normal[ vertIt ].z;

				normalvector = Normalize(normalvector);

				Mesh[ lod ].normal[ vertIt ].x = normalvector.x;
				Mesh[ lod ].normal[ vertIt ].y = normalvector.y;
				Mesh[ lod ].normal[ vertIt ].z = normalvector.z;
			}
		}
	}
	catch (exception& e) {
		throw e.what();
	}
}

void CMesh3DS::createMesh(const obj_type &model)
{
	//for ( int lod = 0; lod < MAX_LODLEVELS; lod += 1 ) {

		sprintf(Mesh[0].name, "%s", model.name);

		for ( int vertIt = 0; vertIt < model.vertices_qty; vertIt += 1) {
			Mesh[0].mapcoord.push_back(model.mapcoord[ vertIt ]);
		}
		for ( int vertIt = 0; vertIt < model.vertices_qty; vertIt += 1) {
			Mesh[0].normal.push_back(model.normal[ vertIt ]);
		}
		for ( int polyIt = 0; polyIt < model.polygons_qty; polyIt += 1) {
			Mesh[0].polygon.push_back(model.polygon[ polyIt ]);
		}
		for ( int vertIt = 0; vertIt < model.vertices_qty; vertIt += 1) {
			Mesh[0].vertex.push_back(model.vertex[ vertIt ]);
		}
	//}
}

void CMesh3DS::PreProcess()
{
	createMesh(Model[0]);
	CalcVertexNormals();
	SetBoundingBox();

	// Destroy display list if already exists.
	if(glIsList(iMeshID))
		glDeleteLists(iMeshID, 1);
	
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

	
	// LOD...
	gluivMeshID_LOD[0] = iMeshID;

	for(int i = 1; i < Engine->iMaxLODLevel; i += 1)
	{
		// Destroy display list if already exists.
		if(glIsList(gluivMeshID_LOD[i]))
			glDeleteLists(gluivMeshID_LOD[i], 1);

		if(Model[i].polygons_qty <= 0)
			continue;
		
		// Ask OpenGL for a new display list number
		gluivMeshID_LOD[i] = glGenLists(1);

		if(gluivMeshID_LOD[i] == 0)
		{
			//TODO : throw an exception
			return;
		}

		glNewList(gluivMeshID_LOD[i], GL_COMPILE);

			RenderWithoutDisplayList(i);

		glEndList();
	}
}

void CMesh3DS::Render()
{
	GLuint gluiList = iMeshID;
	
	if(Game.bPlayMode)															// only apply LOD in game mode
		gluiList = gluivMeshID_LOD[GetLODLevel((CGraphicObject*)this)];
	
	if(!glIsList(gluiList))
		gluiList = iMeshID;
	if(!glIsList(gluiList))
		RenderWithoutDisplayList();
	else
		glCallList(gluiList);
}

void CMesh3DS::RenderWithoutDisplayList()
{
	RenderWithoutDisplayList(0);
}

void CMesh3DS::RenderWithoutDisplayList(int iLODLevel)
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
	}

	if(!g_Manager_Material.Materials[iMaterial].bEnvironmentalMapping)
	{
		if(strcmpi(g_Manager_Material.Materials[iMaterial].Texture, "none") != 0)
			Texture_SetActive(g_Manager_Material.Materials[iMaterial].Texture);
		else
			Texture_SetActiveID(iTexture);
	}

	if ( mShader ) {
		int uniform = glGetUniformLocationARB(Shader->GetProgram(), "colorMap"); 
		glUniform1iARB(uniform, 1);
	}

	glBegin(GL_TRIANGLES);
		for ( unsigned int polyIt = 0; polyIt < Mesh[iLODLevel].polygon.size(); polyIt += 1 )
		{
			//----------------- FIRST VERTEX -----------------
			glNormal3f( Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].a ].x * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].a ].y * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].a ].z * fScale); //normal definition
			// Texture coordinates of the first vertex
			glTexCoord2f( Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].a ].u,
						  Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].a ].v);

			glVertex3f( Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].a ].x * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].a ].y * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].a ].z * fScale); //Vertex definition

			//----------------- SECOND VERTEX -----------------
			glNormal3f( Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].b ].x * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].b ].y * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].b ].z * fScale); //normal definition
			// Texture coordinates of the second vertex
			glTexCoord2f( Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].b ].u,
						  Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].b ].v);
			// Coordinates of the second vertex
			glVertex3f( Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].b ].x * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].b ].y * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].b ].z * fScale); //Vertex definition

			//----------------- THIRD VERTEX -----------------
			glNormal3f( Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].c ].x * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].c ].y * fScale,
						Mesh[ iLODLevel ].normal[ Mesh[ iLODLevel ].polygon[polyIt].c ].z * fScale); //normal definition
			// Texture coordinates of the third vertex
			glTexCoord2f( Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].c ].u,
						  Mesh[ iLODLevel ].mapcoord[ Mesh[ iLODLevel ].polygon[polyIt].c ].v);
			// Coordinates of the Third vertex
			glVertex3f( Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].c ].x * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].c ].y * fScale,
						Mesh[ iLODLevel ].vertex[ Mesh[ iLODLevel ].polygon[polyIt].c ].z * fScale); //Vertex definition

		}
	glEnd();

	if ( mShader ) {
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);

		mShader->end();
	}
}

void CMesh3DS::SetBoundingBox()
{
	float coordXL = 0.0f;
	float coordXR = 0.0f;
	float coordYL = 0.0f;
	float coordYR = 0.0f;
	float coordZL = 0.0f;
	float coordZR = 0.0f;

	coordXL = Mesh[0].vertex[0].x;
	coordXR = Mesh[0].vertex[0].x;
	coordYL = Mesh[0].vertex[0].y;
	coordYR = Mesh[0].vertex[0].y;
	coordZL = Mesh[0].vertex[0].z;
	coordZR = Mesh[0].vertex[0].z;

	for ( unsigned int i = 1; i < Mesh[0].vertex.size(); i += 1 )
	{
		if(Mesh[0].vertex[i].x < coordXL)
		{
				coordXL = Mesh[0].vertex[i].x;
		}
		if(Mesh[0].vertex[i].x > coordXR)
		{
				coordXR = Mesh[0].vertex[i].x;
		}
		if(Mesh[0].vertex[i].y < coordYL)
		{
				coordYL = Mesh[0].vertex[i].y;
		}
		if(Mesh[0].vertex[i].y > coordYR)
		{
				coordYR = Mesh[0].vertex[i].y;
		}
		if(Mesh[0].vertex[i].z < coordZL)
		{
				coordZL = Mesh[0].vertex[i].z;
		}
		if(Mesh[0].vertex[i].z > coordZR)
		{
				coordZR = Mesh[0].vertex[i].z;
		}
	}

	BoundingVolume.X1 = coordXL * fScale;
	BoundingVolume.X2 = coordXR * fScale;
	BoundingVolume.Y1 = coordYL * fScale;
	BoundingVolume.Y2 = coordYR * fScale;
	BoundingVolume.Z1 = coordZL * fScale;
	BoundingVolume.Z2 = coordZR * fScale;
	BoundingVolume.CX = abs(BoundingVolume.X2 - BoundingVolume.X1) / 2 + BoundingVolume.X1;
	BoundingVolume.CY = abs(BoundingVolume.Y2 - BoundingVolume.Y1) / 2 + BoundingVolume.Y1;
	BoundingVolume.CZ = abs(BoundingVolume.Z2 - BoundingVolume.Z1) / 2 + BoundingVolume.Z1;

	float fRadiusX = BoundingVolume.X2 - BoundingVolume.CX;
	float fRadiusY = BoundingVolume.Y2 - BoundingVolume.CY;
	float fRadiusZ = BoundingVolume.Z2 - BoundingVolume.CZ;
	fRadius = sqrt(fRadiusX * fRadiusX + fRadiusY * fRadiusY + fRadiusZ * fRadiusZ);
}

void CMesh3DS::Spawn(char* name, float x, float y, float z, float scale)
{
	fRadius *= scale;

	bSpawned = true;
	setName(name);
	
	SetPosition(CVector3(x, y, z));

	fScale = scale;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


C3DSObjectManager::C3DSObjectManager()
{
}

C3DSObjectManager::~C3DSObjectManager()
{
	Clear();
}

int C3DSObjectManager::Add(char *Filename)
{
	if(strlen(Filename) <= 0)
		return -1;

	Filename = RemoveFileExt(Filename);

	int index = IndexOf(Filename);
	if(index != -1) {
		return index;
	}


	FILE *pFile;
	char fname[255] = "";
	char buffer[255] = "";

	sprintf(fname, "%sMaps\\%s\\Objects\\%s.3ds", g_sGameName, Game.sMapName, Filename);
	
	if(fopen_s(&pFile, fname, "rb") != NULL)
	{
		sprintf(fname, "%sObjects\\%s.3ds", g_sGameName, Filename);

		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			sprintf(buffer, "ERROR[C3DSObjectManager.Add]: '%s' not found!", fname);
			Console->Output(buffer);

			return -1;
		}
	}
	fclose(pFile);


	Mesh.push_back(new CMesh3DS());

	Load3DS(&Mesh.back()->Model[0], fname);

	sprintf_s(buffer, "%s.material", Filename);
	Mesh.back()->iMaterial = Material_Add(buffer);
	if(Mesh.back()->iMaterial != -1)
		Mesh.back()->iTexture = g_Manager_Material.Materials[Mesh.back()->iMaterial].iTexture;

	//if ( Mesh.back()->iNormalMap == 0 )
	//{
	//	sprintf_s(buffer, "%s.jpg", Filename);
	//	Mesh.back()->iNormalMap = Texture_Add(buffer);
	//}

	if(Mesh.back()->iMaterial == -1)
	{
		sprintf_s(buffer, "default.material");
		Mesh.back()->iMaterial = Material_Add(buffer);

		sprintf(Mesh.back()->cTexture, "%s", Filename);

		sprintf_s(buffer, "%s.bmp", Filename);

		Mesh.back()->iTexture = Texture_Add(buffer);
	}

	//if(Mesh.back()->iTexture == 0)
	//{
	//	sprintf(Mesh.back()->cTexture, "%s", Filename);

	//	sprintf_s(buffer, "%s.bmp", Filename);

	//	Mesh.back()->iTexture = Texture_Add(buffer);
	//}

	// Load LOD models
	bool err = false;

	for(int i = 1; i < Engine->iMaxLODLevel; i += 1)
	{
		err = false;

		Mesh.back()->Model[i] = obj_type();

		sprintf(fname, "%sMaps\\%s\\Objects\\%s_LOD_%d.3ds", g_sGameName, Game.sMapName, Filename, i);
	
		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			sprintf(fname, "%sObjects\\%s_LOD_%d.3ds", g_sGameName, Filename, i);

			if(fopen_s(&pFile, fname, "rb") != NULL)
			{
				sprintf(buffer, "WARNING[C3DSObjectManager.Add]: '%s' not found!", fname);
				Console->Output(buffer);

				err = true;
			}
		}

		if(!err) {
			fclose(pFile);

			Load3DS(&Mesh.back()->Model[i], fname);
		}
	}

	Mesh.back()->setId(Mesh.size());
	Mesh.back()->iType = EditorSpace::max3ds;
	Mesh.back()->setName(Filename);
	Mesh.back()->PreProcess();

	return Mesh.size() - 1;
}

void C3DSObjectManager::Clear()
{
	std::vector<CMesh3DS*>::iterator it;
	for ( it = Mesh.begin(); it != Mesh.end(); it += 1 ) {
		delete (*it);
	}

	Mesh.clear();
}

inline int C3DSObjectManager::IndexOf(char* objectname)
{
	objectname = RemoveFileExt(objectname);

	for(unsigned int i = 0; i < Mesh.size(); i += 1)
	{
		if(strcmp(Mesh[i]->getName(), objectname) == 0)
		{
			return i;
		}
	}

	return -1;
}

int C3DSObjectManager::IndexOfId(int id)
{
	for(unsigned int i = 0; i < Mesh.size(); i += 1)
	{
		if(Mesh[i]->getId() == id)
		{
			return i;
		}
	}

	return -1;
}

bool C3DSObjectManager::ObjectExists(char ObjName[255])
{
	int index = IndexOf(ObjName);

	if(index != -1)
	{
		return true;
	}

	return false;
}