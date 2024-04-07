
#pragma once

// Library includes
#include <stdlib.h>

// Project includes
#include "GLWrapper.h"
#include "Tools.h"


class CLight
{
public:
	// Three modes for the light
	typedef enum
	{
		DirectionalLight = 0,					// The light is directional.
		PointLight,								// The light is a point.
		SpotLight								// The light is a spotlight with a cone.
	} LightMode_t;

	CLight();
	~CLight();

	void clear();
	void setLight();
	void setNoAttenuation();
	void setupAttenuation(float farAttenuationBegin, float farAttenuationEnd);
	void setupSpotExponent(float hotSpotAngle);
	/// Quick setup a directional light
	void setupDirectional(const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular, const CVector3& direction,
						  float constant = 1, float linear = 0, float quadratic = 0);

	/// Quick setup a point light
	void setupPointLight(const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular, const CVector3& position,
						 const CVector3& direction, float constant = 1, float linear = 0, float quadratic = 0);

	/// Quick setup a spotlight
	void setupSpotLight(const CRGBA& ambiant, const CRGBA& diffuse, const CRGBA& specular,
						const CVector3& position, const CVector3& direction, float exponent,
						float cutoff, float constant = 1, float linear = 0, float quadratic = 0);

	void disable() { mEnabled = false; }
	void enable() { mEnabled = true; }
	bool getEnabled() { return mEnabled; };
	void setAmbiant(const CRGBA &ambiant) { mAmbiant = ambiant; update(); }
	const CRGBA& getAmbiant() { return mAmbiant; }
	void setCutoff(float cutoff) { mCutOff = cutoff; update(); }
	float getCutoff() { return mCutOff; }
	void setDiffuse(const CRGBA &diffuse) { mDiffuse = diffuse; update(); };
	const CRGBA& getDiffuse() { return mDiffuse; }
	void setDirection(const CVector3 &direction) { mDirection = direction; update(); }
	const CVector3& getDirection() { return mDirection; }
	void setExponent(float exponent) { mExponent = exponent; update(); }
	float getExponent() { return mExponent; }
	void setIndex(int index) { mIndex = index; }
	int getIndex() { return mIndex; }
	void setPosition(const CVector3 &position) { mPosition = position; update(); }
	const CVector3& getPosition() { return mPosition; }
	void setSpecular(const CRGBA &specular) { mSpecular = specular; update(); }
	const CRGBA& getSpecular() { return mSpecular; }
	/* Attenuation factors */
	void setAttenuationConstant(float constant) { mAttenuationConstant = constant; update(); }
	float getAttenuationConstant() { return mAttenuationConstant; }
	void setAttenuationLinear(float linear) { mAttenuationLinear = linear; update(); }
	float getAttenuationLinear() { return mAttenuationLinear; }
	void setAttenuationQuadratic(float quadratic) { mAttenuationQuadratic = quadratic; update(); }
	float getAttenuationQuadratic() { return mAttenuationQuadratic; }

	/**
	 * Set the light mode.
	 */
	void setMode(const LightMode_t mode) { mMode = mode; update(); }

	/**
	  * Get the light mode.
	  */
	LightMode_t getMode() const { return mMode; }

	void update();

private:

	CRGBA		mAmbiant;
	float		mAttenuationConstant;
	float		mAttenuationLinear;
	float		mAttenuationQuadratic;
	float		mCutOff;
	CRGBA		mDiffuse;
	CVector3	mDirection;
	bool		mEnabled;
	float		mExponent;
	uint		mIndex;
	LightMode_t	mMode;
	CVector3	mPosition;
	CRGBA		mSpecular;
};
