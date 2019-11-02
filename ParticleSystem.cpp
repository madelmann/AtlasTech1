#include "Main.h"


int RangedRandom( int range_min, int range_max )
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   int u = int((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
   
   return u;
}

//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
//-----------------------------------------------------------------------------
float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
CVector3 getRandomVector( void )
{
	CVector3 vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -OGL_PI, OGL_PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

	return vVector;
}

//-----------------------------------------------------------------------------
// Name : classifyPoint()
// Desc : Classifies a point against the plane passed
//-----------------------------------------------------------------------------
int classifyPoint( CVector3 *vPoint, Plane *pPlane )
{
	CVector3 vDirection = pPlane->m_vPoint - *vPoint;

    float fResult = CVector3::dotProduct( vDirection,  pPlane->m_vNormal );

	if( fResult < -0.001f )
        return CP_FRONT;

	if( fResult >  0.001f )
        return CP_BACK;

	return CP_ONPLANE;
}

//-----------------------------------------------------------------------------
// Name: CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem()
{
	BoundingVolumeType	= BoundingVolume::Sphere;
	mCollisionVolumeType = BoundingVolume::None;
	iType				= EditorSpace::Particle;
	iMaterial			= -1;
	m_pActiveList		= NULL; // Head node of point sprites that are active
	m_pFreeList			= NULL; // Head node of point sprites that are inactive and waiting to be recycled.
	m_pPlanes			= NULL;
	m_dwActiveCount		= 0;
	m_fCurrentTime		= 0.0f;
	m_fLastUpdate		= 0.0f;
	m_chTexFile			= NULL;
	m_iBillboarding		= 0;
	m_dwMaxParticles	= 1;
	m_dwNumToRelease	= 1;
	m_fReleaseInterval	= 1.0f;
	m_fLifeCycle		= 1.0f;
	m_fSize				= 1.0f;
	m_vColor			= CVector3(1.0f,1.0f,1.0f);
	mPosition			= CVector3(0.0f,0.0f,0.0f);
	m_vVelocity			= CVector3(0.0f,0.0f,0.0f);
	m_vGravity			= CVector3(0.0f,0.0f,0.0f);
	m_bAirResistence	= true;
	m_AreaIsSet			= false;
	m_bBlend			= false;
	m_GlobalGravity		= false;
	m_bPolygon			= false;
	m_fAlpha			= 1.0f;
	m_fFadeIn			= 1.0f;
	m_fFadeOut			= 1.0f;
	m_fVelocityVar		= 1.0f;
	VisibleVolumeType	= BoundingVolume::Box;

	mLocked = false;
	mLoop = true;
	mPositionPointer = NULL;
	mSource = this;
}

//-----------------------------------------------------------------------------
// Name: ~CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
	while( m_pPlanes )					// Repeat till null...
	{
		Plane *pPlane = m_pPlanes;		// Hold onto the first one
		m_pPlanes = pPlane->m_pNext;	// Move down to the next one
		SAFE_DELETE ( pPlane );			// Delete the one we're holding
	}

	while( m_pActiveList )
	{
		Particle* pParticle = m_pActiveList;
		m_pActiveList = pParticle->m_pNext;
		SAFE_DELETE ( pParticle );
	}
	m_pActiveList = NULL;

	while( m_pFreeList )
	{
		Particle *pParticle = m_pFreeList;
		m_pFreeList = pParticle->m_pNext;
		SAFE_DELETE ( pParticle );
	}
	m_pFreeList = NULL;

	if( m_chTexFile != NULL )
	{
		SAFE_DELETE_ARRAY ( m_chTexFile );
		m_chTexFile = NULL;
	}
}

CVector3 CParticleSystem::getPosition(bool tryPointer)
{
	if(!tryPointer || !mPositionPointer) {
		return mPosition;
	}
	else {
		return *mPositionPointer;
	}
}

//-----------------------------------------------------------------------------
// Name: GetTextureObject()
// Desc: 
//-----------------------------------------------------------------------------
GLuint CParticleSystem::GetTextureID()
{
    return iTexture;
}

int CParticleSystem::Init()
{
	bDoCollision = false;
	iTexture = Texture_Add(m_chTexFile);

	SetBoundingBox();

	// Query for the max point size supported by the hardware
	glGetFloatv( GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize );
	glPointSize( m_fMaxPointSize );

	RestartParticleSystem();

	return S_OK;
}

void CParticleSystem::ParentAttach(CGraphicObject *go)
{
	mSource = go;
}

void CParticleSystem::ParentRemove()
{
	mSource = this;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::Render( void )
{
    Particle  *pParticle = m_pActiveList;

	glPushMatrix();
		// This is how will our point sprite's size will be modified by 
		// distance from the viewer.
		float quadratic[] =  { 1.0f, 0.f, 0.001f };
		glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

		// The alpha of a point is calculated to allow the fading of points 
		// instead of shrinking them past a defined threshold size. The threshold 
		// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
		// the minimum and maximum point sizes.
		glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 3.0f);

		glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 2.0f);
		glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, m_fMaxPointSize);

		// Specify point sprite texture coordinate replacement mode for each texture unit
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

		//
		// Render point sprites...
		//

		glDisable(GL_LIGHTING);

		if(Material_Apply(iMaterial, true) != S_OK)
			Texture_SetActiveID(iTexture);

		glRotatef(-mRotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(-mRotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(-mRotation.z, 0.0f, 0.0f, 1.0f);
		CVector3 pos = getPosition(true);
		glTranslatef(-pos.x, -pos.y, -pos.z);

		if(m_fAlpha < 1.0f)
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, m_fAlpha);
		}
		if(m_bBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			glDepthMask(GL_FALSE);
		}

		if(m_bPolygon)
		{
			if(Scene.GetRenderWater())
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);

			static float size = m_fSize / 10.0f;

			static float cam[3];
			cam[0] = g_Camera.Position().x;
			cam[1] = g_Camera.Position().y;
			cam[2] = g_Camera.Position().z;

			static float worldPos[3];

			while( pParticle )
			{
				worldPos[0] = pParticle->m_vCurPos.x;
				worldPos[1] = pParticle->m_vCurPos.y;
				worldPos[2] = pParticle->m_vCurPos.z;

				glColor4f(m_vColor.x, m_vColor.y, m_vColor.z, pParticle->m_fAlpha);

				glPushMatrix();
					glTranslatef(worldPos[0], worldPos[1], worldPos[2]);

					// Render each particle with billboarding ...
					BillBoard_Spherical_Begin(cam, worldPos);
						glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2f(1.0f, 1.0f);
							glVertex3f(	  size,
										  size,
										  0.0f );
							glTexCoord2f(0.0f, 1.0f);
							glVertex3f( - size,
										  size,
										  0.0f );
							glTexCoord2f(1.0f, 0.0f);
							glVertex3f(   size,
										- size,
										  0.0f );
							glTexCoord2f(0.0f, 0.0f);
							glVertex3f( - size,
										- size,
										  0.0f );
						glEnd();
					BillBoard_End();
				glPopMatrix();

				pParticle = pParticle->m_pNext;
			}

			if(Scene.GetRenderWater())
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		}
		else
		{
			glPointSize(m_fSize);
			glEnable(GL_POINT_SPRITE_ARB);

			glBegin(GL_POINTS);
			{
				// Render each particle...
				while( pParticle )
				{
					glColor4f(m_vColor.x, m_vColor.y, m_vColor.z, pParticle->m_fAlpha);

					glVertex3f( pParticle->m_vCurPos.x,
								pParticle->m_vCurPos.y,
								pParticle->m_vCurPos.z );

					pParticle = pParticle->m_pNext;
				}
			}
			glEnd();

			glDisable(GL_POINT_SPRITE_ARB);
		}

		if(m_bBlend)
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
		if(m_fAlpha < 1.0f)
		{
			glDisable(GL_ALPHA_TEST);
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnable(GL_LIGHTING);

	glPopMatrix();
}

//-----------------------------------------------------------------------------
// Name: RestartParticleSystem()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::RestartParticleSystem( void )
{
	Particle  *pParticle;
	Particle **ppParticle;

	ppParticle = &m_pActiveList; // Start at the head of the active list

	while( *ppParticle )
	{
		pParticle = *ppParticle; // Set a pointer to the head

		// Put the particle back on the free list...
		*ppParticle = pParticle->m_pNext;
		pParticle->m_pNext = m_pFreeList;
		m_pFreeList = pParticle;

		--m_dwActiveCount;
	}
}

void CParticleSystem::SetAlpha(float alpha)
{
	m_fAlpha = alpha;
}

void CParticleSystem::SetArea(CVector3 area)
{
	m_Area = area;
	m_AreaIsSet = true;

	SetBoundingBox();
}

void CParticleSystem::SetBlending()
{
	m_bBlend = true;
}

void CParticleSystem::SetBoundingBox()
{
	float fRadiusX = 0.0f;
	float fRadiusY = 0.0f;
	float fRadiusZ = 0.0f;

	if(m_AreaIsSet)
	{
		BoundingVolume.X1 = 0.0f;
		BoundingVolume.X2 = m_Area.x;
		BoundingVolume.Y1 = 0.0f;
		BoundingVolume.Y2 = m_Area.y;
		BoundingVolume.Z1 = 0.0f;
		BoundingVolume.Z2 = m_Area.z;
		BoundingVolume.CX = m_Area.x / 2;
		BoundingVolume.CY = m_Area.y / 2;
		BoundingVolume.CZ = m_Area.z / 2;

		fRadiusX = m_Area.x / 2 + 1.0f;
		fRadiusY = m_Area.y / 2 + 1.0f;
		fRadiusZ = m_Area.z / 2 + 1.0f;
	}
	else
	{
		BoundingVolume.X1 = -m_fSize / 10.0f;
		BoundingVolume.X2 = m_fSize / 10.0f;
		BoundingVolume.Y1 = -m_fSize / 10.0f;
		BoundingVolume.Y2 = m_fSize / 10.0f;
		BoundingVolume.Z1 = -m_fSize / 10.0f;
		BoundingVolume.Z2 = m_fSize / 10.0f;
		BoundingVolume.CX = 0.0f;
		BoundingVolume.CY = 0.0f;
		BoundingVolume.CZ = 0.0f;

		fRadiusX = m_fSize / 10.0f;
		fRadiusY = m_fSize / 10.0f;
		fRadiusZ = m_fSize / 10.0f;
	}

	fRadius = sqrt(fRadiusX * fRadiusX + fRadiusY * fRadiusY + fRadiusZ * fRadiusZ);
}

//-----------------------------------------------------------------------------
// Name: SetCollisionPlane()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::SetCollisionPlane( CVector3 vPlaneNormal, CVector3 vPoint, 
                                         float fBounceFactor, int nCollisionResult )
{
    Plane *pPlane = new Plane;

    pPlane->m_vNormal          = vPlaneNormal;
    pPlane->m_vPoint           = vPoint;
    pPlane->m_fBounceFactor    = fBounceFactor;
    pPlane->m_nCollisionResult = nCollisionResult;

    pPlane->m_pNext = m_pPlanes; // Attach the curent list to it...
    m_pPlanes = pPlane;          // ... and make it the new head.
}

void CParticleSystem::SetGlobalGravity(bool grav)
{
	m_GlobalGravity = grav;
}

void CParticleSystem::SetLoop(bool loop)
{
	mLoop = loop;
}

void CParticleSystem::SetMaterial(char* material)
{
	iMaterial = g_Manager_Material.Add(material);//Material_Add(material);
}

void CParticleSystem::SetPolygonMode(bool polygon)
{
	m_bPolygon = polygon;
}

void CParticleSystem::SetPosition(CVector3 pos)
{
	if(!mLocked)
	{
		mPosition.x = pos.x;
		mPosition.y = pos.y;
		mPosition.z = pos.z;

		RestartParticleSystem();
	}
}

//-----------------------------------------------------------------------------
// Name: SetTexture()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::SetTexture(char *chTexFile)
{
    // Deallocate the memory that was previously reserved for this string.
	if( m_chTexFile != NULL )
	{
		SAFE_DELETE_ARRAY ( m_chTexFile );
		m_chTexFile = NULL;
	}
    
    // Dynamically allocate the correct amount of memory.
    m_chTexFile = new char[strlen( chTexFile ) + 1];

    // If the allocation succeeds, copy the initialization string.
    if( m_chTexFile != NULL )
		strcpy( m_chTexFile, chTexFile );

	iTexture = Texture_Add(m_chTexFile);
}

void CParticleSystem::Spawn(char name[255], float x, float y, float z, float scale)
{
	setName(name);

	if(!mLocked)
	{
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;

		RestartParticleSystem();
	}
}

void CParticleSystem::SetBillboarding(int billboarding)
{
	m_iBillboarding = billboarding;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc:
//-----------------------------------------------------------------------------
int CParticleSystem::Update(float fElpasedTime)
{
    Particle  *pParticle = NULL;
    Particle **ppParticle;
    Plane     *pPlane;
    Plane    **ppPlane;
    CVector3 vOldPosition;

    m_fCurrentTime += fElpasedTime;												// Update our particle system timer...

    ppParticle = &m_pActiveList;												// Start at the head of the active list

    while( *ppParticle )
    {
        pParticle = *ppParticle;												// Set a pointer to the head

        float fTimePassed  = m_fCurrentTime - pParticle->m_fInitTime;

        if( fTimePassed >= m_fLifeCycle )										// Time is up, put the particle back on the free list...
        {
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pFreeList;
            m_pFreeList = pParticle;

			if(mLoop)															// Only decrement m_dwActiveCount if looping is set to true
				m_dwActiveCount -= 1;
        }
        else																	// Update particle position, velocity and alpha value
        {
            pParticle->m_vCurVel += m_vGravity * fElpasedTime;					// Update velocity with respect to Gravity (Constant Accelaration)
			if(m_GlobalGravity)
				pParticle->m_vCurVel += Map.vGravity * fElpasedTime;

            if(m_bAirResistence)												// Update velocity with respect to Wind (Accelaration based on difference of vectors)
				pParticle->m_vCurVel += (Map.vWind - pParticle->m_vCurVel) * fElpasedTime;

			if(fTimePassed < m_fFadeIn)											// Update alpha value of particle according to life time
			{
				pParticle->m_fAlpha = fTimePassed / m_fFadeIn;
			}
			else if(m_fLifeCycle - fTimePassed < m_fFadeOut)
			{
				pParticle->m_fAlpha = (m_fLifeCycle - fTimePassed) / m_fFadeOut;
			}
			else
				pParticle->m_fAlpha = 1.0f;

            // Finally, update position with respect to velocity
            vOldPosition = pParticle->m_vCurPos;
            pParticle->m_vCurPos += pParticle->m_vCurVel * fElpasedTime;

            //-----------------------------------------------------------------
            // BEGIN Checking the particle against each plane that was set up

            ppPlane = &m_pPlanes; // Set a pointer to the head

            while( *ppPlane )
            {
                pPlane = *ppPlane;
                int result = classifyPoint( &pParticle->m_vCurPos, pPlane );

                if( result == CP_BACK /*|| result == CP_ONPLANE */ )
                {
                    if( pPlane->m_nCollisionResult == CR_BOUNCE )
                    {
                        pParticle->m_vCurPos = vOldPosition;

						//-----------------------------------------------------------------
						//
						// The new velocity vector of a particle that is bouncing off
						// a plane is computed as follows:
						//
						// Vn = (N.V) * N
						// Vt = V - Vn
						// Vp = Vt - Kr * Vn
						//
						// Where:
						// 
						// .  = Dot product operation
						// N  = The normal of the plane from which we bounced
						// V  = Velocity vector prior to bounce
						// Vn = Normal force
						// Kr = The coefficient of restitution ( Ex. 1 = Full Bounce, 
						//      0 = Particle Sticks )
						// Vp = New velocity vector after bounce
						//
						//-----------------------------------------------------------------

                        float Kr = pPlane->m_fBounceFactor;

                        CVector3 Vn = CVector3::dotProduct( pPlane->m_vNormal, 
							                                pParticle->m_vCurVel ) * pPlane->m_vNormal;

                        CVector3 Vt = pParticle->m_vCurVel - Vn;
                        CVector3 Vp = Vt - Kr * Vn;

                        pParticle->m_vCurVel = Vp;
                    }
                    else if( pPlane->m_nCollisionResult == CR_RECYCLE )
                    {
                        pParticle->m_fInitTime -= m_fLifeCycle;
                    }
                    else if( pPlane->m_nCollisionResult == CR_STICK )
                    {
                        pParticle->m_vCurPos = vOldPosition;
                        pParticle->m_vCurVel = CVector3(0.0f,0.0f,0.0f);
                    }
                }

                ppPlane = &pPlane->m_pNext;
            }

            // END Plane Checking
            //-----------------------------------------------------------------

            ppParticle = &pParticle->m_pNext;
        }
    }

    //-------------------------------------------------------------------------
    // Emit new particles in accordance to the flow rate...
    // 
    // NOTE: The system operates with a finite number of particles.
    //       New particles will be created until the max amount has
    //       been reached, after that, only old particles that have
    //       been recycled can be reintialized and used again.
    //-------------------------------------------------------------------------

    if( m_fCurrentTime - m_fLastUpdate > m_fReleaseInterval )
    {
        // Reset update timing...
        m_fLastUpdate = m_fCurrentTime;

        // Emit new particles at specified flow rate...
		for( DWORD i = 0; i < m_dwNumToRelease; i += 1 )
        {
            // Do we have any free particles to put back to work?
            if( m_pFreeList )
            {
                // If so, hand over the first free one to be reused.
                pParticle = m_pFreeList;
                // Then make the next free particle in the list next to go!
                m_pFreeList = pParticle->m_pNext;
            }
            else
            {
                // There are no free particles to recycle...
                // We'll have to create a new one from scratch!
                if( m_dwActiveCount < m_dwMaxParticles )
                {
                    if( NULL == ( pParticle = new Particle ) )
                        return E_OUTOFMEMORY;
                }
            }

            if( m_dwActiveCount < m_dwMaxParticles )
            {
                pParticle->m_pNext = m_pActiveList; // Make it the new head...
                m_pActiveList = pParticle;

                // Set the attributes for our new particle...
				pParticle->m_fAlpha = 0.0f;
                pParticle->m_vCurVel = m_vVelocity;

                if( m_fVelocityVar != 0.0f )
                {
                    CVector3 vRandomVec = getRandomVector();
                    pParticle->m_vCurVel += vRandomVec * m_fVelocityVar;
                }

                pParticle->m_fInitTime  = m_fCurrentTime;
				CVector3 pos = getPosition(true);
                pParticle->m_vCurPos    = pos;

				if(m_AreaIsSet)													// Randomize spawn position
				{
					pParticle->m_vCurPos.x = pos.x + (float)RangedRandom(0, (int)m_Area.x);
					pParticle->m_vCurPos.y = pos.y + (float)RangedRandom(0, (int)m_Area.y);
					pParticle->m_vCurPos.z = pos.z + (float)RangedRandom(0, (int)m_Area.z);
				}
				else
				{
					//pParticle->m_vCurPos.x = mSource->mPosition.x;
					//pParticle->m_vCurPos.y = mSource->mPosition.y;
					//pParticle->m_vCurPos.z = mSource->mPosition.z;
					pParticle->m_vCurPos.x = pos.x;
					pParticle->m_vCurPos.y = pos.y;
					pParticle->m_vCurPos.z = pos.z;
				}

				m_dwActiveCount += 1;
            }
        }
    }

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


CParticleManager::CParticleManager()
{
	Count = 0;
}

int CParticleManager::Add(char* Filename)
{
	if(Count >= PARTICLE_MAX)
		return -1;

	if(strlen(Filename) <= 0)
		return -1;

	Filename = RemoveFileExt(Filename);

	iActive = Count;

	FILE *pFile;
	char fname[255] = "";
	char buffer[255] = "";

	sprintf(fname, "%sMaps\\%s\\Particles\\%s.particle", g_sGameName, Game.sMapName, Filename);
	
	if(fopen_s(&pFile, fname, "rb") != NULL)
	{
		sprintf(fname, "%sParticles\\%s.particle", g_sGameName, Filename);

		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			sprintf(buffer, "ERROR[ParticleManager.Add]: '%s' not found!", fname);
			Console->Output(buffer);

			return -1;
		}
	}

	Object[iActive] = CParticleSystem();
	Object[iActive].setId(Count);
	Object[iActive].setName(Filename);

	char oneline[255] = "";
	do
	{
		sprintf(buffer, "");
		readstr(pFile, buffer);
		sprintf(oneline, "PARTICLE.%s", buffer);

		ProcessCommand(oneline);
	} while(strcmp(buffer, "") != 0);

	fclose(pFile);

	Object[iActive].Init();

	//if(index == -1)
	Count++;

	return iActive;
}

void CParticleManager::Clear()
{
	for(int i = Count - 1; i > 0; i -= 1)
	{
		Object[i] = CParticleSystem();
	}

	Count = 0;
	iActive = -1;
}

int CParticleManager::IndexOf(char* objectname)
{
	objectname = RemoveFileExt(objectname);

	for(int i = 0; i < Count; i += 1)
	{
		if(strcmp(Object[i].getName(), objectname) == 0)
		{
			return i;
		}
	}

	return -1;
}

int CParticleManager::IndexOfID(long id)
{
	for(int i = 0; i < Count; i += 1)
	{
		if(Object[i].getId() == id)
		{
			return i;
		}
	}

	return -1;
}

int CParticleManager::Init( void )
{
	//
	// If the required extensions are present, get the addresses of their 
	// functions that we wish to use...
	//

    char *ext = (char*)glGetString( GL_EXTENSIONS );

	if( strstr( ext, "GL_ARB_point_parameters" ) == NULL )
	{
		MessageBox(NULL,"GL_ARB_point_parameters extension was not found",
			"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return S_FALSE;
	}
	else
	{
		glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
		glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

		if( !glPointParameterfARB || !glPointParameterfvARB )
		{
			MessageBox(NULL,"One or more GL_ARB_point_parameters functions were not found",
				"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return S_FALSE;
		}
	}

    return S_OK;
}