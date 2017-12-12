#ifndef _GLWRAPPER_H
#define _GLWRAPPER_H

#include <stdlib.h>
#include <windows.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
//#include <gl\glu.h>										// Header File For The GLu32 Library
//#include <gl\glaux.h>									// Header File For The GLaux Library
//#include "glext.h"


class CGLDriverStates
{
public:
	typedef enum
	{
		CCW = 0,
		CW
	} CullMode_t;

	void		setCullMode(CullMode_t cullMode);
	CullMode_t	getCullMode() const;

protected:

private:
	CullMode_t	mCullMode;
};


#endif