#include "GLWrapper.h"


void CGLDriverStates::setCullMode(CGLDriverStates::CullMode_t cullMode)
{
	glCullFace(cullMode == CCW ? GL_BACK : GL_FRONT);
	mCullMode = cullMode;
}

CGLDriverStates::CullMode_t CGLDriverStates::getCullMode() const
{
	return mCullMode;
}
