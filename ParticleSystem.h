#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Main.h"
//#include "vector3f.h"


#define PARTICLE_MAX	100

//-----------------------------------------------------------------------------
// SYMBOLIC CONSTANTS
//-----------------------------------------------------------------------------

// Classify Point
const int CP_FRONT   = 0;
const int CP_BACK    = 1;
const int CP_ONPLANE = 2;

// Collision Results
const int CR_BOUNCE  = 0;
const int CR_STICK   = 1;
const int CR_RECYCLE = 2;

const float OGL_PI = 3.141592654f;

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

struct Plane
{
    CVector3 m_vNormal;           // The plane's normal
    CVector3 m_vPoint;            // A coplanar point within the plane
    float    m_fBounceFactor;     // Coefficient of restitution (or how bouncy the plane is)
    int      m_nCollisionResult;  // What will particles do when they strike the plane

    Plane   *m_pNext;             // Next plane in list
};

struct Particle
{
	float	  m_fAlpha;
    CVector3  m_vCurPos;    // Current position of particle
    CVector3  m_vCurVel;    // Current velocity of particle
    float     m_fInitTime;  // Time of creation of particle

    Particle *m_pNext;      // Next particle in list
};

struct PointVertex
{
    CVector3 posit;
    CVector3 color;
};


//-----------------------------------------------------------------------------
// CLASSES
//-----------------------------------------------------------------------------

class CParticleSystem : public CGraphicObject
{
public:
	CParticleSystem(void);
	~CParticleSystem(void);

	int Init();
	void Lock() { mLocked = true; }

	void ParentAttach(CGraphicObject *go);
	void ParentRemove();

	void SetBillboarding(int billboarding);

	void SetMaxParticles( DWORD dwMaxParticles ) { m_dwMaxParticles = dwMaxParticles; }
	DWORD GetMaxParticles( void ) { return m_dwMaxParticles; }

	void SetNumToRelease( DWORD dwNumToRelease ) { m_dwNumToRelease = dwNumToRelease; }
	DWORD GetNumToRelease( void ) { return m_dwNumToRelease; }

	void SetReleaseInterval( float fReleaseInterval ) { m_fReleaseInterval = fReleaseInterval; }
	float GetReleaseInterval( void ) { return m_fReleaseInterval; }

	void SetLifeCycle( float fLifeCycle ) { m_fLifeCycle = fLifeCycle; }
	float GetLifeCycle( void ) { return m_fLifeCycle; }

	void SetSize( float fSize ) { m_fSize = fSize; }
	float GetSize( void ) { return m_fSize; }
	float GetMaxPointSize( void ) { return m_fMaxPointSize; }

	void SetColor( CVector3 clrColor ) { m_vColor = clrColor; }
	CVector3 GetColor( void ) { return m_vColor; }

	void SetFadeIn(float fadespeed) { m_fFadeIn = fadespeed; }
	void SetFadeOut(float fadespeed) { m_fFadeOut = fadespeed; }

	void SetPosition(CVector3 pos);
	CVector3 GetPosition( void ) { return mPosition; }

	void SetPositionPointer(CVector3* pos) { mPositionPointer = pos; }
	CVector3 getPosition(bool tryPointer);

	void SetVelocity( CVector3 vVelocity ) { m_vVelocity = vVelocity; }
	CVector3 GetVelocity( void ) { return m_vVelocity; }

	void SetGravity( CVector3 vGravity ) { m_vGravity = vGravity; }
	CVector3 GetGravity( void ) { return m_vGravity; }

	void SetAirResistence( bool bAirResistence ) { m_bAirResistence = bAirResistence; }
	bool GetAirResistence( void ) { return m_bAirResistence; }

	void SetVelocityVar( float fVelocityVar ) { m_fVelocityVar = fVelocityVar; }
	float GetVelocityVar( void ) { return m_fVelocityVar; }

	void SetCollisionPlane( CVector3 vPlaneNormal, CVector3 vPoint, 
							float fBounceFactor = 1.0f, int nCollisionResult = CR_BOUNCE );
	void Render( void );
	void RestartParticleSystem(void);
	void SetAlpha(float alpha);
	void SetArea(CVector3 area);
	void SetBlending();
	void SetBoundingBox();
	void SetGlobalGravity(bool grav);
	void SetLoop(bool loop);
	void SetMaterial(char* material);
	void SetPolygonMode(bool polygon);
	void Spawn(char name[255], float x, float y, float z, float scale);
	int  Update( float fElapsedTime );

	void SetTexture( char *chTexFile );
	GLuint GetTextureID(void);

private:
	Particle   *m_pActiveList;
	Particle   *m_pFreeList;
	Plane      *m_pPlanes;
	DWORD       m_dwActiveCount;
	float       m_fCurrentTime;
	float       m_fLastUpdate;
	float       m_fMaxPointSize;
	bool        m_bDeviceSupportsPSIZE;

	// Particle Attributes
	CVector3	m_Area;
	bool		m_AreaIsSet;
	bool		m_bBlend;
	bool		m_GlobalGravity;
	bool		m_bPolygon;
	DWORD       m_dwMaxParticles;
	DWORD       m_dwNumToRelease;
	float		m_fAlpha;
	float		m_fFadeIn;
	float		m_fFadeOut;
	float       m_fReleaseInterval;
	float       m_fLifeCycle;
	float       m_fSize;
	int			m_iBillboarding;
	CVector3    m_vColor;
	CVector3    m_vVelocity;
	CVector3    m_vGravity;
	bool        m_bAirResistence;
	float       m_fVelocityVar;
	char       *m_chTexFile;

	bool			mLocked;
	bool			mLoop;
	CVector3*		mPositionPointer;
	CGraphicObject	*mSource;
};


class CParticleManager
{
public:
	CParticleManager();
	int  Add(char* Filename);
	void Clear();
	int  IndexOf(char* Filename);
	int  IndexOfID(long id);
	int  Init();

	int	Count;
	int iActive;
	CParticleSystem Object[PARTICLE_MAX];
};

#endif