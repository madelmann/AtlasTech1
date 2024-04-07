
// Library includes

// Project includes
#include "Terrain.h"


// In this file we just added a third texture coordinate on our terrain for 
// doing our caustic effects.  This is done in SetTextureCoord().  When we
// want to render caustics we just turn on the third texture unit, otherwise
// it won't draw them.


// This stores the desired depth that we want to fog
extern float g_FogDepth;

// This tells us if we want detail texturing turned on or off
extern bool g_bDetail;

// This holds the current tile scale value for our detail texture
extern int g_DetailScale;


extern	CVertex	Terrain[9][256][256];
int		vCount;
CFace	fTerrain[130050];
int		fCount;


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude2(CVector3 vNormal)
{
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2) : Where V is the vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a normalize vector (A vector exactly of length 1)
/////
/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

//CVector3 Normalize2(CVector3 vVector)
//{
//	// Get the magnitude of our normal
//	float magnitude = (float)sqrt( (vVector.x * vVector.x) + 
//								   (vVector.y * vVector.y) + 
//								   (vVector.z * vVector.z) );				
//
//	// Now that we have the magnitude, we can divide our vector by that magnitude.
//	// That will make our vector a total length of 1.  
//	vVector = vVector / magnitude;		
//	
//	// Finally, return our normalized vector
//	return vVector;										
//}
//
//
//void GetNormal(CVector3 v[3], CVector3 *normal)
//{
//    CVector3 a, b;
//
//    // calculate the vectors A and B
//    // note that v[3] is defined with counterclockwise winding in mind
//    // a
//    a.x = v[0].x - v[1].x;
//    a.y = v[0].y - v[1].y;
//    a.z = v[0].z - v[1].z;
//    // b
//    b.x = v[1].x - v[2].x;
//    b.y = v[1].y - v[2].y;
//    b.z = v[1].z - v[2].z;
//
//    // calculate the cross product and place the resulting vector
//    // into the address specified by vertex_t *normal
//
//    normal->x = (a.z * b.y) - (a.y * b.z);
//    normal->y = (a.y * b.x) - (a.x * b.y);
//    normal->z = (a.x * b.z) - (a.z * b.x);
//
//    //normal->x = (a.y * b.z) - (a.z * b.y);
//    //normal->y = (a.z * b.x) - (a.x * b.z);
//    //normal->z = (a.x * b.y) - (a.y * b.x);
//
//    // set vector to lengh 1
//    *normal = Normalize2(*normal);
//}


///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the height into the height map
/////
///////////////////////////////// HEIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int Height(BYTE *pHeightMap, int X, int Y)
{
	// Make sure we don't go past our array size
	int x = X % MAP_SIZE;					// Error check our x value
	int y = Y % MAP_SIZE;					// Error check our y value

	if( (x < 0) || (y < 0) ) return 0;

	if(!pHeightMap) return 0;				// Make sure our data is valid

	// Use the equation: index = (x + (y * arrayWidth) ) to find the current height
	return pHeightMap[x + (y * MAP_SIZE)];	// Index into our height array and return the height
}


///////////////////////////////// SET TEXTURE COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This sets the current texture coordinate of the terrain, based on the X and Z
/////
///////////////////////////////// SET TEXTURE COORD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SetTextureCoord(float x, float z, float height)
{
	// Find the (u, v) coordinate for the current vertex
	//float u =  (float)x / (float)MAP_SIZE;
	//float v = -(float)z / (float)MAP_SIZE;
	float u =  (float)x / (MAP_SIZE *8);
	float v = -(float)z / (MAP_SIZE *8);
	
	if(g_bMultiTexturing) {

		// Give OpenGL the current terrain texture coordinate for our height map
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

		// Give OpenGL the current detail texture coordinate for our height map
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);

		// Give OpenGL the current caustic texture coordinate for our height map
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, u, v);

	}

}

bool VectorEqual(CVector3 a, CVector3 b)
{
	if(a.x == b.x && a.y == b.y && a.z == b.z)
		return true;
	else
		return false;
}

//void CalculateFaceNormals(BYTE *pHeightMap, float posx, float posy, float posz)
//{
//	int X = 0, Y = 0;						// Create some variables to walk the array with.
//	int x, y, z;							// Create some variables for readability
//	bool bSwitchSides = false;
//
//	vCount = 0;
//
//	// Go through all of the rows of the height map
//	for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE )
//	{
//		for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE)
//		{
//			Terrain[X][Y].Vertex.x = (int)(X * g_iHeightMapScaleX + posx);
//			Terrain[X][Y].Vertex.y = (int)(Height(pHeightMap, X, Y ) * g_iHeightMapScaleY + posy);
//			Terrain[X][Y].Vertex.z = (int)(Y * g_iHeightMapScaleZ + posz);
//		}
//	}
//
//	vCount--;
//
//	fCount = 0;
//
//	for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE)
//	{
//		for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE)
//		{
//
//			fTerrain[fCount].v[0].x = Terrain[X][Y].Vertex.x;
//			fTerrain[fCount].v[0].y = Terrain[X][Y].Vertex.y;
//			fTerrain[fCount].v[0].z = Terrain[X][Y].Vertex.z;
//
//			fTerrain[fCount].v[1].x = Terrain[X + STEP_SIZE][Y].Vertex.x;
//			fTerrain[fCount].v[1].y = Terrain[X + STEP_SIZE][Y].Vertex.y;
//			fTerrain[fCount].v[1].z = Terrain[X + STEP_SIZE][Y].Vertex.z;
//
//			fTerrain[fCount].v[2].x = Terrain[X][Y + STEP_SIZE].Vertex.x;
//			fTerrain[fCount].v[2].y = Terrain[X][Y + STEP_SIZE].Vertex.y;
//			fTerrain[fCount].v[2].z = Terrain[X][Y + STEP_SIZE].Vertex.z;
//
//			GetNormal(fTerrain[fCount].v, &fTerrain[fCount].vNormal); 
//
//			fCount++;
//
//		}
//	}
//
//
//	int i;
//	int j;
//	int counttox, counttoy;
//
//	for (i = 0; i <= MAP_SIZE * MAP_SIZE; i++)
//	{
//
//		counttoy = (int)(i / MAP_SIZE);
//		counttox = i - counttoy * MAP_SIZE;
//
//		Terrain[counttox][counttoy].vNormal.x = 0.0f;
//		Terrain[counttox][counttoy].vNormal.y = 0.0f;
//		Terrain[counttox][counttoy].vNormal.z = 0.0f;
//
//
//		for (j = 0; j <= fCount; j++)
//		{
//			if(VectorEqual(Terrain[counttox][counttoy].Vertex, fTerrain[j].v[0]) || VectorEqual(Terrain[counttox][counttoy].Vertex, fTerrain[j].v[1]) || VectorEqual(Terrain[counttox][counttoy].Vertex, fTerrain[j].v[2]))
//			{
//				Terrain[counttox][counttoy].vNormal.x += fTerrain[j].vNormal.x;
//				Terrain[counttox][counttoy].vNormal.y += fTerrain[j].vNormal.y;
//				Terrain[counttox][counttoy].vNormal.z += fTerrain[j].vNormal.z;
//			}
//		}
//
//	}
//}

void RenderHeightMapXY(BYTE *pHeightMap, float posx, float posy, float posz)
{
	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	bool bSwitchSides = false;

	glDisable(GL_CULL_FACE);

	// Make sure our height data is valid
	if(!pHeightMap) return;		
	
	// Activate the first texture ID and bind the tree background to it
	if(g_bMultiTexturing) {

		glActiveTextureARB(GL_TEXTURE0_ARB);

	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Texture[TERRAIN_ID]);


	glMatrixMode(GL_TEXTURE);

		// Reset the current matrix and apply our chosen scale value
		glLoadIdentity();
		//glScalef(1.0f, 1.0f, 1.0f);
		glScalef(0.5f, 0.5f, 1);

	// Leave the texture matrix and set us back in the model view matrix
	glMatrixMode(GL_MODELVIEW);


	if(g_bMultiTexturing) {

		// If we want detail texturing on, let's render the second texture
		if(g_bDetail)
		{
			// Activate the second texture ID and bind the fog texture to it
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			
			// Here we turn on the COMBINE properties and increase our RGB
			// gamma for the detail texture.  2 seems to work just right.
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
			
			// Bind the detail texture
			glBindTexture(GL_TEXTURE_2D, g_Texture[DETAIL_ID]);
		
			// Now we want to enter the texture matrix.  This will allow us
			// to change the tiling of the detail texture.
			glMatrixMode(GL_TEXTURE);

				// Reset the current matrix and apply our chosen scale value
				glLoadIdentity();
				glScalef((float)g_DetailScale, (float)g_DetailScale, 1);

			// Leave the texture matrix and set us back in the model view matrix
			glMatrixMode(GL_MODELVIEW);
		}

	}

	// We want to render triangle strips
	//glBegin( GL_TRIANGLES);
	glBegin( GL_TRIANGLE_STRIP);

	for( X = 0; X < MAP_SIZE - 1; X += STEP_SIZE)
	{
		if(bSwitchSides)
			for( Y = 0; Y < MAP_SIZE - 1; Y += STEP_SIZE)
			{

				glNormal3f(Terrain[X][Y].vNormal.x, Terrain[X][Y].vNormal.y, Terrain[X][Y].vNormal.z);
				SetTextureCoord(Terrain[X][Y].Vertex.x, Terrain[X][Y].Vertex.z, Terrain[X][Y].Vertex.y);
				glVertex3f(Terrain[X][Y].Vertex.x, Terrain[X][Y].Vertex.y, Terrain[X][Y].Vertex.z);

				glNormal3f(Terrain[X + 1][Y].vNormal.x, Terrain[X + 1][Y].vNormal.y, Terrain[X + 1][Y].vNormal.z);
				SetTextureCoord(Terrain[X + 1][Y].Vertex.x, Terrain[X + 1][Y].Vertex.z, Terrain[X + 1][Y].Vertex.y);
				glVertex3f(Terrain[X + 1][Y].Vertex.x, Terrain[X + 1][Y].Vertex.y, Terrain[X + 1][Y].Vertex.z);
			}
		else
			//for ( Y = MAP_SIZE - 1; Y > 0; Y -= STEP_SIZE )
			for ( Y = MAP_SIZE - 1; Y >= 0; Y -= STEP_SIZE )
			{

				glNormal3f(Terrain[X][Y].vNormal.x, Terrain[X][Y].vNormal.y, Terrain[X][Y].vNormal.z);
				SetTextureCoord(Terrain[X][Y].Vertex.x, Terrain[X][Y].Vertex.z, Terrain[X][Y].Vertex.y);
				glVertex3f(Terrain[X][Y].Vertex.x, Terrain[X][Y].Vertex.y, Terrain[X][Y].Vertex.z);

				glNormal3f(Terrain[X + 1][Y].vNormal.x, Terrain[X + 1][Y].vNormal.y, Terrain[X + 1][Y].vNormal.z);
				SetTextureCoord(Terrain[X + 1][Y].Vertex.x, Terrain[X + 1][Y].Vertex.z, Terrain[X + 1][Y].Vertex.y);
				glVertex3f(Terrain[X + 1][Y].Vertex.x, Terrain[X + 1][Y].Vertex.y, Terrain[X + 1][Y].Vertex.z);
			}

			bSwitchSides = !bSwitchSides;
	}


	// Stop rendering triangle strips
	glEnd();
	if(g_bMultiTexturing) {
	
		glActiveTextureARB(GL_TEXTURE0_ARB);

	}
	glMatrixMode(GL_TEXTURE);

		// Reset the current matrix and apply our chosen scale value
		glLoadIdentity();
		glScalef(1, 1,1);

	// Leave the texture matrix and set us back in the model view matrix
	glMatrixMode(GL_MODELVIEW);


	if(g_bMultiTexturing) {

		// Turn the second multitexture pass off
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

	}

	// Turn the first multitexture pass off
	if(g_bMultiTexturing) {
	
		glActiveTextureARB(GL_TEXTURE0_ARB);		

	}
    glDisable(GL_TEXTURE_2D);
}


void ModifyHeightMap(BYTE *pHeightMap, int X, int Y, int Delta)
{
	if((X < 0) || (Y < 0) || (X >= 256) || (Y >= 256))
		return;

	// Make sure we don't go past our array size
	int x = X % MAP_SIZE;					// Error check our x value
	int y = Y % MAP_SIZE;					// Error check our y value

	if(!pHeightMap) return;						// Make sure our data is valid

	//x = int(x / STEP_SIZE) * STEP_SIZE;
	//y = int(y / STEP_SIZE) * STEP_SIZE;

	for (int v = -2; v < 3; v++) {
		for(int u = -2; u < 3; u++) {

			if( pHeightMap[(x + u * STEP_SIZE) + ((y + v * STEP_SIZE) * MAP_SIZE)] + Delta < 0 ) {

				pHeightMap[(x + u * STEP_SIZE) + ((y + v * STEP_SIZE) * MAP_SIZE)] = 0;

			} else if( pHeightMap[(x + u * STEP_SIZE) + ((y + v * STEP_SIZE) * MAP_SIZE)] + Delta > 255 ) {

				pHeightMap[(x + u * STEP_SIZE) + ((y + v * STEP_SIZE) * MAP_SIZE)] = 255;

			} else {

				pHeightMap[(x + u * STEP_SIZE) + ((y + v * STEP_SIZE) * MAP_SIZE)] += Delta;

			}

		}
	}
}


void LoadNormalFile(LPSTR strName, int nSize)
{
	FILE *pFile = NULL;

	char buffer[200];

	sprintf(buffer, "Terrain\\%s", strName);

	if(fopen_s(&pFile, buffer, "rb") != NULL)	{
		// Display our error message and stop the function
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	for(int i = 0; i < nSize; i += 1)
	{
		int counttoy = (int)(i / MAP_SIZE);
		int counttox = i - counttoy * MAP_SIZE;

		fread(&Terrain[counttox][counttoy], sizeof(CVertex), nSize, pFile);
	}

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror( pFile );

	// Check if we received an error.
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// Close the file.
	fclose(pFile);
}


///////////////////////////////// LOAD RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a .raw file into an array of bytes.  Each value is a height value.
/////
///////////////////////////////// LOAD RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	if(fopen_s(&pFile, strName, "rb") != NULL)	{
		// Display our error message and stop the function
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// Here we load the .raw file into our pHeightMap data array.
	// We are only reading in '1', and the size is the (width * height)
	fread( pHeightMap, 1, nSize, pFile );

	// After we read the data, it's a good idea to check if everything read fine.
	int result = ferror( pFile );

	// Check if we received an error.
	if (result)
	{
		MessageBox(NULL, "Can't get data!", "Error", MB_OK);
	}

	// Close the file.
	fclose(pFile);

	LoadNormalFile(strName, MAP_SIZE * MAP_SIZE);

}


///////////////////////////////// SAVE RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a .raw file into an array of bytes.  Each value is a height value.
/////
///////////////////////////////// SAVE RAW FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SaveRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// Let's open the file in Write/Binary mode.
	if( fopen_s(&pFile, strName, "w+b") != NULL ) {
		// Display our error message and stop the function
		MessageBox(NULL, "Can't find the height map!", "Error", MB_OK);
		return;
	}

	// Here we save the .raw file from our pHeightMap data array.
	fwrite( pHeightMap, sizeof(BYTE) * nSize, 1, pFile );

	// Close the file.
	fclose(pFile);
}

