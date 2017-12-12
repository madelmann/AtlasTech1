#ifndef ERROR_HH
#define ERROR_HH


#include <GL/gl.h>


#define CHECKGLSILENT GLError::Silent(__FILE__,__LINE__);


namespace GLError
{
	int Check();
	const char* Name();
	void Silent(char* filename, int linenumber);

	extern GLenum lasterror;
}

#endif