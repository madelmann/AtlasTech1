
#pragma once

// Library includes

// Project includes
#include "Object.h"


class CWater : public CGraphicObject
{
public:
	CWater();
	~CWater();

	// This clamps our animation to a frame rate (like 30 frames)
	bool AnimateNextFrame(int desiredFrameRate);

	// This renders our screen to a texture that is used for reflection
	void CreateReflectionTexture(float waterHeight, int textureSize);

	// This renders our screen to a texture for refraction and depth
	void CreateRefractionDepthTexture(float waterHeight, int textureSize);

	void SetBoundingBox();

	// This draws our quad for the water
	void Render();

	// This cycles through our caustic bitmaps to produce the animation
	void RenderCaustics(float waterHeight, float causticScale);
};

