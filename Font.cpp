
// Library includes
#include <windows.h>				// The windows header file.

// Project includes
#include "Font.h"					// Our font class header.
#include "GLWrapper.h"


extern GLuint Texture_Add(char* Filename);


CGLFont::CGLFont()
{
	Width = Height = 400;

	mFont.mCharHeight = 30.f;
	mFont.mCharKerning = 0.f;
	mFont.mCharWidth = 25.f;
	mFont.mName = "";
	mFont.mTexture = -1;
}

CGLFont::~CGLFont()
{
	ClearGLFont();                // Clear the font that we created in the constructor.
}

// Delete the font.
void CGLFont::ClearGLFont()
{
	// Delete font.
	if(listBase)
		glDeleteLists(listBase, 255);
}

int CGLFont::GetFontSize()
{
	return Size;
}

// Initialize font.
bool CGLFont::Initialize(HDC hDC, int size, char *fontName)
{
	// Create the font list.
	listBase = InitFont(fontName, size, hDC);

	if(listBase == 0)
		return false;

	return true;
}


unsigned int CGLFont::InitFont(char *fontName, int fontSize, HDC hDC)
{
	// Windows font
	HFONT hFont;
	unsigned int base;

	// Create space for 255 characters.
	base = glGenLists(255);

	if(stricmp(fontName, "symbol") == 0)
	{
		hFont = CreateFont(Size, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE,
						   SYMBOL_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						   ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName);
	}
	else
	{
		hFont = CreateFont(Size, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE,
						   ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
						   ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName);
	}

	// Error checking.
	if(!hFont)
		return 0;

	// Set up font.
	SelectObject(hDC, hFont);
	wglUseFontBitmaps(hDC, 0, 255, base);

	return base;
}


// Print the text at the desired position.
bool CGLFont::PrintText(char *string, float x, float y)
{
	// Error checking...
	if((listBase == 0 || string == NULL))
	  return false;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();

		glOrtho(0, 0, Width, Height, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			// And finally in order to get this to work correctly we must disable depth testing.
			// Since this is not 3D we can't do any depth testing.
			glDisable(GL_DEPTH_TEST);

			// Move to screen position sent in to this function.
			glRasterPos2f(x, y);

			// Print the string to the screen.
			glPushAttrib(GL_LIST_BIT);
				glListBase(listBase);
				glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);
			glPopAttrib();

			// Set everything back to perpective below, don't forget to re-enable depth testing.
			glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);										
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	return true;
}

bool CGLFont::PrintText(std::string text, float x, float y)
{
	// Error checking...
	if((listBase == 0 || text.length() <= 0))
	  return false;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();

		glOrtho(0, 0, Width, Height, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			// And finally in order to get this to work correctly we must disable depth testing.
			// Since this is not 3D we can't do any depth testing.
			//glDisable(GL_DEPTH_TEST);

			// Move to screen position sent in to this function.
			glRasterPos2f(x, y);

			const char* buffer = text.c_str();

			// Print the string to the screen.
			glPushAttrib(GL_LIST_BIT);
				glListBase(listBase - 32);
				glCallLists(text.length(), GL_UNSIGNED_BYTE, buffer);
			glPopAttrib();

			// Set everything back to perpective below, don't forget to re-enable depth testing.
			//glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	glMatrixMode(GL_PROJECTION);										
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	return true;
}


void CGLFont::SetWidthHeight(int w, int h)
{
   Width = w;
   Height = h;
}

void CGLFont::drawChar(char ch, float x, float y)
{
	float s = (ch % 16) * 25.f;//mFont.mCharWidth;
	float t = (ch / 16) * 30.f;//mFont.mCharHeight;

	glTexCoord2f(s, t);
	glVertex2f(x, y);

	glTexCoord2f(s, t + 30.f);
	glVertex2f(x, y + 30.f);

	glTexCoord2f(s + 25.f, t + 30.f);
	glVertex2f(x + 25.f, y + 30.f);

	glTexCoord2f(s + 25.f, t);
	glVertex2f(x + 25.f, y);
}

void CGLFont::drawString(const std::string &str, float x, float y)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDepthFunc(GL_ALWAYS);
	glDepthMask(FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();

			// make (0, 0) the upper left corner and
			// (width, height) the lower right corner
			glOrtho(0, 0, Width, Height, 0, 1);

			// set the texture matrix so that texture coordinates
			// s and t range from 0 to width/height
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
				float texMatrix[16];
				memset(texMatrix, 0, sizeof(texMatrix));
				texMatrix[0 + 0 * 4] = 1.0 / 1.0;//tex->getTexelWidth();
				texMatrix[1 + 1 * 4] = 1.0 / 1.0;//tex->getTexelHeight();
				texMatrix[2 + 2 * 4] = 1.0;
				texMatrix[3 + 3 * 4] = 1.0;
				glLoadMatrixf(texMatrix);

				// setup GL so that we can render transparent textures
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glBindTexture(GL_TEXTURE_2D, mFont.mTexture);

				// render letters as small 2D quads using the font texture
				// room for improvement: make one quad instead of many small
				glBegin(GL_QUADS);
					for (unsigned int i = 0; i < str.length(); i++) {
						drawChar(str[i], x + i * (mFont.mCharWidth + mFont.mCharKerning), y);
					}
				glEnd();

			glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

void CGLFont::selectFont(char* font, float size)
{
	mFont.mTexture = Texture_Add(font);
	mFont.mCharHeight *= size;
	mFont.mCharWidth *= size;
}

void CGLFont::selectFont(int texture, float size)
{
	mFont.mTexture = texture;
	mFont.mCharHeight *= size;
	mFont.mCharWidth *= size;
}