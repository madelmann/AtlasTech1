
#ifndef _3DSOBJECTMANAGER_H
#define _3DSOBJECTMANAGER_H


#include <vector>

#include "Main.h"
#include "3dsloader.h"



#define MAX_LODLEVELS	3


typedef struct
{
	char name[20];

	std::vector<vertex_type> normal;
	std::vector<vertex_type> vertex;
	std::vector<polygon_type> polygon;
	std::vector<mapcoord_type> mapcoord;

} Mesh3DS_t;


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
		bool ObjectExists(char ObjName[255]);

		//CMesh3DS	Object[MAX_3DSOBJECTS];
		std::vector<CMesh3DS*> Mesh;
};

#endif
