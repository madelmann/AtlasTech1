#ifndef _CAMERA_H
#define _CAMERA_H


#include "Main.h"


#define CAMERA_MODE_NOTDEFINED	0
#define CAMERA_MODE_FPS			1


class CCamera
{
public:
	CCamera();											// Our camera constructor

	// These are are data access functions for our camera's data
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}

	void Look();										// This uses gluLookAt() to tell OpenGL where to look
	void PositionCamera(float positionX, float positionY, float positionZ,		// This changes the position, view, and up vector of the camera.
			 		    float viewX,     float viewY,     float viewZ,			// This is primarily used for initialization
						float upVectorX, float upVectorY, float upVectorZ);
	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);		// This rotates the camera around a point (I.E. your character)
	void RotateView(float angle, float X, float Y, float Z);					// This rotates the camera's view around the position depending on the values passed in.
	void SetPosition(CVector3 NewPos);
	void SetSequencePosition(CVector3 NewPos);
	void SetSequenceView(CVector3 NewView);
	void SetView(CVector3 NewView);
	

	int			iMode;
	CVector3	m_vGamePosition;
	CVector3	m_vGameView;
	CVector3	m_vPosition;								// The camera's position
	CVector3	m_vSequencePosition;
	CVector3	m_vSequenceView;
	CVector3	m_vStrafe;									// The camera's strafe vector
	CVector3	m_vView;									// The camera's view
	CVector3	m_vUpVector;								// The camera's up vector
};


class CFPSCamera : public CCamera
{
public:
	CFPSCamera();

	void CheckForMovement();							// This checks for keyboard movement
	void MoveCamera(float speed);						// This will move the camera forward or backward depending on the speed
	void SetViewByMouse();								// This moves the camera's view by the mouse movements (First person view)
	void StrafeCamera(float speed);						// This strafes the camera left or right depending on the speed (+/-)
	void Update();										// This updates the camera's view and other data (Should be called each frame)
};


#endif