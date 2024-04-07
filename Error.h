
#pragma once

// Library includes

// Project includes
#include "GLWrapper.h"


#define CHECKGLSILENT GLError::Silent(__FILE__,__LINE__);


namespace GLError
{
	int Check();
	const char* Name();
	void Silent(char* filename, int linenumber);

	extern GLenum lasterror;
}
