#ifndef _FONT_H
#define _FONT_H


#include <string>
using std::string;


typedef struct
{
	float	mCharHeight;
	float	mCharKerning;
	float	mCharWidth;
	char*	mName;
	int		mTexture;
} Font_t;


class CGLFont
{
public:
	CGLFont();																	// Constructor.
	~CGLFont();																	// Destructor.

	void ClearGLFont();															// Delete font.
	int GetFontSize();															// Returns font's size
	bool Initialize(HDC hDC, int size, char *fontName);							// Initialize font.
	bool PrintText(char *string, float x, float y);
	bool PrintText(string text, float x, float y);								// Print the text.
	void SetWidthHeight(int w, int h);											// Set the window size.

	void drawString(const string &str, float x, float y);
	void selectFont(char* font, float size = 1.f);
	void selectFont(int texture, float size = 1.f);

protected:
	unsigned int InitFont(char *fontName, int fontSize, HDC hDC);				// Initialize list of text.

	unsigned int listBase;														// Store font.
	int Width;
	int Size;
	int Height;																	// Window width and height.

private:
	static void drawChar(char ch, float x, float y);

	Font_t	mFont;
};

#endif