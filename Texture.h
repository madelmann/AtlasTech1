#ifndef _TEXTURE_H
#define _TEXTURE_H


#include "Main.h"
#include "Singleton.h"
#include <vector>
#include "jpeglib.h"


class Texture
{
public:
	Texture(int id)
	: mId(id)
	{
		glGenTextures(1, &mId);
	}
	~Texture()
	{
	}

	int getId()
	{
		return mId;
	}
	int getHeight()
	{
		return mHeight;
	}
	void setHeight(int height)
	{
		mHeight = height;
	}
	int getWidth()
	{
		return mWidth;
	}
	void setWidth(int width)
	{
		mWidth = width;
	}

private:
	int		mBpp;
	GLuint	mEnvMode;
	GLuint	mFormat;
	int		mHeight;
	GLuint	mId;
	char*	mName;
	GLuint	mType;
	int		mWidth;
	GLuint	mWrapModeR;
	GLuint	mWrapModeS;
	GLuint	mWrapModeT;
};


// This is our image structure for our targa data
struct tImage
{
	int channels;			// The channels in the image (3 = RGB : 4 = RGBA)
	int sizeX;				// The width of the image in pixels
	int sizeY;				// The height of the image in pixels
	unsigned char *data;	// The image pixel data
};

class CTexture
{
public:
	CTexture::CTexture()
	{
		iBpp = -1;
		iHeight = -1;
		iWidth = -1;
		sName = "";
		mId = 0;
	};

	CTexture::~CTexture()
	{
		iBpp = -1;
		iHeight = -1;
		iWidth = -1;
		sName = "";
		mId = 0;
	};

	int		iBpp;
	int		iHeight;
	int		iWidth;
	string	sName;
	GLuint	mId;

private:
};

class CTextureManager: public CSingleton< CTextureManager >
{
	friend class CSingleton< CTextureManager >;

public:
	CTextureManager();
	~CTextureManager();

public :
	void Apply();
	void clear();
	void CreateAnimBMPTextures(LPSTR szFileName, int count);
	GLuint CreateBMPTexture(LPSTR Filename);
	GLuint CreateBMPTexture1D(LPSTR Filename);
	GLuint CreateJPGTexture(LPSTR Filename);
	void CreateRenderTexture(UINT textureArray[], int width, int height, int channels, int type, int textureID);
	GLuint CreateTexture(char* Filename);
	GLuint CreateTGATexture(LPSTR Filename);
	void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData);					// This decompresses the JPEG and fills in the image data
	GLuint DeleteTexture(LPSTR Filename);
	GLuint DeleteTextureByIndex(GLuint index);
	GLuint getNextId() const;
	int GetTGABPP(char* Filename);
	unsigned char* GetTGAData(char* Filename);
	inline GLuint IndexOf(char Filename[255]);
	Texture* newTexture(char* Fielname);
	GLuint NextFreeTexture();
	void Reset();
	void SetActiveTexture(char* Filename);
	void SetActiveTextureID(GLuint id);
	void UnLoadTextures(GLuint StartIndex);

	int					iFilter;
	GLfloat				iMaxFilter[4];
	GLfloat				iMinFilter[4];

protected:

private:
	vector<CTexture*>	Textures;
};


#endif