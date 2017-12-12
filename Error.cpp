#include <iostream>
#include <GL\gl.h>

#include "Error.h"


const char* noerror = "No Error";
const char* unknown = "Unkown Error";
const char* invenum = "Unacceptable enumerated value Specified";
const char* invval = "A numarical argument is out of range";
const char* invoper = "The instruction is not allowed in the current state";
const char* stackoverflow = "The command will cause a stack overflow";
const char* stackunderflow = "The command will cause a stack underflow";
const char* outofmem = "Not enough memory available to execute command";
const char* tablelarge = "The specified table is too large";


GLenum GLError::lasterror;


int GLError::Check()
{
	lasterror = glGetError();
	return (lasterror != GL_NO_ERROR);
}

const char* GLError::Name()
{
	switch (lasterror)
	{
		case(GL_NO_ERROR): 
			return noerror;
		case(GL_INVALID_ENUM):
			return invenum;
		case(GL_INVALID_VALUE):
			return invval;
		case(GL_INVALID_OPERATION):
			return invoper;
		case(GL_STACK_OVERFLOW):
			return stackoverflow;
		case(GL_STACK_UNDERFLOW):
			return stackunderflow;
		case(GL_OUT_OF_MEMORY):
			return outofmem;
		//case(GL_TABLE_TOO_LARGE):
		//	return tablelarge;
		default:
			return unknown;
	}
}

void GLError::Silent(char* filename, int linenumber)
{
	lasterror = glGetError();

	if (lasterror != GL_NO_ERROR)
		std::cout << filename << ":" << linenumber << " GL ERROR: " << Name() << std::endl;
}