
#ifndef _3DSOBJECTMANAGER_H
#define _3DSOBJECTMANAGER_H


#include <vector>

#include "Main.h"



#define MAX_LODLEVELS	3

#define MAX_VERTICES 8000 // Max number of vertices (for each object)
#define MAX_POLYGONS 8000 // Max number of polygons (for each object)


// Our vertex type
typedef struct
{
    float x, y, z;
} vertex_type;


// The polygon (triangle), 3 numbers that aim 3 vertices
typedef struct
{
    unsigned short a, b, c;
} polygon_type;


// The mapcoord type, 2 texture coordinates for each vertex
typedef struct
{
    float u, v;
} mapcoord_type;


// The object type
typedef struct
{
	char name[20];
    
	int vertices_qty;
    int polygons_qty;

	vertex_type normal[MAX_VERTICES];
    vertex_type vertex[MAX_VERTICES]; 
    polygon_type polygon[MAX_POLYGONS];
    mapcoord_type mapcoord[MAX_VERTICES];
    int id_texture;
} obj_type, *obj_type_ptr;


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
