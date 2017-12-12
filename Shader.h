#ifndef _CSHADER_H
#define _CSHADER_H


#include "Main.h"


// This is a define that we use for our function pointers
#define APIENTRYP APIENTRY *

// Here we include the vertex and fragment shader defines
#define GL_VERTEX_SHADER_ARB              0x8B31
#define GL_FRAGMENT_SHADER_ARB            0x8B30

#define GL_VERTEX_PROGRAM_ARB             0x8620
#define GL_FRAGMENT_PROGRAM_ARB           0x8804

// This is what GL uses for handles when using extensions
typedef unsigned int GLhandleARB;
typedef char GLcharARB;

// Below are all of our function pointer typedefs for all the extensions we need
typedef GLhandleARB (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC)	(GLenum shaderType);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC)			(GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC)			(GLhandleARB shaderObj);
typedef GLhandleARB (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC)	(void);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC)			(GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC)			(GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC)		(GLhandleARB programObj);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC)	(GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC)			(GLhandleARB containerObj, GLhandleARB attachedObj);
typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC)			(GLhandleARB obj);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FARBPROC)	(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC)			(GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLUNIFORM1FARBPROC)			(GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM1FVARBPROC)			(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM1IARBPROC)			(GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1IVARBPROC)			(GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRYP PFNGLUNIFORM2FARBPROC)			(GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM2FVARBPROC)			(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM2IARBPROC)			(GLint location, GLint v0, GLint v1);
typedef void (APIENTRYP PFNGLUNIFORM2IVARBPROC)			(GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRYP PFNGLUNIFORM3FARBPROC)			(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM3FVARBPROC)			(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM3IARBPROC)			(GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRYP PFNGLUNIFORM3IVARBPROC)			(GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRYP PFNGLUNIFORM4FARBPROC)			(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM4FVARBPROC)			(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM4IARBPROC)			(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRYP PFNGLUNIFORM4IVARBPROC)			(GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVARBPROC)	(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVARBPROC)	(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVARBPROC)	(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// Here we extern our functions to be used elsewhere
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORM2IARBPROC glUniform2iARB;
extern PFNGLUNIFORM3IARBPROC glUniform3iARB;
extern PFNGLUNIFORM4IARBPROC glUniform4iARB;
extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
extern PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
extern PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
extern PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC glUniform4fvARB;
extern PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB;
extern PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;

// This is our very basic shader class that we will use
class CShader
{
public:
	// Create an empty constructor and have the deconstructor release our memory.
	CShader()
	{
		mName = "";
	}
	CShader(char* name, string strVertex, string strFragment)
	{
		mName = name;
		InitShaders(strVertex, strFragment);
	}
	~CShader()
	{
		Release();
	}

	void begin()	{	glUseProgramObjectARB(mProgramObject); }
	void end()		{	glUseProgramObjectARB(0);				 }

	void Bind(const char* name, const float* value, size_t size);

	char* GetName()
	{
		return mName;
	}

	// This loads our text file for each shader and returns it in a string
	string LoadTextFile(string strFile);

	// This loads a vertex and fragment shader
	void InitShaders(string strVertex, string strFragment);

	// This returns an ID for a variable in our shader
	GLint GetVariable(string strVariable);

	// These are our basic get functions for our private data
	GLhandleARB GetProgram()	{	return mProgramObject; }
	GLhandleARB GetVertexS()	{	return mVertexShader; }
	GLhandleARB GetFragmentS()	{	return mFragmentShader; }

	// Below are functions to set an integer or a set of floats
	void SetInt(GLint variable, int newValue)								{ glUniform1iARB(variable, newValue);		}
	void SetFloat(GLint variable, float newValue)							{ glUniform1fARB(variable, newValue);		}
	void SetFloat2(GLint variable, float v0, float v1)						{ glUniform2fARB(variable, v0, v1);			}
	void SetFloat3(GLint variable, float v0, float v1, float v2)			{ glUniform3fARB(variable, v0, v1, v2);		}
	void SetFloat4(GLint variable, float v0, float v1, float v2, float v3)	{ glUniform4fARB(variable, v0, v1, v2, v3);	}

	void SetName(char* name)
	{
		mName = name;
	}

	// These 2 functions turn on and off our shader
	void TurnOn()		{	glUseProgramObjectARB(mProgramObject); }
	void TurnOff()		{	glUseProgramObjectARB(0);				 }

	// This releases our memory for our shader
	void Release();

private:

	char*		mName;

	// This handle stores our vertex shader information
	GLhandleARB mVertexShader;

	// This handle stores our fragment shader information
	GLhandleARB mFragmentShader;

	// This handle stores our program information which encompasses our shader
	GLhandleARB mProgramObject;
};


class CShaderManager : public CSingleton< CShaderManager >
{
public:
	CShaderManager();
	~CShaderManager();

	bool useShaders() const;
	void useShaders(bool state);

	CShader* Add(char* Filename);
	bool Begin();
	bool Begin(char* Name);
	void Bind(const char* name, const float* value, size_t size);
	void Clear();
	GLhandleARB GetProgram();
	void End();
	int Exists(char* Filename);
	CShader* GetActive();
	bool Init();

private:
	vector<int>			mActive;
	int					mCount;
	vector<CShader*>	mShaders;
	bool				mUseShaders;
};


#endif