
#pragma once

// Library includes
#include <vector>

// Project includes
#include "3dsloader.h"
#include "Object.h"


class CMesh3DS : public CGraphicObject
{
public:
	CMesh3DS();

	void PreProcess();
	void Render();
	void RenderWithoutDisplayList();
	void RenderWithoutDisplayList(int iLODLevel);
	void SetBoundingBox();
	void Spawn(char* name, float x, float y, float z, float scale);

	obj_type	Model[MAX_LODLEVELS];

private:
	void CalcVertexNormals();
	void createMesh(const obj_type &model);

	Mesh3DS_t	Mesh[MAX_LODLEVELS];
};

class C3DSObjectManager
{
public:
		C3DSObjectManager();
		~C3DSObjectManager();

		int Add(char *Filename);
		void Clear();
		inline int IndexOf(char* objectname);
		int IndexOfId(int id);
		bool ObjectExists(char* objectname);

		std::vector<CMesh3DS*> Mesh;
};
