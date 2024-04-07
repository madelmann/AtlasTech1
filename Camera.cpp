
// Library includes

// Project includes
#include "Camera.h"
#include "Game.h"
#include "Globals.h"
#include "GLWrapper.h"
#include "Normals.h"
#include "Tools.h"


// This is how fast our camera moves
#define kSpeed	32.0f


/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	// Calculate the cross product with the non communitive equation
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// Return the cross product
	return vNormal;										 
}

/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2) : Where V is the vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}

///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the class constructor
/////
///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// Init a vector to 0 0 0 for our position
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// Init a starting view vector (looking up and out the screen) 
	CVector3 vUp   = CVector3(0.0, 0.0, 1.0);		// Init a standard up vector (Rarely ever changes)

	iMode = CAMERA_MODE_NOTDEFINED;

	m_vPosition	= vZero;					// Init the position to zero
	m_vView		= vView;					// Init the view to a std starting view
	m_vUpVector	= vUp;						// Init the UpVector
}


///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the camera's position and view and up vector.
/////
///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	// The code above just makes it cleaner to set the variables.
	// Otherwise we would have to set each variable x y and z.

	m_vPosition = vPosition;					// Assign the position
	m_vView     = vView;						// Assign the view
	m_vUpVector = vUpVector;					// Assign the up vector
}


void CCamera::SetPosition(CVector3 NewPos)
{
	m_vPosition = NewPos;
}

void CCamera::SetSequencePosition(CVector3 NewPos)
{
	m_vSequencePosition = NewPos;
	Look();
}

void CCamera::SetSequenceView(CVector3 NewView)
{
	m_vSequenceView = NewView;
	Look();
}

void CCamera::SetView(CVector3 NewView)
{
	m_vView = NewView;
}


///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This rotates the view around the position using an axis-angle rotation
/////
///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// Get the view vector (The direction we are facing)
	CVector3 vView = m_vView - m_vPosition;		

	// Calculate the sine and cosine of the angle once
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// Find the new x position for the new rotated point
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Find the new y position for the new rotated point
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// Find the new z position for the new rotated point
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	m_vView = m_vPosition + vNewView;
}


///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This updates the camera according to the 
/////
///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Look()
{
	// Give openGL our camera position, then camera view, then camera up vector
	if(Game.bScriptSequence)
		gluLookAt(m_vSequencePosition.x, m_vSequencePosition.y, m_vSequencePosition.z,	
				  m_vSequenceView.x, m_vSequenceView.y, m_vSequenceView.z,	
				  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
	else
		gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
				  m_vView.x, m_vView.y, m_vView.z,	
				  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

CFPSCamera::CFPSCamera()
{
	iMode = CAMERA_MODE_FPS;
}

//////////////////////////// CHECK FOR MOVEMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the input faster than in the WinProc()
/////
//////////////////////////// CHECK FOR MOVEMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFPSCamera::CheckForMovement()
{	
	// Once we have the frame interval, we find the current speed
	float speed = kSpeed * g_fFrameInterval;

	// Check if we hit the "run" key
	if(GetKeyState(Engine->Keybinding.kbRun.uKey) & 0x80)
	{
		Engine->Keybinding.kbRun.bPressed = true;

		if(Game.bPlayMode)
			speed *= 4;
		else
			speed *= 20;
	}
	else
	{
		Engine->Keybinding.kbRun.bPressed = false;
	}

	char buffer[255] = "";

	// Check if we hit the "backward" key
	if(GetKeyState(Engine->Keybinding.kbBackward.uKey) & 0x80)
	{
		// Move our camera backward by a negative speed
		MoveCamera(-speed);
		Engine->Keybinding.kbBackward.bPressed = true;
	}
	else
	{
		Engine->Keybinding.kbBackward.bPressed = false;
	}

	// Check if we hit the "forward" key
	if(GetKeyState(Engine->Keybinding.kbForward.uKey) & 0x80)
	{
		// Move our camera forward by a positive speed
		MoveCamera(speed);
		Engine->Keybinding.kbForward.bPressed = true;
	}
	else
	{
		Engine->Keybinding.kbForward.bPressed = false;
	}

	// Check if we hit the "strafe left" key
	if(GetKeyState(Engine->Keybinding.kbStrafeLeft.uKey) & 0x80)
	{
		// Strafe the camera left
		StrafeCamera(-speed);
		Engine->Keybinding.kbStrafeLeft.bPressed = true;
	}
	else
	{
		Engine->Keybinding.kbStrafeLeft.bPressed = false;
	}

	// Check if we hit the "strafe right" key
	if(GetKeyState(Engine->Keybinding.kbStrafeRight.uKey) & 0x80)
	{
		// Strafe the camera right
		StrafeCamera(speed);
		Engine->Keybinding.kbStrafeRight.bPressed = true;
	}
	else
	{
		Engine->Keybinding.kbStrafeRight.bPressed = false;
	}

	if(Game.bPlayMode)
	{
		// Check if we hit the "rotate right" key
		if(GetKeyState(Engine->Keybinding.kbRotateLeft.uKey) & 0x80)
		{
			RotateView(speed / 20, 0.0f, 1.0f, 0.0f);
		}

		// Check if we hit the "rotate right" key
		if(GetKeyState(Engine->Keybinding.kbRotateRight.uKey) & 0x80)
		{
			RotateView(-speed / 20, 0.0f, 1.0f, 0.0f);
		}

		// Check if we hit the jump key
		if(GetKeyState(Engine->Keybinding.kbJump.uKey) & 0x80 && Game.mPlayer->iJump == 0)
		{
			Game.mPlayer->iJump = 1;
			Game.mPlayer->dTimeBeginJump = g_dCurTime;

			sprintf(buffer, "jump.wav");
		}

		// Check if we hit the crouch key
		if(GetKeyState(Engine->Keybinding.kbCrouch.uKey) & 0x80)
		{
			g_fCameraOverFloor = CAMERAOVERFLOOR_DUCK;
		}
		else if((GetKeyState(Engine->Keybinding.kbCrouch.uKey) & 0x80) == false)
		{
			g_fCameraOverFloor = CAMERAOVERFLOOR_DEFAULT;
		}
	}

	if(strcmp(buffer, "") != 0)
	{
		int iSound = g_Manager_Sound.Add(buffer, false);
		g_Manager_Sound.PlaySoundByID(iSound);
	}
}

///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will move the camera forward or backward depending on the speed
/////
///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFPSCamera::MoveCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	int iCollide = 0;

	if(Engine->bClipping)
	{
		if(g_iLocalPlayer >= 0)
		{
			CGraphicObject gobj = g_Manager_Player->Player[g_iLocalPlayer];

			gobj.mPosition.x += vVector.x * speed;

			if(Scene.CollisionWithSphere(&gobj) > 0)
				iCollide += 1;

			gobj.mPosition.x -= vVector.x * speed;
			gobj.mPosition.z += vVector.z * speed;

			if(Scene.CollisionWithSphere(&gobj) > 0)
				iCollide += 2;
		}

		if(iCollide == 3)
			return;
	}

	
	if(Engine->bClipping)
	{
		POINT g = {0, 0};
		//POINT g = g_Manager_Octree.CanMoveGradient(m_vPosition.x, m_vPosition.z, m_vPosition.x + vVector.x * speed, m_vPosition.z + vVector.z * speed);

		if(iCollide == 2 || iCollide == 0)
			if(!g.x)
				m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
		if(iCollide == 1 || iCollide == 0)
			if(!g.y)
				m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
		if(iCollide == 2 || iCollide == 0)
			if(!g.x)
				m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
		if(iCollide == 1 || iCollide == 0)
			if(!g.y)
				m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
	}
	else
	{
		m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
		m_vPosition.y += vVector.y * speed;		// Add our acceleration to our position's Y
		m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
		m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
		m_vView.y += vVector.y * speed;			// Add our acceleration to our view's Y
		m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
	}
}

///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This allows us to look around using the mouse, like in most first person games.
/////
///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFPSCamera::SetViewByMouse()
{
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = Engine->Screen.Middle.x;			// This is a binary shift to get half the width
	int middleY = Engine->Screen.Middle.y;			// This is a binary shift to get half the height
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
	static float currentRotX = 0.0f;
	
	// Get the mouse's current X,Y position
	GetCursorPos(&mousePos);						
	
	// If our cursor is still in the middle, we never moved... so don't update the screen
	if( (mousePos.x == middleX) && (mousePos.y == middleY) )
		return;

	// Set the mouse position to the middle of our window
	SetCursorPos(middleX, middleY);
	
	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)( (middleX - mousePos.x) ) / g_fMouseSpeedDown;
	angleZ = (float)( (middleY - mousePos.y) ) / g_fMouseSpeedDown;

	g_fCameraAngleY += angleY * 180 / fPi;
	if(g_fCameraAngleY >= 360.0f)
		g_fCameraAngleY -= 360.0f;
	if(g_fCameraAngleY < 0)
		g_fCameraAngleY += 360.0f;

	g_fCameraAngleZ += angleZ * 180 / fPi;
	if(g_fCameraAngleZ >= 360.0f)
		g_fCameraAngleZ -= 360.0f;
	if(g_fCameraAngleZ < 0)
		g_fCameraAngleZ += 360.0f;

	static float lastRotX = 0.0f; 
 	lastRotX = currentRotX; // We store off the currentRotX and will use it in when the angle is capped
	
	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	currentRotX += angleZ;
 
	if(Game.bPlayMode)
	{
		// If the current rotation (in radians) is greater than 1.5, we want to cap it.
		if(currentRotX > 1.5f)     
		{
			currentRotX = 1.5f;
			
			// Rotate by remaining angle if there is any
			if(lastRotX != 1.5f) 
			{
				// To find the axis we need to rotate around for up and down
				// movements, we need to get a perpendicular vector from the
				// camera's view vector and up vector.  This will be the axis.
				// Before using the axis, it's a good idea to normalize it first.
				CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
				vAxis = Normalize(vAxis);
					
				// rotate the camera by the remaining angle (1.0f - lastRotX)
				RotateView( 1.5f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
			}
		}
		// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
		else if(currentRotX < -1.0f)
		{
			currentRotX = -1.0f;
			
			// Rotate by the remaining angle if there is any
			if(lastRotX != -1.0f)
			{
				// To find the axis we need to rotate around for up and down
				// movements, we need to get a perpendicular vector from the
				// camera's view vector and up vector.  This will be the axis.
				// Before using the axis, it's a good idea to normalize it first.
				CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
				vAxis = Normalize(vAxis);
				
				// rotate the camera by ( -1.0f - lastRotX)
				RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
			}
		}
		// Otherwise, we can rotate the view around our position
		else 
		{	
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
		
			// Rotate around our perpendicular axis
			RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	else
	{
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		// Before using the axis, it's a good idea to normalize it first.
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);
	
		// Rotate around our perpendicular axis
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}
	
	// Always rotate the camera around the y-axis
	RotateView(angleY, 0, 1, 0);
}

///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This strafes the camera left and right depending on the speed (-/+)
/////
///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFPSCamera::StrafeCamera(float speed)
{
	int iCollide = 0;

	if(Engine->bClipping)
	{
		if(g_iLocalPlayer >= 0)
		{
			CGraphicObject gobj = g_Manager_Player->Player[g_iLocalPlayer];

			gobj.mPosition.x += m_vStrafe.x * speed;

			if(Scene.CollisionWithSphere(&gobj) > 0)
				iCollide += 1;

			gobj.mPosition.x -= m_vStrafe.x * speed;
			gobj.mPosition.z += m_vStrafe.z * speed;

			if(Scene.CollisionWithSphere(&gobj) > 0)
				iCollide += 2;
		}

		if(iCollide == 3)
			return;
	}


	if(Engine->bClipping)
	{
		POINT g = {0, 0};
		//POINT g = g_Manager_Octree.CanMoveGradient(m_vPosition.x, m_vPosition.z, m_vPosition.x + vVector.x * speed, m_vPosition.z + vVector.z * speed);

		if(iCollide == 2 || iCollide == 0)
			if(!g.x)
				m_vPosition.x += m_vStrafe.x * speed;		// Add our acceleration to our position's X
		if(iCollide == 1 || iCollide == 0)
			if(!g.y)
				m_vPosition.z += m_vStrafe.z * speed;		// Add our acceleration to our position's Z
		if(iCollide == 2 || iCollide == 0)
			if(!g.x)
				m_vView.x += m_vStrafe.x * speed;			// Add our acceleration to our view's X
		if(iCollide == 1 || iCollide == 0)
			if(!g.y)
				m_vView.z += m_vStrafe.z * speed;			// Add our acceleration to our view's Z
	}
	else
	{
		m_vPosition.x += m_vStrafe.x * speed;		// Add our acceleration to our position's X
		m_vPosition.y += m_vStrafe.y * speed;		// Add our acceleration to our position's Y
		m_vPosition.z += m_vStrafe.z * speed;		// Add our acceleration to our position's Z
		m_vView.x += m_vStrafe.x * speed;			// Add our acceleration to our view's X
		m_vView.y += m_vStrafe.y * speed;			// Add our acceleration to our view's Y
		m_vView.z += m_vStrafe.z * speed;			// Add our acceleration to our view's Z
	}
}

void CFPSCamera::Update() 
{
	// Initialize a variable for the cross product result
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// Normalize the strafe vector
	m_vStrafe = Normalize(vCross);

	if(!Game.bScriptSequence)
	{
		// Move the camera's view by the mouse
		SetViewByMouse();

		// This checks to see if the keyboard was pressed
		CheckForMovement();
	}
}
