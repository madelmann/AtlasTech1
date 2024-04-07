
// Library includes

// Project includes
#include "Main.h"
#include "Water.h"


// This is used for the scaling of our normal map texture.
// This makes it so the water doesn't look so jittery, but
// more smooth in it's waves.
const float kNormalMapScale = 0.25f;
extern int g_TextureSize;


CWater::CWater()
{
	BoundingVolumeType	= BoundingVolume::Box;
	mCollisionVolumeType = BoundingVolume::None;
	VisibleVolumeType	= BoundingVolume::Box;
}

CWater::~CWater()
{
}

/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CWater::AnimateNextFrame(int desiredFrameRate)
{
	static double lastTime = 0.0f;
	double elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
    double currentTime = timeGetTime() * 0.001; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
    if( elapsedTime > (1.0 / desiredFrameRate) )
    {
		// Reset the last time
        lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
        return true;
    }

	// We don't animate right now.
	return false;
}

void CWater::RenderCaustics(float waterHeight, float causticScale)
{
	// How this function works is that we need to turn on a new
	// texture unit for multi-texturing the caustics over the terrain.
	// Since the first 2 units are already used for the terrain and
	// detail texture, we choose GL_TEXTURE2_ARB (unit 3).  We then
	// restrict the animation to 30 frames per second so it doesn't
	// animate too fast.  This frame rate of course would change
	// depending on the water flow.
	//
	// We then scale the texture matrix for our caustic texture,
	// right before we render the terrain again.  We use a clipping
	// plane to only render the bottom part of the terrain because
	// that is all that needs to be rendered with caustics.  In Main.cpp
	// I describe a better and faster way to do this than how we have it
	// here.

	// Activate the third texture ID and bind the caustic texture to it
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glEnable(GL_TEXTURE_2D);

	// Below we increase our caustic texture at a rate of 30 frames per second.
	// This simulates light from the water's surface.

	// Start the first index at 0 ("caust00") for our current caustic texture;
	static int startIndex = 0;

	// Bind the current caustic texture to our terrain
	glBindTexture(GL_TEXTURE_2D, g_Manager_Texture->IndexOf("Caustics\\caust00.bmp") + startIndex);

	// We create a frame counter so that we only render the caustics every
	// 5 frames that we draw.  This way it slows the animation down.
	static int frameCount = 0;

	// If the frame count reached 5, let's increase the current caustic bitmap.
	if(frameCount == 5)
	{
		startIndex = ((startIndex + 1) % NUM_WATER_TEX);
		frameCount = 0;
	}

	// Increase the frame count every frame
	frameCount++;

	// Just like we did with the detail texture for the terrain, we
	// scale the caustic texture by our inputted scale value.
	glMatrixMode(GL_TEXTURE);

		// Reset the current matrix and apply our chosen scale value
		glLoadIdentity();
		glScalef(causticScale, causticScale, 1);

	// Leave the texture matrix and put us back in the model view matrix
	glMatrixMode(GL_MODELVIEW);

	// Next, we need to render the terrain with caustic effects, but only
	// the bottom part of the terrain (clip the top part)

	// Create the plane equation for the part we want to keep (bottom)
	double plane[4] = {0.0, -1.0, 0.0, waterHeight};
	//double plane[4] = {-2048, -1.0, -2048, waterHeight};

	// Turn on a clip plane and set the clipping equation
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Render the bottom of the terrain with caustics
	QuadTree.Render();

	// Turn clipping off
	glDisable(GL_CLIP_PLANE0);

	// Reset the texture matrix
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Turn the third multi-texture pass off
	glActiveTextureARB(GL_TEXTURE4_ARB);
    glDisable(GL_TEXTURE_2D);
}


/////////////////////////// CREATE REFLECTION TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders our world to a texture for our reflection
/////
/////////////////////////// CREATE REFLECTION TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*

void CWater::CreateReflectionTexture(float waterHeight, int textureSize)
{
	// To create the reflection texture we just need to set the view port
	// to our texture map size, then render the current scene our camera
	// is looking at to the already allocated texture unit.  Since this
	// is a reflection of the top of the water surface we use clipping 
	// planes to only render the top of the world as a reflection.  If
	// we are below the water we don't flip the reflection but just use
	// the current view of the top as we are seeing through the water.
	// When you look through water at the surface it isn't really reflected,
	// only when looking down from above the water on the surface.

	// Change the view port to be the size of the texture we will render to
    glViewport(0, 0, textureSize, textureSize);

	// Clear the color and depth bits, reset the matrix and position our camera.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

	glPushMatrix();
		// If our camera is above the water we will render the scene flipped upside down.
		// In order to line up the reflection nicely with the world we have to translate
		// the world to the position of our reflected surface, multiplied by two.
		if(g_Camera.Position().y > waterHeight)
		{
			// Translate the world, then flip it upside down
			glTranslatef(0.0f, waterHeight * 2.0f, 0.0f);
			glScalef(1.0, -1.0, 1.0);

			// Set our plane equation and turn clipping on
			double plane[4] = {0.0, 0.5, 0.0, -waterHeight};
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);

			g_iSkyBoxFaceCulling = GL_FRONT;

			// Render the world upside down and clipped (only render the top flipped).
			Scene.BeginRenderWater();
				RenderWorld();
			Scene.EndRenderWater();

			g_iSkyBoxFaceCulling = GL_BACK;

			// Turn clipping off
			glDisable(GL_CLIP_PLANE0);
		}
		else
		{
			RenderWorld();
		}
	glPopMatrix();

    // Bind the current scene to our reflection texture
	glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);
}


/////////////////////////// CREATE REFRACTION DEPTH TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders our world to a texture to do the reflection and depth calculations
/////
/////////////////////////// CREATE REFRACTION DEPTH TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*

void CWater::CreateRefractionDepthTexture(float waterHeight, int textureSize)
{
	// To create the refraction and depth textures we do the same thing
	// we did for the reflection texture, except we don't need to turn
	// the world upside down.  We want to find the depth of the water,
	// not the depth of the sky and above water terrain.

 	// Change the view port to be the size of the texture we will render to
    glViewport(0, 0, textureSize, textureSize);

	// Clear the color and depth bits, reset the matrix and position our camera.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

	glPushMatrix();
		// If our camera is above the water we will render only the parts that
		// are under the water.  If the camera is below the water we render
		// only the stuff above the water.  Like the reflection texture, we
		// incorporate clipping planes to only render what we need.

		// If the camera is above water, render the data below the water
		if(g_Camera.Position().y > waterHeight)
		{
			double plane[4] = {0.0, -1.0, 0.0, waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld();
			glDisable(GL_CLIP_PLANE0);
		}
		// If the camera is below the water, only render the data above the water
		else
		{
			glCullFace(GL_FRONT);
			double plane[4] = {0.0, 1.0, 0.0, -waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld();
			glDisable(GL_CLIP_PLANE0);
			glCullFace(GL_BACK);
		}
    glPopMatrix();

    // Bind the current scene to our refraction texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);

    // Bind the current scene to our depth texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, textureSize, textureSize, 0);
}

void CWater::SetBoundingBox()
{
	BoundingVolume.X1 = QuadTree.fTerrainXStart;
	BoundingVolume.Y1 = (Map.fWaterHeightBase - Map.fWaveHeight - 1.0f);
	BoundingVolume.Z1 = QuadTree.fTerrainZStart;
	BoundingVolume.X2 = QuadTree.fTerrainXEnd;
	BoundingVolume.Y2 = (Map.fWaterHeightBase + Map.fWaveHeight + 1.0f);
	BoundingVolume.Z2 = QuadTree.fTerrainZEnd;
	BoundingVolume.CX = (BoundingVolume.X2 - BoundingVolume.X1) / 2;
	BoundingVolume.CY = (BoundingVolume.Y2 - BoundingVolume.Y1) / 2;
	BoundingVolume.CZ = (BoundingVolume.Z2 - BoundingVolume.Z1) / 2;

	BoundingVolume.Radius = ((BoundingVolume.X2 - BoundingVolume.X1) / 2 + (BoundingVolume.Y2 - BoundingVolume.Y1) / 2 + (BoundingVolume.Z2 - BoundingVolume.Z1) / 2) / 3;
	fRadius = BoundingVolume.Radius;
}

///////////////////////////////// RENDER WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the flat-water surface as a quad
/////
///////////////////////////////// RENDER WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CWater::Render()
{
	if(!Frustum.volumeInFrustum(BoundingVolume))
		return;

	if(!Game.bPlayMode)
		RenderBoundingVolume();

	// Turn on the first texture unit and bind the REFLECTION texture
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);

	// Turn on the second texture unit and bind the REFRACTION texture
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);

	// Turn on the third texture unit and bind the NORMAL MAP texture
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[NORMAL_ID]);

	// Turn on the fourth texture unit and bind the DUDV MAP texture
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[DUDVMAP_ID]);

	// Turn on the fifth texture unit and bind the DEPTH texture
    glActiveTextureARB(GL_TEXTURE4_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);

	// Below is where we draw our quad for the water.  Before we start
	// giving out UV coordinates for each texture unit, we need to create
	// some variables to store each one.  The dudv and depth map has it's
	// UV coordinates calculated in the g_Manager_Shader.  This is why below we set
	// the UV coordinates to 0 to show they aren't set in the code.
	// When you run this application you will notice that the water is moving.
	// To give this effect we create a static float call "move" that slowly
	// increases by our g_WaterFlow variable.  To give the water the wavy
	// effect we move the refraction map in the opposite direction
	// of the normal map.  This is very important to the effect.

	// Create a static variable for the movement of the water
	static float move = 0.0f;

	// Use this variable for the normal map and make it slower
	// than the refraction map's speed.  We want the refraction
	// map to be jittery, but not the normal map's waviness.
	float move2 = move * kNormalMapScale;

	// Set the refraction map's UV coordinates to our global g_WaterUV
	float refrUV = g_WaterUV;

	// Set our normal map's UV scale and shrink it by kNormalMapScale
	float normalUV = g_WaterUV * kNormalMapScale;

	// Move the water by our global speed
	move += Map.fWaterFlow * g_fElpasedTime;

	g_Manager_Shader->Begin("Water");

		GLint uniform;
		// Set the variable "reflection" to correspond to the first texture unit
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "reflection"); 
		glUniform1iARB(uniform, REFLECTION_ID); //second paramter is the texture unit 

		// Set the variable "refraction" to correspond to the second texture unit
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "refraction");
		glUniform1iARB(uniform, REFRACTION_ID); 

		// Set the variable "normalMap" to correspond to the third texture unit
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "normalMap");
		glUniform1iARB(uniform, NORMAL_ID);

		// Set the variable "dudvMap" to correspond to the fourth texture unit
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "dudvMap"); 
		glUniform1iARB(uniform, DUDVMAP_ID);

		// Set the variable "depthMap" to correspond to the fifth texture unit
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "depthMap");
		glUniform1iARB(uniform, DEPTH_ID); 

		// Give the variable "waterColor" a blue color
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "waterColor");
		//glUniform4fARB(uniform, 0.1f, 0.1f, 0.2f, 1.0f);
		glUniform4fARB(uniform, Map.vWaterColor.x, Map.vWaterColor.y, Map.vWaterColor.z, 1.0f);

		// We don't use lighting in this tutorial, but we do need to calculate
		// the diffuse and specular lighting on the water to increase realism.
		CVector3 lightPos = Light[LIGHT_SUN].getPosition();

		// Give the variable "lightPos" our light position
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "lightPos");
		glUniform4fARB(uniform, lightPos.x, lightPos.y * -1, lightPos.z, (GLfloat)Light[LIGHT_SUN].getMode()); 

		// Store the camera position in a variable
		CVector3 camera = g_Camera.Position();

		// Give the variable "cameraPos" our camera position
		uniform = glGetUniformLocationARB(g_Manager_Shader->GetProgram(), "cameraPos");
		glUniform4fARB(uniform, camera.x, camera.y, camera.z, 1.0f);

		// Draw our huge water quad
		glBegin(GL_QUADS);
			// The back left vertice for the water
			if(Map.bWaterReflections)
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);							// Reflection texture
			if(Map.bWaterRefractions)
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, refrUV - move);					// Refraction texture
			if(Map.bWaterNormals)
				glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, normalUV + move2);				// Normal map texture
			//glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0.0f, 0.0f);							// DUDV map texture
			//glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0.0f, 0.0f);							// Depth texture
			glVertex3f(QuadTree.fTerrainXStart, Map.fWaterHeight, QuadTree.fTerrainZStart);	// 0, 0

			// The front left vertice for the water
			if(Map.bWaterReflections)
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, g_WaterUV);						// Reflection texture
			if(Map.bWaterRefractions)
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f - move);					// Refraction texture
			if(Map.bWaterNormals)
				glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f + move2);					// Normal map texture
			//glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0.0f, g_WaterUV);						// DUDV map texture
			//glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0.0f, g_WaterUV);						// Depth texture
			glVertex3f(QuadTree.fTerrainXEnd, Map.fWaterHeight, QuadTree.fTerrainZStart);	// 1, 0

			// The front right vertice for the water
			if(Map.bWaterReflections)
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, g_WaterUV);				// Reflection texture
			if(Map.bWaterRefractions)
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, 0.0f - move);					// Refraction texture
			if(Map.bWaterNormals)
				glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, 0.0f + move2);				// Normal map texture
			//glMultiTexCoord2fARB(GL_TEXTURE3_ARB, g_WaterUV, g_WaterUV);					// DUDV map texture
			//glMultiTexCoord2fARB(GL_TEXTURE4_ARB, g_WaterUV, g_WaterUV);					// Depth texture
			glVertex3f(QuadTree.fTerrainXEnd, Map.fWaterHeight, QuadTree.fTerrainZEnd);		// 1, 1

			// The back right vertice for the water
			if(Map.bWaterReflections)
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, 0.0f);						// Reflection texture
			if(Map.bWaterRefractions)
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, refrUV - move);				// Refraction texture
			if(Map.bWaterNormals)
				glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, normalUV + move2);			// Normal map texture
			//glMultiTexCoord2fARB(GL_TEXTURE3_ARB, g_WaterUV, 0.0f);						// DUDV map texture
			//glMultiTexCoord2fARB(GL_TEXTURE4_ARB, g_WaterUV, 0.0f);						// Depth texture
			glVertex3f(QuadTree.fTerrainXStart, Map.fWaterHeight, QuadTree.fTerrainZEnd);	// 0, 1
		glEnd();
	g_Manager_Shader->End();

	// Turn the fifth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE4_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the fourth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the third multi-texture pass off
	glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the second multi-texture pass off
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Reset multi texture to first texture slot
	glActiveTextureARB(GL_TEXTURE0_ARB);


	//// Draw our huge foam/surge quad
	//glBegin(GL_QUADS);
	//	glVertex3f(g_Manager_Octree.fTerrainXStart, waterHeight + 0.05f, g_Manager_Octree.fTerrainZStart);	// 0, 0

	//	glVertex3f(g_Manager_Octree.fTerrainXEnd, waterHeight + 0.05f, g_Manager_Octree.fTerrainZStart);	// 1, 0

	//	glVertex3f(g_Manager_Octree.fTerrainXEnd, waterHeight + 0.05f, g_Manager_Octree.fTerrainZEnd);		// 1, 1

	//	glVertex3f(g_Manager_Octree.fTerrainXStart, waterHeight + 0.05f, g_Manager_Octree.fTerrainZEnd);	// 0, 1
	//glEnd();
}