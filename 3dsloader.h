
#ifndef _3DSLOADER_H
#define _3DSLOADER_H


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

//extern char Load3DS(obj_type_ptr p_object, char *filename);
extern char Load3DS(obj_type *p_object, char *p_filename);


#endif