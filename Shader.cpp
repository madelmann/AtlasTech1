
#include "Main.h"


// The function pointers for shaders
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB = NULL;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
PFNGLBINDPROGRAMARBPROC glBindProgramARB;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB;



void CShader::Bind(const char* name, const float* value, size_t size)
{
	if(mProgramObject != 0)
	{
		glUseProgramObjectARB(mProgramObject);

		unsigned int location = glGetUniformLocationARB(mProgramObject, name); /* This is EXPENSIVE and should be avoided. */

		if      (size == 1)  glUniform1fvARB(location, 1, value);
		else if (size == 2)  glUniform2fvARB(location, 1, value);
		else if (size == 3)  glUniform3fvARB(location, 1, value);
		else if (size == 4)  glUniform4fvARB(location, 1, value);
		else if (size == 9)  glUniformMatrix3fvARB(location, 1, false, value);
		else if (size == 16) glUniformMatrix4fvARB(location, 1, false, value);
	}

	return;
}

///////////////////////////////// LOAD TEXT FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads and returns a text file for our shaders
/////
///////////////////////////////// LOAD TEXT FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

string CShader::LoadTextFile(string strFile)
{
    // Open the file passed in
    ifstream fin(strFile.c_str());

    // Make sure we opened the file correctly
    if(!fin)
	return "";

    string strLine = "";
    string strText = "";

    // Go through and store each line in the text file within a "string" object
    while(getline(fin, strLine))
    {
		strText = strText + "\n" + strLine;
    }

    // Close our file
    fin.close();

    // Return the text file's data
    return strText;
}


///////////////////////////////// INIT SHADERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a vertex and fragment shader file
/////
///////////////////////////////// INIT SHADERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::InitShaders(string strVertex, string strFragment)
{
    // These will hold the shader's text file data
    string strVShader, strFShader;

    // Make sure the user passed in a vertex and fragment shader file
    if(!strVertex.length() || !strFragment.length())
	    return;

    // If any of our shader pointers are set, let's free them first.
    if(mVertexShader || mFragmentShader || mProgramObject)
	{
		// UGLY BUG!!!: if shader pointers are released CGLFont stops working!
		//Release();
	}

    // Here we get a pointer to our vertex and fragment shaders
    mVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    mFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Now we load the shaders from the respective files and store it in a string.
    strVShader = LoadTextFile(strVertex.c_str());
    strFShader = LoadTextFile(strFragment.c_str());

    // Do a quick switch so we can do a double pointer below
	const char *szVShader = strVShader.c_str();
    const char *szFShader = strFShader.c_str();

    // Now this assigns the shader text file to each shader pointer
    glShaderSourceARB(mVertexShader, 1, &szVShader, NULL);
    glShaderSourceARB(mFragmentShader, 1, &szFShader, NULL);

	GLsizei infoLogLength = 0;
	GLcharARB infoLog = NULL;

    // Now we actually compile the shader's code
    glCompileShaderARB(mFragmentShader);

	glGetObjectParameterivARB(mFragmentShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
	glGetInfoLogARB(mFragmentShader, 255, NULL, &infoLog);

	Console->Output("Fragment Shader: \n%s", &infoLog);

	glCompileShaderARB(mVertexShader);

	glGetObjectParameterivARB(mVertexShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
	glGetInfoLogARB(mVertexShader, 255, NULL, &infoLog);

	Console->Output("Vertex Shader: \n%s", &infoLog);

    // Next we create a program object to represent our shaders
    mProgramObject = glCreateProgramObjectARB();

    // We attach each shader we just loaded to our program object
    glAttachObjectARB(mProgramObject, mVertexShader);
    glAttachObjectARB(mProgramObject, mFragmentShader);

    // Our last init function is to link our program object with OpenGL
    glLinkProgramARB(mProgramObject);


	glGetObjectParameterivARB(mProgramObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
	glGetInfoLogARB(mProgramObject, 255, NULL, &infoLog);

	Console->Output("%s", &infoLog);
}


///////////////////////////////// GET VARIABLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a variable ID for a shader variable
/////
///////////////////////////////// GET VARIABLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

GLint CShader::GetVariable(string strVariable)
{
    // If we don't have an active program object, let's return -1
    if(!mProgramObject)
	    return -1;

    // This returns the variable ID for a variable that is used to find
    // the address of that variable in memory.
    return glGetUniformLocationARB(mProgramObject, strVariable.c_str());
}


///////////////////////////////// RELEASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function frees all of our shader data
/////
///////////////////////////////// RELEASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::Release()
{
    // If our vertex shader pointer is valid, free it
    if(mVertexShader)
    {
	    glDetachObjectARB(mProgramObject, mVertexShader);
	    glDeleteObjectARB(mVertexShader);
	    mVertexShader = NULL;
    }

    // If our fragment shader pointer is valid, free it
    if(mFragmentShader)
    {
	    glDetachObjectARB(mProgramObject, mFragmentShader);
	    glDeleteObjectARB(mFragmentShader);
	    mFragmentShader = NULL;
    }

    // If our program object pointer is valid, free it
    if(mProgramObject)
    {
	    glDeleteObjectARB(mProgramObject);
	    mProgramObject = NULL;
    }
}



CShaderManager::CShaderManager()
: mUseShaders(false)
{
	mActive.clear();
	mCount = 0;
	mShaders.clear();

	Init();
}

CShaderManager::~CShaderManager()
{
	for(vector<CShader*>::iterator i = mShaders.begin(); i != mShaders.end(); i += 1)
	{
		delete (*i);
	}

	mActive.clear();
	mCount = 0;
	mShaders.clear();
}

CShader* CShaderManager::Add(char* Filename)
{
	if ( !mUseShaders ) {
		Console->Output("shaders disabled");
		return 0;
	}
	if ( strlen(Filename) <= 0 ) {
		return 0;
	}

	if ( Exists(Filename) != -1 ) {
		return mShaders[Exists(Filename)];
	}

	FILE *pFile = NULL;

	char frag[255] = "";
	sprintf(frag, "%sMaps\\%s\\Shaders\\%s\\shader.frag", g_sGameName, Game.sMapName, Filename);
	char vert[255] = "";
	sprintf(vert, "%sMaps\\%s\\Shaders\\%s\\shader.vert", g_sGameName, Game.sMapName, Filename);

	// Open a file pointer to the BMP file and check if it was found and opened 
	if(fopen_s(&pFile, frag, "rb") != NULL) 
	{
		sprintf(frag, "%sShaders\\%s\\shader.frag", g_sGameName, Filename);
		sprintf(vert, "%sShaders\\%s\\shader.vert", g_sGameName, Filename);

		if(fopen_s(&pFile, frag, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			sprintf(frag, "ERROR[CreateShader]: '%s' not found!\n", Filename);
			Console->Output(frag);
			return NULL;
		}
	}
	fclose(pFile);

	string fragshader = frag;
	string vertshader = vert;

	mShaders.push_back(new CShader(Filename, vertshader, fragshader));
	mCount++;

	return mShaders.back();
}

bool CShaderManager::Begin()
{
	if ( mActive.empty() )
		return false;

	mShaders[mActive.back()]->TurnOn();
	return true;
}

bool CShaderManager::Begin(char* Name)
{
	int active = Exists(Name);
	if(active == -1)
		return false;

	mActive.push_back(active);

	return Begin();
}

void CShaderManager::Bind(const char* name, const float* value, size_t size)
{
	if ( mActive.empty() ) {
		return;
	}

	mShaders[mActive.back()]->Bind(name, value, size);
}

void CShaderManager::Clear()
{
	mActive.clear();
	mCount = 0;
	mShaders.clear();
}

GLhandleARB CShaderManager::GetProgram()
{
	if ( mActive.empty() ) {
		return 0;
	}

	return mShaders[mActive.back()]->GetProgram();
}

void CShaderManager::End()
{
	if ( mActive.empty() ) {
		return;
	}

	mShaders[mActive.back()]->TurnOff();
	mActive.pop_back();
}

int CShaderManager::Exists(char* Filename)
{
	if(strlen(Filename) <= 0)
		return -1;

	for(unsigned int i = 0; i < mShaders.size(); i += 1)
	{
		if(strcmpi(Filename, mShaders[i]->GetName()) == 0)
			return i;
	}

	return -1;
}

CShader* CShaderManager::GetActive()
{
	if ( mActive.empty() ) {
		return 0;
	}

	return mShaders[mActive.back()];
}

///////////////////////////////////// INIT GLSL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes all of our GLSL functions and checks availability.
/////
///////////////////////////////////// INIT GLSL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CShaderManager::Init()
{
	// This grabs a list of all the video card's extensions it supports
	char *szGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	// Make sure find the GL_ARB_shader_objects extension so we can use shaders.
	if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
	{
		MessageBox(g_hWnd, "GL_ARB_shader_objects extension not supported!", "Error", MB_OK);
		return false;
	}

	// Make sure we support the GLSL shading language 1.0
	if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
	{
		MessageBox(g_hWnd, "GL_ARB_shading_language_100 extension not supported!", "Error", MB_OK);
		return false;
    }

	// Now let's set all of our function pointers for our extension functions
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniform2iARB = (PFNGLUNIFORM2IARBPROC)wglGetProcAddress("glUniform2iARB");
	glUniform3iARB = (PFNGLUNIFORM3IARBPROC)wglGetProcAddress("glUniform3iARB");
	glUniform4iARB = (PFNGLUNIFORM4IARBPROC)wglGetProcAddress("glUniform4iARB");
	glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
	glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2fvARB");
	glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3fvARB");
	glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4fvARB");
	glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress("glUniformMatrix3fvARB");
	glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
	glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
	glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
	glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)wglGetProcAddress("glProgramLocalParameter4fARB");
	glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
	glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");

	// Return a success!
	return true;
}

bool CShaderManager::useShaders() const
{
	return mUseShaders;
}

void CShaderManager::useShaders(bool state)
{
	mUseShaders = state;
}
