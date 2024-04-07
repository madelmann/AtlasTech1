
#pragma once

// Library includes
#include <string>
#include <vector>

// Project includes
#include "GLWrapper.h"


// This is our very basic shader class that we will use
class CShader
{
public:
	// Create an empty constructor and have the deconstructor release our memory.
	CShader()
	{
		mName = "";
	}
	CShader(char* name, std::string strVertex, std::string strFragment)
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

	// This loads our text file for each shader and returns it in a std::string
	std::string LoadTextFile(std::string strFile);

	// This loads a vertex and fragment shader
	void InitShaders(std::string strVertex, std::string strFragment);

	// This returns an ID for a variable in our shader
	GLint GetVariable(std::string strVariable);

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


class CShaderManager
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
	std::vector<int>		mActive;
	int						mCount;
	std::vector<CShader*>	mShaders;
	bool					mUseShaders;
};

