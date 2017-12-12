#include "Main.h"


void RenderLineBox(AABBVolume BoundingBox)
{
	glBegin(GL_LINE_LOOP);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);

		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z2);

		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z2);

		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z1);
	glEnd();
}

void RenderSolidBox(AABBVolume BoundingBox)
{
	glBegin(GL_QUADS);
		// Bottom
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z1);

		// Top
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z1);

		// Left
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z1);

		// Right
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z2);

		// Front
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z1);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z1);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z2);

		// Back
		glVertex3f(BoundingBox.X1, BoundingBox.Y1, BoundingBox.Z2);
		glVertex3f(BoundingBox.X1, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y2, BoundingBox.Z2);
		glVertex3f(BoundingBox.X2, BoundingBox.Y1, BoundingBox.Z2);
	glEnd();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


CPhysicObject::CPhysicObject()
: CObject()
{
	mApplyPhysic = false;
	mMass = 0.0f;
	mPosition = CVector3(0.0f, 0.0f, 0.0f);
}

CPhysicObject::CPhysicObject(float mass)
{
	mApplyPhysic = false;
	mCollisionVolumeType = BoundingVolume::None;
	mMass = mass;
	mPosition = CVector3(0.0f, 0.0f, 0.0f);
}

void CPhysicObject::ApplyForce(Physic::Force_e force, CVector3 vforce)
{
	switch(force)
	{
	case Physic::ConstantPush:
		mVelocity = vforce;
		break;
	case Physic::Gravity:
		mGravity = vforce;
		break;
	case Physic::Push:
		mVelocityPush = vforce;
		break;
	}
}

void CPhysicObject::DoForce(Physic::Force_e force)
{
	double time = timeGetTime();

	switch(force)
	{
	case Physic::Gravity:
		mPosition.x += mGravity.x * static_cast<float>((time - mLastPhysic) / 1000);
		mPosition.y += mGravity.y * static_cast<float>((time - mLastPhysic) / 1000);
		mPosition.z += mGravity.z * static_cast<float>((time - mLastPhysic) / 1000);
		break;
	case Physic::Push:
		mPosition.x += mVelocityPush.x * static_cast<float>((time - mLastPhysic) / 1000);
		mPosition.y += mVelocityPush.y * static_cast<float>((time - mLastPhysic) / 1000);
		mPosition.z += mVelocityPush.z * static_cast<float>((time - mLastPhysic) / 1000);

		mVelocityPush.x -= 0.5f;
		if(mVelocityPush.x < 0)
			mVelocityPush.x = 0;
		mVelocityPush.y -= 0.5f;
		if(mVelocityPush.y < 0)
			mVelocityPush.y = 0;
		mVelocityPush.z -= 0.5f;
		if(mVelocityPush.z < 0)
			mVelocityPush.z = 0;
		break;
	}

	mLastPhysic = time;
}

void CPhysicObject::DoPhysic(bool dophysic)
{
	mApplyPhysic = dophysic;
}

bool CPhysicObject::GetDoPhysic()
{
	return mApplyPhysic;
}

CVector3 CPhysicObject::GetPosition()
{
	return mPosition;
}

void CPhysicObject::SetGravity(CVector3 gravity)
{
	mGravity = gravity;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


CGraphicObject::CGraphicObject()
: CPhysicObject(),
  iNormalMap(0),
  iTexture(0),
  mShader(NULL)
{
	BoundingVolumeType = BoundingVolume::Box;
	mCollisionVolumeType = BoundingVolume::Box;
	bDoCollision = true;
	bPickable = false;
	bSolidBoundingVolume = false;
	bSpawned = false;
	sprintf(cTexture, "");
	fRadius = 0.0f;
	fScale = 1.0f;
	fTranslationX = 0.0f;
	fTranslationY = 0.0f;
	fTranslationZ = 0.0f;
	iHealth = 0;
	iMaterial = -1;
	iType = -1;
	sprintf(sTriggerScript, "");
	mPosition = CVector3(0.0f, 0.0f, 0.0f);
	mPositionOld = mPosition;
	mRotation = CVector3();
	VisibleVolumeType = BoundingVolume::Box;
}

CGraphicObject::~CGraphicObject()
{
}

bool CGraphicObject::BoxCollision(CVector3 vector, float radius)
{
	if(!bDoCollision)
		return false;

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
		return true;
	else
		return false;
}

bool CGraphicObject::BoxCollision(CVector3 vector, float radius, bool force)
{
	if(!force)
		return BoxCollision(vector, radius);

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
		return true;
	else
		return false;
}

bool CGraphicObject::CylinderCollision(CVector3 vector, float radius)
{
	if(!bDoCollision)
		return false;

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
	{
		if(vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2)
			return true;
	}
	
	return false;
}

bool CGraphicObject::CylinderCollision(CVector3 vector, float radius, bool force)
{
	if(!force)
		return CylinderCollision(vector, radius);

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
	{
		if(vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2)
			return true;
	}
	
	return false;
}

CVector3 CGraphicObject::GetPosition()
{
	return mPosition;
}

void CGraphicObject::RebuildBoundingBox()
{
	if(mRotation.x)
	{
		BoundingVolume.CX *= mRotation.x;
		BoundingVolume.X1 *= mRotation.x;
		BoundingVolume.X2 *= mRotation.x;
	}
	if(mRotation.y)
	{
		BoundingVolume.CY *= mRotation.y;
		BoundingVolume.Y1 *= mRotation.y;
		BoundingVolume.Y2 *= mRotation.y;
	}
	if(mRotation.z)
	{
		BoundingVolume.CZ *= mRotation.z;
		BoundingVolume.Z1 *= mRotation.z;
		BoundingVolume.Z2 *= mRotation.z;
	}
	//if(mRotation.x)
	//{
	//	BoundingVolume.CX = BoundingVolume_original.CX * mRotation.x;
	//	BoundingVolume.X1 = BoundingVolume_original.X1 * mRotation.x;
	//	BoundingVolume.X2 = BoundingVolume_original.X2 * mRotation.x;
	//}
	//if(mRotation.y)
	//{
	//	BoundingVolume.CY = BoundingVolume_original.CY * mRotation.y;
	//	BoundingVolume.Y1 = BoundingVolume_original.Y1 * mRotation.y;
	//	BoundingVolume.Y2 = BoundingVolume_original.Y2 * mRotation.y;
	//}
	//if(mRotation.z)
	//{
	//	BoundingVolume.CZ = BoundingVolume_original.CZ * mRotation.z;
	//	BoundingVolume.Z1 = BoundingVolume_original.Z1 * mRotation.z;
	//	BoundingVolume.Z2 = BoundingVolume_original.Z2 * mRotation.z;
	//}
}

void CGraphicObject::RenderBoundingVolume()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glLineWidth(3.0f);

	switch(VisibleVolumeType)
	{
	case BoundingVolume::Box:
		if(bSolidBoundingVolume)
			RenderSolidBox(BoundingVolume);
		else
			RenderLineBox(BoundingVolume);
		break;
	case BoundingVolume::Cylinder:
		{
			static GLUquadricObj *myCylinder = gluNewQuadric();
			gluQuadricTexture(myCylinder, GL_FALSE);

			glPushMatrix();
				glTranslatef(BoundingVolume.CX, BoundingVolume.CY, BoundingVolume.CZ);

				if(!bSolidBoundingVolume)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);								// Draw Frontfacing Polygons As Wireframe

				gluCylinder(myCylinder, fRadius, fRadius, BoundingVolume.X2 - BoundingVolume.X1, 12, 6);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);									// Return to solid mode
			glPopMatrix();
			break;
		}
	case BoundingVolume::Sphere:
		{
			static GLUquadricObj *mySphere = gluNewQuadric();
			gluQuadricTexture(mySphere, GL_FALSE);

			glPushMatrix();
				glTranslatef(BoundingVolume.CX, BoundingVolume.CY, BoundingVolume.CZ);

				if(!bSolidBoundingVolume)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);								// Draw Frontfacing Polygons As Wireframe

				gluSphere(mySphere, fRadius, 12, 6);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);									// Return to solid mode
			glPopMatrix();
			break;
		}
	default:
		break;
	}

	glLineWidth(1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

void CGraphicObject::SetBoundingBox(AABBVolume &box)
{
	BoundingVolume = box;
	//BoundingVolume_original = box;
}

void CGraphicObject::setMaterial(GLuint material)
{
	iMaterial = material;
}

void CGraphicObject::setNormalMap(GLuint normalmap)
{
	iNormalMap = normalmap;
}

void CGraphicObject::setShader(CShader* shader)
{
	mShader = shader;
}

void CGraphicObject::setTexture(GLuint texture)
{
	iTexture = texture;
}

void CGraphicObject::SetPosition(CVector3 position)
{
	mPosition = position;

	iOctreeX = int((mPosition.x + 2048) / 256);
	iOctreeZ = int((mPosition.z + 2048) / 256);

	SetSelectionIndicator();
}

void CGraphicObject::SetSelectionIndicator()
{
	vSelectionIndicator.x = 0.0f;//(BoundingBox.X2 - BoundingBox.X1) / 2;
	vSelectionIndicator.y = BoundingVolume.Y2;
	vSelectionIndicator.z = 0.0f;//(BoundingBox.Z2 - BoundingBox.Z1) / 2;
}

bool CGraphicObject::SphereCollision(CVector3 vector, float radius)
{
	if(!bDoCollision)
		return false;

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
		return true;
	else
		return false;
}

bool CGraphicObject::SphereCollision(CVector3 vector, float radius, bool force)
{
	if(!force)
		return SphereCollision(vector, radius);

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x + radius >= BoundingVolume.X1 && vector.x - radius <= BoundingVolume.X2 &&
		vector.y + radius >= BoundingVolume.Y1 && vector.y - radius <= BoundingVolume.Y2 &&
		vector.z + radius >= BoundingVolume.Z1 && vector.z - radius <= BoundingVolume.Z2)
		return true;
	else
		return false;
}

bool CGraphicObject::SphereCollisionLine(CVector3 vector, float radius)
{
	if(!bDoCollision)
		return false;

	if( vector.x + radius >= mPositionOld.x + BoundingVolume.CX - fRadius && vector.x - radius <= mPosition.x + BoundingVolume.CX + fRadius &&
		vector.y + radius >= mPositionOld.y + BoundingVolume.CY - fRadius && vector.y - radius <= mPosition.y + BoundingVolume.CY + fRadius &&
		vector.z + radius >= mPositionOld.z + BoundingVolume.CZ - fRadius && vector.z - radius <= mPosition.z + BoundingVolume.CZ + fRadius)
		return true;
	else
		return false;
}

bool CGraphicObject::SphereCollisionPoint(CVector3 vector)
{
	if(!bDoCollision)
		return false;

	vector.x -= mPosition.x;
	vector.y -= mPosition.y;
	vector.z -= mPosition.z;

	if( vector.x >= BoundingVolume.X1 && vector.x <= BoundingVolume.X2 &&
		vector.y >= BoundingVolume.Y1 && vector.y <= BoundingVolume.Y2 &&
		vector.z >= BoundingVolume.Z1 && vector.z <= BoundingVolume.Z2)
		return true;
	else
		return false;
}