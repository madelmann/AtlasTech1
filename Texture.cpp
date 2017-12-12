#include "Main.h"


CTextureManager::CTextureManager()
{
	iFilter = 3;
	iMaxFilter[0] = GL_NEAREST;
	iMinFilter[0] = GL_NEAREST;
	iMaxFilter[1] = GL_LINEAR;
	iMinFilter[1] = GL_NEAREST_MIPMAP_LINEAR;
	iMaxFilter[2] = GL_LINEAR;
	iMinFilter[2] = GL_LINEAR_MIPMAP_NEAREST;
	iMaxFilter[3] = GL_LINEAR;
	iMinFilter[3] = GL_LINEAR_MIPMAP_LINEAR;
}

CTextureManager::~CTextureManager()
{
	clear();
}

void CTextureManager::Apply()
{
	for(unsigned int i = 0; i < Textures.size(); i += 1)
	{
		glBindTexture(GL_TEXTURE_2D, Textures[i]->mId);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);					// select modulate to mix texture with color for shading

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, iMinFilter[iFilter]);		// when texture area is small, bilinear filter the closest mipmap
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, iMaxFilter[iFilter]);		// when texture area is large, bilinear filter the first mipmap

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Engine->fAnsitropicFilter);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Textures[i].iWidth, Textures[i].iHeight, GL_RGB, GL_UNSIGNED_BYTE, &Texture[i]);
	}
}

void CTextureManager::clear()
{
	for ( unsigned int i = 0; i < Textures.size(); i += 1 ) {
		delete Textures[i];
	}

	Textures.clear();
}

void CTextureManager::CreateAnimBMPTextures(LPSTR szFileName, int count)
{
	char szBuffer[255] = "";

	// Don't load anything if a valid file name doesn't exist
	if(!szFileName)
		return;

	// Loop until we have loaded in all the desired bitmaps
	for(int i = 0; i < count; i += 1)
	{
		// The bitmaps must be named with 2 digits (i.e. caust00.bmp caust01.bmp, etc..)
		sprintf_s(szBuffer, "%s%d%d.bmp", szFileName, i / 10, i % 10);

		// Create a texture map from our loaded bitmap
		CreateTexture(szBuffer);
	}
}

GLuint CTextureManager::CreateBMPTexture(LPSTR Filename)
{
	AUX_RGBImageRec *pImage = NULL;
	FILE *pFile = NULL;

	char buffer[255] = "";
	sprintf(buffer, "%sMaps\\%s\\Textures\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTextures\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[CreateBMPTexture]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return (GLuint)-1;
		}
	}
	fclose(pFile);

	int index = IndexOf(Filename);
	//int index = NextFreeTexture();
	if(index != (GLuint)-1) {
		return index;
	}

	// Load the bitmap using the aux function stored in glaux.lib
	pImage = auxDIBImageLoad(buffer);				

	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage == NULL)								
		return (GLuint)-1;

	Textures.push_back(new CTexture());

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &Textures.back()->mId);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, Textures.back()->mId);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, iMinFilter[iFilter]);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, iMaxFilter[iFilter]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Engine->fAnsitropicFilter);

	GLfloat wrap = 1.0f;

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	GLint gliFormat = GL_RGB;
	if(Engine->bCompressedTextures)
		gliFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

	gluBuild2DMipmaps(GL_TEXTURE_2D, gliFormat, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);


	sprintf(buffer, "");
	//sprintf(buffer, "%s", ExtractFilename(Filename));
	unsigned int i = strlen(Filename) - 1;

	while(Filename[i] != '\\')
	{
		i -= 1;
	}

	while(i < strlen(Filename))
	{
		i += 1;
		sprintf(buffer, "%s%c", buffer, Filename[i]);
	}

	//Name.push_back(buffer);

	Textures.back()->iBpp = 24;
	Textures.back()->iHeight = pImage->sizeY;
	Textures.back()->iWidth = pImage->sizeX;
	Textures.back()->sName = Filename;


	// Now we need to free the image data that we loaded since openGL stored it as a texture
	if(pImage)										// If we loaded the image
	{
		if(pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}

		free(pImage);								// Free the image structure
	}

	return Textures.back()->mId;
}

GLuint CTextureManager::CreateBMPTexture1D(LPSTR Filename)
{
	AUX_RGBImageRec *pImage = NULL;
	FILE *pFile = NULL;

	char buffer[255] = "";
	sprintf(buffer, "%sMaps\\%s\\Textures\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTextures\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[CreateBMPTexture]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return (GLuint)-1;
		}
	}
	fclose(pFile);

	int index = IndexOf(Filename);
	if(index != (GLuint)-1) {
		return index;
	}

	// Load the bitmap using the aux function stored in glaux.lib
	pImage = auxDIBImageLoad(buffer);				

	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage == NULL)								
		return (GLuint)-1;

	CTexture *texture = new CTexture();

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texture->mId);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_1D, texture->mId);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pImage->data);


	sprintf(buffer, "");
	//sprintf(buffer, "%s", ExtractFilename(Filename));
	unsigned int i = strlen(Filename) - 1;

	while(Filename[i] != '\\')
	{
		i -= 1;
	}

	while(i < strlen(Filename))
	{
		i += 1;
		sprintf(buffer, "%s%c", buffer, Filename[i]);
	}

	texture->iBpp = 24;
	texture->iHeight = pImage->sizeY;
	texture->iWidth = pImage->sizeX;
	texture->sName = Filename;

	Textures.push_back(texture);


	// Now we need to free the image data that we loaded since openGL stored it as a texture
	if(pImage)										// If we loaded the image
	{
		if(pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}

		free(pImage);								// Free the image structure
	}

	return texture->mId;
}

GLuint CTextureManager::CreateJPGTexture(LPSTR Filename)
{
	struct jpeg_decompress_struct cinfo;
	tImage *pImage = NULL;
	FILE *pFile;
	char buffer[255] = "";

	sprintf(buffer, "%sMaps\\%s\\Textures\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTextures\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[CreateBMPTexture]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return (GLuint)-1;
		}
	}

	int index = IndexOf(Filename);
	//int index = NextFreeTexture();
	if(index != (GLuint)-1) {
		return index;
	}
	
	// Create an error handler
	jpeg_error_mgr jerr;

	// Have our compression info object point to the error handler address
	cinfo.err = jpeg_std_error(&jerr);
	
	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);
	
	// Specify the data source (Our file pointer)	
	jpeg_stdio_src(&cinfo, pFile);
	
	// Allocate the structure that will hold our eventual jpeg data (must free it!)
	pImage = (tImage*)malloc(sizeof(tImage));

	// Decode the jpeg file and fill in the image data structure to pass back
	DecodeJPG(&cinfo, pImage);
	
	// This releases all the stored memory for reading and decoding the jpeg
	jpeg_destroy_decompress(&cinfo);
	
	// Close the file pointer that opened the file
	fclose(pFile);


	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage->data == NULL)								
		return (GLuint)-1;

	CTexture *texture = new CTexture();

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texture->mId);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, texture->mId);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, iMinFilter[iFilter]);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, iMaxFilter[iFilter]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Engine->fAnsitropicFilter);

	GLfloat wrap = 1.0f;

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	GLint gliFormat = GL_RGB;
	if(Engine->bCompressedTextures)
		gliFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

	gluBuild2DMipmaps(GL_TEXTURE_2D, gliFormat, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);


	sprintf(buffer, "");
	//sprintf(buffer, "%s", ExtractFilename(Filename));
	unsigned int i = strlen(Filename) - 1;

	while(Filename[i] != '\\')
	{
		i -= 1;
	}

	while(i < strlen(Filename))
	{
		i += 1;
		sprintf(buffer, "%s%c", buffer, Filename[i]);
	}

	texture->iBpp = 24;
	texture->iHeight = pImage->sizeY;
	texture->iWidth = pImage->sizeX;
	texture->sName = Filename;

	Textures.push_back(texture);


	// Return the jpeg data (remember, you must free this data after you are done)
	if(pImage)										// If we loaded the image
	{
		if(pImage->data)							// If there is texture data
			free(pImage->data);						// Free the texture data, we don't need it anymore

		free(pImage);								// Free the image structure
	}

	return texture->mId;
}

///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a blank texture to render too
/////
///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CTextureManager::CreateRenderTexture(UINT textureArray[], int width, int height, int channels, int type, int textureID)
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [width * height * channels];
	memset(pTexture, 0, width * height * channels * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					

	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, type, GL_UNSIGNED_INT, pTexture);						

	// Set the texture quality
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}

GLuint CTextureManager::CreateTexture(char* Filename)
{
	char sExtention[4] = "";
	//char* sExtention = "";

	if(Filename == NULL)
		return (GLuint)-1;

	if(strlen(Filename) <= 0)
		return (GLuint)-1;

	if(Filename[strlen(Filename) - 4] == '.')
	{
		for(unsigned int i = strlen(Filename) - 3; i < strlen(Filename); i += 1)
			sprintf(sExtention, "%s%c", sExtention, Filename[i]);
	}

	//sExtention = ExtractFileExt(Filename);

	if(strcmpi(sExtention, "bmp") == 0)
		return CreateBMPTexture(Filename);
	else if(strcmpi(sExtention, "jpg") == 0)
		return CreateJPGTexture(Filename);
	else if(strcmpi(sExtention, "tga") == 0)
		return CreateTGATexture(Filename);
	else
		return (GLuint)-1;
}

// Load a TGA texture
GLuint CTextureManager::CreateTGATexture(LPSTR Filename)
{
	FILE* pFile;
	char buffer[255];

	sprintf(buffer, "%sMaps\\%s\\Textures\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTextures\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[CreateTGATexture]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return (GLuint)-1;
		}
	}
	fclose(pFile);
 
	int index = IndexOf(Filename);
	//int index = NextFreeTexture();
	if(index != (GLuint)-1) {
		return index;
	}

	TGAImg Img;        // Image loader

	// Load our Texture
	if(Img.Load(buffer) != IMG_OK)
	{
		// Display an error message saying the file was not found, then return NULL
		sprintf(buffer, "ERROR[CreateTGATexture]: '%s' not found!\n", Filename);
		Console->Output(buffer);
		return (GLuint)-1;
	}

	CTexture *texture = new CTexture();

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texture->mId);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
 
	glBindTexture(GL_TEXTURE_2D, texture->mId); // Set our Tex handle as current
 
	// Create the texture
	if(Img.GetBPP() == 24)
	{
		texture->iBpp = 24;
		glTexImage2D(GL_TEXTURE_2D, 0, 3, Img.GetWidth(), Img.GetHeight(), 0,
			         GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
	}
	else if(Img.GetBPP() == 32)
	{
		texture->iBpp = 32;
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Img.GetWidth(), Img.GetHeight(), 0,
			         GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());
	}
	else
		return (GLuint)-1;

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, iMinFilter[iFilter]);//GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, iMaxFilter[iFilter]);//GL_LINEAR );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Engine->fAnsitropicFilter);

	GLfloat wrap = 1.0f;

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	if(texture->iBpp == 24)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Img.GetWidth(), Img.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());
	else if(texture->iBpp == 32)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, Img.GetWidth(), Img.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, Img.GetImg());

	//GLint gliFormat = GL_RGBA;

	//if(texture.iBpp == 24)
	//{
	//	if(Engine->bCompressedTextures)
	//		gliFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	//	else
	//		gliFormat = GL_RGB;
	//}
	//else
	//	if(texture.iBpp == 32)
	//	{
	//		if(Engine->bCompressedTextures)
	//			gliFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	//		else
	//			gliFormat = GL_RGBA;
	//	}

	//gluBuild2DMipmaps(GL_TEXTURE_2D, gliFormat, Img.GetWidth(), Img.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, Img.GetImg());

	sprintf(buffer, "");
	unsigned int i = strlen(Filename) - 1;

	while(Filename[i] != '\\')
	{
		i--;
	}

	while(i < strlen(Filename))
	{
		i++;
		sprintf(buffer, "%s%c", buffer, Filename[i]);
	}

	texture->iHeight = Img.GetHeight();
	texture->iWidth = Img.GetWidth();
	texture->sName = Filename;

	Textures.push_back(texture);

	// Now we need to free the image data that we loaded since openGL stored it as a texture
	Img.~TGAImg();

	return texture->mId;
}

///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This decodes the jpeg and fills in the tImage structure
/////
///////////////////////////////// DECODE JPG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CTextureManager::DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData)
{
	// Read in the header of the jpeg file
	jpeg_read_header(cinfo, TRUE);
	
	// Start to decompress the jpeg file with our compression info
	jpeg_start_decompress(cinfo);

	// Get the image dimensions and channels to read in the pixel data
	pImageData->channels = cinfo->num_components;
	pImageData->sizeX    = cinfo->image_width;
	pImageData->sizeY    = cinfo->image_height;

	// Get the row span in bytes for each row
	int rowSpan = cinfo->image_width * cinfo->num_components;
	
	// Allocate memory for the pixel buffer
	pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*rowSpan*pImageData->sizeY));
			
	// Create an array of row pointers
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];

	for (int i = 0; i < pImageData->sizeY; i += 1)
		rowPtr[i] = &(pImageData->data[i * rowSpan]);

	// Now comes the juice of our work, here we extract all the pixel data
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		// Read in the current row of pixels and increase the rowsRead count
		rowsRead += jpeg_read_scanlines(cinfo, 
										&rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	
	// Delete the temporary row pointers
	delete [] rowPtr;

	// Finish decompressing the data
	jpeg_finish_decompress(cinfo);
}

GLuint CTextureManager::DeleteTexture(LPSTR Filename)
{
	return DeleteTextureByIndex(IndexOf(Filename));
}

GLuint CTextureManager::DeleteTextureByIndex(GLuint index)
{
	if(index == (GLuint)-1)
		return (GLuint)-1;

	glDeleteTextures(1, &Textures[index]->mId);

	delete Textures[index];

	return index;
}

GLuint CTextureManager::getNextId() const
{
	//glGenTextures(1, &Texture[Count]);

	return (GLuint)Textures.size();
}

int CTextureManager::GetTGABPP(char* Filename)
{
	FILE* pFile;
	char buffer[255];

	sprintf(buffer, "%sMaps\\%s\\Terrain\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTerrain\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[GetTGAData]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return NULL;
		}
	}
	fclose(pFile);
 

	TGAImg Img;        // Image loader

	// Load our Texture
	if(Img.Load(buffer) != IMG_OK)
	{
		// Display an error message saying the file was not found, then return NULL
		sprintf(buffer, "ERROR[GetTGAData]: while reading '%s'!\n", Filename);
		Console->Output(buffer);
		return NULL;
	}

	return Img.GetBPP();
}

unsigned char* CTextureManager::GetTGAData(char* Filename)
{
	FILE* pFile;
	char buffer[255];

	sprintf(buffer, "%sMaps\\%s\\Terrain\\%s", g_sGameName, Game.sMapName, Filename);
	
	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, buffer, "rb") != NULL) 
	{
		sprintf(buffer, "%sTerrain\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, buffer, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(buffer, "ERROR[GetTGAData]: '%s' not found!\n", Filename);
			Console->Output(buffer);
			return NULL;
		}
	}
	fclose(pFile);
 

	TGAImg Img;        // Image loader

	// Load our Texture
	if(Img.Load(buffer) != IMG_OK)
	{
		// Display an error message saying the file was not found, then return NULL
		sprintf(buffer, "ERROR[GetTGAData]: while reading '%s'!\n", Filename);
		Console->Output(buffer);
		return NULL;
	}

	return Img.GetImg();
}

inline GLuint CTextureManager::IndexOf(char Filename[255])
{
	//for(unsigned int i = 0; i < Textures.size(); i += 1) {
	for ( vector<CTexture*>::iterator it = Textures.begin(); it != Textures.end(); it++ ) {
		if((*it)->sName.compare(Filename) == 0 ) {
			return (*it)->mId;
		}
	}

	return (GLuint)-1;
}

Texture* CTextureManager::newTexture(char* Fielname)
{
	//Texture *newTexture = new Texture(getNextId());

	//return newTexture;

	return NULL;
}

GLuint CTextureManager::NextFreeTexture()
{
	for(unsigned int i = 0; i < Textures.size(); i += 1) {
		if(Textures[i]->sName.compare("") == 0) {
			return Textures[i]->mId;
		}
	}

	return (GLuint)-1;
}

void CTextureManager::Reset()
{
	iFilter = 3;
	iMaxFilter[0] = GL_NEAREST;
	iMinFilter[0] = GL_NEAREST;
	iMaxFilter[1] = GL_LINEAR;
	iMinFilter[1] = GL_NEAREST_MIPMAP_LINEAR;
	iMaxFilter[2] = GL_LINEAR;
	iMinFilter[2] = GL_LINEAR_MIPMAP_NEAREST;
	iMaxFilter[3] = GL_LINEAR;
	iMinFilter[3] = GL_LINEAR_MIPMAP_LINEAR;

	//Textures.clear();
}

void CTextureManager::SetActiveTexture(char* Filename)
{
	GLuint index = IndexOf(Filename);

	//if(iActiveTexture == index)
	//	return;

	//iActiveTexture = index;

	if(index == (GLuint)-1) {
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, index);
	}
}

void CTextureManager::SetActiveTextureID(GLuint id)
{
	//if(iActiveTexture == ID)
	//	return;

	//iActiveTexture = ID;

	if(id == (GLuint)-1) {
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, id);
	}
}

void CTextureManager::UnLoadTextures(GLuint StartIndex)
{
	if(StartIndex == (GLuint)-1)
		return;
	if(StartIndex >= Textures.size())
		return;

	for(unsigned int i = 0; i < Textures.size(); i += 1)
	{
		DeleteTextureByIndex(i);
	}

	Textures.resize(StartIndex);
}