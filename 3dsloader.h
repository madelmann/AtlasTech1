
#ifndef _3DSLOADER_H
#define _3DSLOADER_H


// Library includes
#include <vector>

// Project includes


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
} obj_type, * obj_type_ptr;


typedef struct
{
    char name[20];

    std::vector<vertex_type> normal;
    std::vector<vertex_type> vertex;
    std::vector<polygon_type> polygon;
    std::vector<mapcoord_type> mapcoord;

} Mesh3DS_t;



/**********************************************************
 *
 * FUNCTION Load3DS (obj_type_ptr, char *)
 *
 * This function loads a mesh from a 3ds file.
 * Please note that we are loading only the vertices, polygons and mapping lists.
 * If you need to load meshes with advanced features as for example: 
 * multi objects, materials, lights and so on, you must insert other chunk parsers.
 *
 *********************************************************/

extern char Load3DS(obj_type *p_object, char *p_filename);


#endif