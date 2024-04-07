
#pragma once

// Library includes

// Project includes
#include <windows.h>
#include <glew.h>
#include <gl/GL.h>									// Header File For The OpenGL32 Library
#include "GLExt.h"
#include <gl/GLU.h>									// Header File For The GLu32 Library
#include <glaux.h>									// Header File For The GLaux Library



// These are for our multi-texture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4

// These are used to increase the detail texture for our terrain
#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// We need to define this for glTexParameteri()
#define GL_CLAMP_TO_EDGE					0x812F								// This is for our skybox textures


// Here are the multitexture function prototypes
typedef void (APIENTRY* PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY* PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY* PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
// Here we extern our function pointer for volumetric fog positioning
typedef void (APIENTRY* PFNGLFOGCOORDFEXTPROC) (GLfloat coord);


/*
// Here we extern our function pointers for OpenGL
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;

// Here we extern our function pointer for volumetric fog positioning
extern PFNGLFOGCOORDFEXTPROC				glFogCoordfEXT;

// This declares our point sprite extension
extern PFNGLPOINTPARAMETERFARBPROC			glPointParameterfARB;
extern PFNGLPOINTPARAMETERFVARBPROC			glPointParameterfvARB;

// GL_ARB_occlusion_query - Begin
extern PFNGLGENQUERIESARBPROC				glGenQueriesARB;
extern PFNGLDELETEQUERIESARBPROC			glDeleteQueriesARB;
extern PFNGLISQUERYARBPROC					glIsQueryARB;
extern PFNGLBEGINQUERYARBPROC				glBeginQueryARB;
extern PFNGLENDQUERYARBPROC					glEndQueryARB;
extern PFNGLGETQUERYIVARBPROC				glGetQueryivARB;
extern PFNGLGETQUERYOBJECTIVARBPROC			glGetQueryObjectivARB;
extern PFNGLGETQUERYOBJECTUIVARBPROC		glGetQueryObjectuivARB;
// GL_ARB_occlusion_query - End
*/

extern PFNWGLEXTSWAPCONTROLPROC				glSwapIntervalEXT;
extern PFNWGLEXTGETSWAPINTERVALPROC			glGetSwapIntervalEXT;


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
typedef GLhandleARB(APIENTRYP PFNGLCREATESHADEROBJECTARBPROC)	(GLenum shaderType);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC)			(GLhandleARB shaderObj, GLsizei count, const GLcharARB** string, const GLint* length);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC)			(GLhandleARB shaderObj);
typedef GLhandleARB(APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC)	(void);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC)			(GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC)			(GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC)		(GLhandleARB programObj);
typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC)	(GLhandleARB programObj, const GLcharARB* name);
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

/*
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
*/



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

private:
	CullMode_t	mCullMode;
};

