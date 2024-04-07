
// Library includes
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <fstream>

// Project includes
#include "3dsloader.h"


//>------ Primary Chunk, at the beginning of each file
#define PRIMARY       0x4D4D

//>------ Main Chunks
#define EDIT3DS       0x3D3D
#define VERSION       0x0002
#define MESHVERSION   0x3D3E
#define EDITKEYFRAME  0xB000

//>------ sub defines of EDIT3DS
#define EDITMATERIAL  0xAFFF
#define EDITOBJECT    0x4000

//>------ sub defines of EDITMATERIAL
#define MATNAME       0xA000  
#define MATLUMINANCE  0xA010
#define MATDIFFUSE    0xA020
#define MATSPECULAR   0xA030
#define MATSHININESS  0xA040
#define MATMAP        0xA200
#define MATMAPFILE    0xA300

#define OBJTRIMESH    0x4100

//>------ sub defines of OBJTRIMESH
#define TRIVERT       0x4110
#define TRIFACE       0x4120
#define TRIFACEMAT    0x4130
#define TRIUV         0x4140
#define TRISMOOTH     0x4150
#define TRILOCAL      0x4160

//>------ sub defines of EIDTKEYFRAME
#define KFMESH        0xB002
#define KFHEIRARCHY   0xB030
#define KFNAME        0xB010


//>>------  these define the different color chunk types
#define RGBF   0x0010
#define RGB24  0x0011


std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

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

char Load3DS(obj_type *p_object, char *p_filename)
{
	int i; //Index variable
	
	FILE* file; //File pointer
	
	unsigned short l_chunk_id; //Chunk identifier
	unsigned int l_chunk_lenght; //Chunk lenght

	unsigned char l_char; //Char variable
	unsigned short l_qty; //Number of elements in each chunk

	unsigned short l_face_flags; //Flag that stores some face information

	if( (file = fopen(p_filename, "rb") ) == NULL)
		return 0; //Open the file

	int32_t fileLength = fseek(file, 0, SEEK_END);
	rewind(file);

	while ( ftell(file) < fileLength ) //Loop to scan the whole file 
	{
		//getche(); //Insert this command for debug (to wait for keypress for each chuck reading)

		fread (&l_chunk_id, 2, 1, file); //Read the chunk header
		//printf("ChunkID: %x\n",l_chunk_id); 
		fread (&l_chunk_lenght, 4, 1, file); //Read the lenght of the chunk
		//printf("ChunkLenght: %x\n",l_chunk_lenght);

		switch (l_chunk_id)
        {
			//----------------- MAIN3DS -----------------
			// Description: Main chunk, contains all the other chunks
			// Chunk ID: 4d4d 
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case PRIMARY: 
				break;    

			//----------------- EDIT3DS -----------------
			// Description: 3D Editor chunk, objects layout info 
			// Chunk ID: 3d3d (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case EDIT3DS:
				break;
			
			//--------------- EDIT_OBJECT ---------------
			// Description: Object block, info for each object
			// Chunk ID: 4000 (hex)
			// Chunk Lenght: len(object name) + sub chunks
			//-------------------------------------------
			case EDITOBJECT: 
					i = 0;
					do
					{
						fread (&l_char, 1, 1, file);
						p_object->name[i] = l_char;
						i++;
					} while (l_char != '\0' && i < 20);
				break;

			//--------------- OBJ_TRIMESH ---------------
			// Description: Triangular mesh, contains chunks for 3d mesh info
			// Chunk ID: 4100 (hex)
			// Chunk Lenght: 0 + sub chunks
			//-------------------------------------------
			case OBJTRIMESH:
				break;
			
			//--------------- TRI_VERTEXL ---------------
			// Description: Vertices list
			// Chunk ID: 4110 (hex)
			// Chunk Lenght: 1 x unsigned short (number of vertices) 
			//             + 3 x float (vertex coordinates) x (number of vertices)
			//             + sub chunks
			//-------------------------------------------
			case TRIVERT: 
					fread(&l_qty, sizeof (unsigned short), 1, file);
					p_object->vertices_qty = l_qty;
					//printf("Number of vertices: %d\n", l_qty);
	                
					for(i = 0; i < l_qty; i += 1)
					{
						fread (&p_object->vertex[i].x, sizeof(float), 1, file);
 						//printf("Vertices list x: %f\n", p_object->vertex[i].x);
						fread (&p_object->vertex[i].z, sizeof(float), 1, file);
 						//printf("Vertices list y: %f\n", p_object->vertex[i].z);
						fread (&p_object->vertex[i].y, sizeof(float), 1, file);
 						//printf("Vertices list z: %f\n", p_object->vertex[i].y);
					}
				break;

			//--------------- TRI_FACEL1 ----------------
			// Description: Polygons (faces) list
			// Chunk ID: 4120 (hex)
			// Chunk Length: 1 x unsigned short (number of polygons) 
			//             + 3 x unsigned short (polygon points) x (number of polygons)
			//             + sub chunks
			//-------------------------------------------
			case TRIFACE:
					fread(&l_qty, sizeof (unsigned short), 1, file);
					p_object->polygons_qty = l_qty;
					//printf("Number of polygons: %d\n",l_qty); 
	                
					for ( i = 0; i < l_qty; i += 1 ) {
						fread (&p_object->polygon[i].a, sizeof (unsigned short), 1, file);
						//printf("Polygon point a: %d\n", p_object->polygon[i].a);
						fread (&p_object->polygon[i].b, sizeof (unsigned short), 1, file);
						//printf("Polygon point b: %d\n", p_object->polygon[i].b);
						fread (&p_object->polygon[i].c, sizeof (unsigned short), 1, file);
						//printf("Polygon point c: %d\n", p_object->polygon[i].c);
						fread (&l_face_flags, sizeof (unsigned short), 1, file);
						//printf("Face flags: %x\n", l_face_flags);
					}
                break;

			//------------- TRI_MAPPINGCOORS ------------
			// Description: Vertices list
			// Chunk ID: 4140 (hex)
			// Chunk Lenght: 1 x unsigned short (number of mapping points) 
			//             + 2 x float (mapping coordinates) x (number of mapping points)
			//             + sub chunks
			//-------------------------------------------
			case TRIUV:
					fread(&l_qty, sizeof (unsigned short), 1, file);
					for(i = 0; i < l_qty; i += 1)
					{
						fread (&p_object->mapcoord[i].u, sizeof (float), 1, file);
						//printf("Mapping list u: %f\n", p_object->mapcoord[i].u);
						fread (&p_object->mapcoord[i].v, sizeof (float), 1, file);
						//printf("Mapping list v: %f\n", p_object->mapcoord[i].v);
					}
                break;

			//----------- Skip unknow chunks ------------
			//We need to skip all the chunks that currently we don't use
			//We use the chunk lenght information to set the file pointer
			//to the same level next chunk
			//-------------------------------------------
			default:
				 fseek(file, l_chunk_lenght - 6, SEEK_CUR);
        } 
	}

	fclose(file);	// Closes the file stream
	return 1;		// Returns ok
}
