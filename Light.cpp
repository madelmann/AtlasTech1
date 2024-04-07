
// Library includes

// Project includes
#include "Light.h"


CLight::CLight()
{
	clear();
}

CLight::~CLight()
{
}

void CLight::clear()
{
	mAmbiant				= CRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	mAttenuationConstant	= 1.0f;
	mAttenuationLinear		= 0.0f;
	mAttenuationQuadratic	= 0.0f;
	mCutOff					= 180.0f;
	mDiffuse				= CRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	mDirection				= CVector3(0.f, 0.f, 0.f);
	mEnabled				= false;
	mExponent				= 0.0f;
	mIndex					= 0;
	mMode					= DirectionalLight;
	mPosition				= CVector3(0.f, 0.f, 0.f);
	mSpecular				= CRGBA(0.f, 0.f, 0.f, 0.f);
}

void CLight::setLight()
{
	uint lightNum = GL_LIGHT0 + mIndex;

	glLightfv(lightNum, GL_CONSTANT_ATTENUATION,	&mAttenuationConstant);
	glLightfv(lightNum, GL_LINEAR_ATTENUATION,	&mAttenuationLinear);
	glLightfv(lightNum, GL_QUADRATIC_ATTENUATION,	&mAttenuationQuadratic);

	GLfloat tmp4[4];

	tmp4[0] = mAmbiant.R / 255.f;
	tmp4[1] = mAmbiant.G / 255.f;
	tmp4[2] = mAmbiant.B / 255.f;
	tmp4[3] = mAmbiant.A / 255.f;
	glLightfv(lightNum, GL_AMBIENT, tmp4);

	tmp4[0] = mDiffuse.R / 255.f;
	tmp4[1] = mDiffuse.G / 255.f;
	tmp4[2] = mDiffuse.B / 255.f;
	tmp4[3] = mDiffuse.A / 255.f;
	glLightfv(lightNum, GL_DIFFUSE, tmp4);

	tmp4[0] = mSpecular.R / 255.f;
	tmp4[1] = mSpecular.G / 255.f;
	tmp4[2] = mSpecular.B / 255.f;
	tmp4[3] = mSpecular.A / 255.f;
	glLightfv(lightNum, GL_SPECULAR, tmp4);

	tmp4[0] = mPosition.x;
	tmp4[1] = mPosition.y;
	tmp4[2] = mPosition.z;
	tmp4[3] = 0.f;
	if ( mMode != DirectionalLight ) {
		tmp4[3] = 0.f;
	}
	glLightfv(lightNum, GL_POSITION, tmp4);

	if ( mMode == SpotLight ) {
		// Get the cutoff of the spot
		float cutoff = 180.f * (mCutOff / fPi);
		glLightfv(lightNum, GL_SPOT_CUTOFF, &cutoff);

		tmp4[0] = mDirection.x;
		tmp4[1] = mDirection.y;
		tmp4[2] = mDirection.z;
		glLightfv(lightNum, GL_SPOT_DIRECTION, tmp4);

		glLightfv(lightNum, GL_SPOT_EXPONENT, &mExponent);
	}
	else
	{
		// Disactive spot properties
		glLighti(lightNum, GL_SPOT_CUTOFF, 180);
		glLighti(lightNum, GL_SPOT_EXPONENT, 0);
	}

	if ( mEnabled ) {
		glEnable(GL_LIGHT0 + mIndex);
	}
	else {
		glDisable(GL_LIGHT0 + mIndex);
	}
}

void CLight::setNoAttenuation()
{
	mAttenuationConstant = 1.f;
	mAttenuationLinear = 0.f;
	mAttenuationQuadratic = 0.f;
}

void CLight::setupAttenuation(float farAttenuationBegin, float farAttenuationEnd)
{
	// limit case
	if(farAttenuationEnd <= 0)
	{
		mAttenuationConstant = 1000000;
		mAttenuationLinear = 0;
		mAttenuationQuadratic = 0;
	}
	else
	{
		// The following factors are "it feels good for farAttenuationBegin=0/farAttenuationEnd=1" factors.
		// btw, at r=farAttenuationEnd=1, att= 1/11 ~= 0.
		const float	constant = 1.0f;
		const float	linear = 0.f;
		const float	quadratic = 10.0f;

		/*
			With GL 'att=1/(c+l*r+q*r2)' formula, it is impossible to simulate correctly
			farAttenuationBegin (very big decrase if for instance farAttenuationBegin is near farAttenuationEnd),
			hence simulate it very badly by multiplying the farAttenuationEnd by some factor
		*/
		float	factor = 1.f;
		if(farAttenuationBegin / farAttenuationEnd>0.5f) {
			factor = 2.f;
		}
		else if(farAttenuationBegin > 0) {
			factor = 1.f + 2 * farAttenuationBegin / farAttenuationEnd;
		}
		farAttenuationEnd *= factor;

		// scale according to farAttenuationEnd.
		mAttenuationConstant = constant;
		mAttenuationLinear = linear / farAttenuationEnd;
		mAttenuationQuadratic = quadratic / SQR(farAttenuationEnd);
	}
}

void CLight::setupSpotExponent(float hotSpotAngle)
{
	float divid = (float)log(cos(hotSpotAngle));

	if ( divid == 0.f) {
		divid=0.0001f;
	}

	setExponent((float)(log(0.9) / divid));
}

/// Quick setup a directional light
void CLight::setupDirectional (const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular, const CVector3& direction,
							   float constant, float linear, float quadratic)
{
	// Set the mode
	setMode(DirectionalLight);

	// Set the colors
	setAmbiant(ambiant);
	setDiffuse(diffuse);
	setSpecular(specular);

	// Set the direction
	setDirection(direction);

	// Set attenuation
	setAttenuationConstant(constant);
	setAttenuationLinear(linear);
	setAttenuationQuadratic(quadratic);

	// Dummy to avoid uninit data, and problems of cache
	setPosition(CVector3());
	setExponent(0.f);
	setCutoff(0.f);
}

/// Quick setup a point light
void CLight::setupPointLight (const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular, const CVector3& position,
							  const CVector3& direction, float constant, float linear, float quadratic)
{
	// Set the mode
	setMode(DirectionalLight);

	// Set the colors
	setAmbiant(ambiant);
	setDiffuse(diffuse);
	setSpecular(specular);

	// Set the direction
	setDirection(direction);

	// Set attenuation
	setAttenuationConstant(constant);
	setAttenuationLinear(linear);
	setAttenuationQuadratic(quadratic);

	// Dummy to avoid uninit data, and problems of cache
	setPosition(CVector3());
	setExponent(0.f);
	setCutoff(0.f);
}

/// Quick setup a spotlight
void CLight::setupSpotLight (const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular,
							 const CVector3& position, const CVector3& direction, float exponent,
							 float cutoff, float constant, float linear, float quadratic)
{
	// Set the mode
	setMode(SpotLight);

	// Set the colors
	setAmbiant(ambiant);
	setDiffuse(diffuse);
	setSpecular(specular);

	// Set the position and direction
	setPosition(position);
	setDirection(direction);

	// Set spotlight parameters
	setExponent(exponent);
	setCutoff(cutoff);

	// Set attenuation
	setAttenuationConstant(constant);
	setAttenuationLinear(linear);
	setAttenuationQuadratic(quadratic);
}

void CLight::update()
{
	setLight();
}
