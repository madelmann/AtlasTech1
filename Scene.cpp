#include "Main.h"
#include "Scene.h"

using namespace std;


bool preventDuplicates(vector<CSceneID> &vec, int child)
{
	for(vector<CSceneID>::iterator i = vec.begin(); i != vec.end(); i++)
	{
		if(i->getId() == child)
		{
			return false;
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////CTimedOperation - Begin ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CTimedOperation::CTimedOperation()
{
	iRepeat = 0;
	sprintf(sCommand, "");
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////CTimedOperation - End /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////// CSceneObject - Begin ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CSceneObject::CSceneObject(CGraphicObject* obj)
: bAllowDraw(true),
  bLastDrawResult(true),
  bMarkedForDeletion(false)
{
	gluiQuery = 0;
	gluiQueryResultAvailible = 0;
	gluiSamples = 0;
	go = obj;
	uiOcclusionCalculatedBeforeFPS = 0;
	Motion.push_back(new CMotion(go->mPosition, go->mRotation));
	//Nodes.clear();

	mLastRendered = 0;
}

CSceneObject::~CSceneObject()
{
	for(list<CMotion*>::iterator i = Motion.begin(); i != Motion.end(); i++)
	{
		delete (*i);
	}

	Motion.clear();
	Nodes.clear();
}

/*
 * CanDraw(): process occlusion query output
 */
bool CSceneObject::CanDraw()
{
	uiOcclusionCalculatedBeforeFPS += 1;
	bAllowDraw = bLastDrawResult;

	if(uiOcclusionCalculatedBeforeFPS < Engine->uiMaxOcclusionCalculatedBeforeFPS)
		return bAllowDraw;

	//OcclusionQuery();

	//int available = 0;
	//while(!available) {
	//	glGetQueryObjectivARB(gluiQuery, GL_QUERY_RESULT_AVAILABLE_ARB, &available);
	//}
	//glGetQueryObjectuivARB(gluiQuery, GL_QUERY_RESULT_ARB, &gluiSamples);

	//if(gluiSamples < Engine->gluiMinVisibleSamples)
	//	return false;

	AABBVolume volume;
	volume.CX = go->mPosition.x + go->BoundingVolume.CX;
	volume.CY = go->mPosition.y + go->BoundingVolume.CY;
	volume.CZ = go->mPosition.z + go->BoundingVolume.CZ;
	volume.X1 = go->mPosition.x + go->BoundingVolume.X1;
	volume.Y1 = go->mPosition.y + go->BoundingVolume.Y1;
	volume.Z1 = go->mPosition.z + go->BoundingVolume.Z1;
	volume.X2 = go->mPosition.x + go->BoundingVolume.X2;
	volume.Y2 = go->mPosition.y + go->BoundingVolume.Y2;
	volume.Z2 = go->mPosition.z + go->BoundingVolume.Z2;
	volume.Radius = go->fRadius;

	if(Scene.GetRenderWater())			// !BUGS! needs some more thinking
	{
		volume.CY = (go->mPosition.y + go->BoundingVolume.CY - Map.fWaterHeight) * (-1);
		volume.Y1 = (go->mPosition.y + go->BoundingVolume.Y1 - Map.fWaterHeight) * (-1);
		volume.Y2 = (go->mPosition.y + go->BoundingVolume.Y2 - Map.fWaterHeight) * (-1);
	}

	bAllowDraw = Frustum.volumeInFrustum(volume);

	uiOcclusionCalculatedBeforeFPS = 0;

	return bAllowDraw;
}

int CSceneObject::Hover(CVector3 p3d)
{
	CVector3 bb1, bb2;
	bool result = false;
	int hid = -1;

	for(vector<CSceneID>::iterator it = Nodes.begin(); it != Nodes.end(); it++)
	{
		CSceneObject* so = Scene.GetObjectByID(it->getId());

		if(so == NULL)
			continue;

		switch(so->go->VisibleVolumeType)
		{
		case BoundingVolume::Box:
			result = so->go->BoxCollision(p3d, 1.0f, true);
			break;
		case BoundingVolume::Cylinder:
			result = so->go->CylinderCollision(p3d, 1.0f, true);
			break;
		case BoundingVolume::Sphere:
			result = so->go->SphereCollision(p3d, 1.0f, true);
			break;
		default:
			result = so->go->BoxCollision(p3d, 1.0f, true);
			break;
		}

		if(result)
		{
			hid = so->SID.getId();

			return hid;
		}
	}

	hid = -1;
	return hid;
}

/*
 * Move(): incremental move scene object and all childs
 */
void CSceneObject::Move(CVector3 pos)
{
	if(Editor->bAllowMoveX)
		go->mPosition.x += pos.x;
	if(Editor->bAllowMoveY)
		go->mPosition.y += pos.y;
	if(Editor->bAllowMoveZ)
		go->mPosition.z += pos.z;

	for(vector<CSceneID>::iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		GetObjectByID(i->getId())->Move(pos);
	}
}

/*
 * NodeAttach(): attach child node
 */
bool CSceneObject::NodeAttach(int child)
{
	if(child == -1)
		return false;

	CSceneID id;
	id.setId(child);

	if(preventDuplicates(Nodes, child))
		Nodes.push_back(id);
	else
		return false;

	sort(Nodes.begin(), Nodes.end());

	GetObjectByID(id.getId())->SetParent(SID.getId());

	return true;
}

/*
 * NodeFreeChildren(): remove parent node entries from all child nodes
 */
bool CSceneObject::NodeFreeChildren()
{
	if(Nodes.size() <= 0)
		return true;

	for(vector<CSceneID>::iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		GetObjectByID(i->getId())->RemoveParent();
	}

	return true;
}

/*
 * NodeRemove(): remove parent node entry from child and erase child node entry from parent
 */
bool CSceneObject::NodeRemove(int child)
{
	if(child == -1)
		return false;

	for(vector<CSceneID>::iterator it = Nodes.begin(); it != Nodes.end(); it++)
	{
		if(it->getId() == child)
		{
			GetObjectByID(child)->RemoveParent();

			Nodes.erase(it);

			return true;
		}
	}

	return false;
}

/*
 * OcclusionQuery(): start ARB occlusion query
 */
void CSceneObject::OcclusionQuery()
{
	if(uiOcclusionCalculatedBeforeFPS < Engine->uiMaxOcclusionCalculatedBeforeFPS)
		return;

	go->bSolidBoundingVolume = true;

	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, gluiQuery);
		go->RenderBoundingVolume();
		//Render();
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);

	go->bSolidBoundingVolume = false;
}

/*
 * RemoveParent(): remove parent node entry
 */
int CSceneObject::RemoveParent()
{
	int old = SIDParent.getId();
	SIDParent.setId(-1);

	return old;
}

/*
 * Render(): render scene object
 */
void CSceneObject::Render()
{
	if(!this->CanDraw())
		return;

	if(bMarkedForDeletion)
		return;

	mLastRendered = g_FrameCount;

	glPushMatrix();
		glTranslatef(go->mPosition.x, go->mPosition.y, go->mPosition.z);
		glRotatef(go->mRotation.x, 1.0f, 0.0f, 0.0f);
		glRotatef(go->mRotation.y, 0.0f, 1.0f, 0.0f);
		glRotatef(go->mRotation.z, 0.0f, 0.0f, 1.0f);

		Material_Apply(go->iMaterial, false);

		if(!Game.bPlayMode)
		{
			if(Scene.iHoverObject == SID.getId())								// blue bounding box if mouse hovers over current object
			{
				glColor3f(0.0f, 0.0f, 1.0f);
			}
			else if(Scene.iHoverObject != -1)
			{
				CSceneObject* sobj = GetObjectByID(Scene.iHoverObject);
				if(sobj != NULL)
				{
					if(sobj->SIDParent.getId() == SID.getId())					// green bounding box for parent object if mouse hovers over current object
						glColor3f(0.0f, 1.0f, 0.0f);
				}
			}
			if(Scene.iActive == SID.getId())									// red bounding box if current object is selected
			{
				glColor3f(1.0f, 0.0f, 0.0f);
			}
			else if(Scene.iActive != -1)
			{
				CSceneObject* sobj = GetObjectByID(Scene.iActive);
				if(sobj != NULL)
				{
					if(sobj->SIDParent.getId() == SID.getId())					// green bounding box for parent object
						glColor3f(0.0f, 1.0f, 0.0f);
				}
			}

			go->RenderBoundingVolume();
		}

		// Set ambient color
		if(Engine->bCellShading)
			glColor3f(0.0f, 0.0f, 0.0f);
		else
			glColor3f(Map.fAmbientColor[0], Map.fAmbientColor[1], Map.fAmbientColor[2]);

		// Render
		if(Scene.iActive == SID.getId() && !Game.bPlayMode && Mouse.Button1)
			;	// don't draw if object is in move by editor
		else
			go->Render();

	glPopMatrix();

	// Update motion list
	//if(Motion.front()->mPosition != go->mPosition || Motion.front()->mRotation != go->mRotation)
	//{
	//	Motion.insert(Motion.begin(), new CMotion(go->mPosition, go->mRotation));

	//	if(Motion.size() > 5)
	//	{
	//		Motion.pop_back();
	//	}
	//}
}

/*
 * Rotate(): incremental rotate scene object and all child nodes
 */
void CSceneObject::Rotate(CVector3 rot)
{
	if(Editor->bAllowMoveX)
		go->mRotation.x += rot.x;
	if(Editor->bAllowMoveY)
		go->mRotation.y += rot.y;
	if(Editor->bAllowMoveZ)
		go->mRotation.z += rot.z;

	if(go->mRotation.x > 360.0f)
		go->mRotation.x -= 360.0f;
	if(go->mRotation.x < 0.0f)
		go->mRotation.x += 360.0f;
	if(go->mRotation.y > 360.0f)
		go->mRotation.y -= 360.0f;
	if(go->mRotation.y < 0.0f)
		go->mRotation.y += 360.0f;
	if(go->mRotation.z > 360.0f)
		go->mRotation.z -= 360.0f;
	if(go->mRotation.z < 0.0f)
		go->mRotation.z += 360.0f;

	//go->RebuildBoundingBox();

	for(vector<CSceneID>::iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		GetObjectByID(i->getId())->Rotate(rot);
	}
}

/*
 * SetPosition(): sets scene object's absolut position
 */
void CSceneObject::SetPosition(CVector3 pos)
{
	if(Editor->bAllowMoveX)
		go->mPosition.x = pos.x;
	if(Editor->bAllowMoveY)
		go->mPosition.y = pos.y;
	if(Editor->bAllowMoveZ)
		go->mPosition.z = pos.z;
}

/*
 * SetParent(): sets scene object's parent
 */
void CSceneObject::SetParent(int parent)
{
	SIDParent.setId(parent);
}

/*
 * SetRotation(): sets scene object's absolut rotation
 */
void CSceneObject::SetRotation(CVector3 rot)
{
	if(Editor->bAllowMoveX)
		go->mRotation.x = rot.x;
	if(Editor->bAllowMoveY)
		go->mRotation.y = rot.y;
	if(Editor->bAllowMoveZ)
		go->mRotation.z = rot.z;

	if(go->mRotation.x > 360.0f)
		go->mRotation.x -= 360.0f;
	if(go->mRotation.x < 0.0f)
		go->mRotation.x += 360.0f;
	if(go->mRotation.y > 360.0f)
		go->mRotation.y -= 360.0f;
	if(go->mRotation.y < 0.0f)
		go->mRotation.y += 360.0f;
	if(go->mRotation.z > 360.0f)
		go->mRotation.z -= 360.0f;
	if(go->mRotation.z < 0.0f)
		go->mRotation.z += 360.0f;

	//go->RebuildBoundingBox();

	for(vector<CSceneID>::iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		GetObjectByID(i->getId())->SetRotation(rot);
	}
}

/*
 * SetScale(): sets scene object's and all children's scale
 */
void CSceneObject::SetScale(float scale)
{
	go->fScale = scale;

	go->SetBoundingBox();
	go->PreProcess();

	for(vector<CSceneID>::iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		GetObjectByID(i->getId())->SetScale(scale);
	}
}

bool CSceneObject::operator == (const CSceneObject &other)
{
	return (this->SID == other.SID);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// CSceneObject - End //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


CScene::CScene()
{
	SceneObjects.clear();

	bCellShading = false;
	bRenderWater = false;
	Count = 0;
	iActive = -1;
	iHoverObject = -1;
	sprintf(sHoverMessage, "");
}

CScene::~CScene()
{
	SceneObjects.clear();
	TimedOperations.clear();
}

int CScene::Add(CGraphicObject* obj)
{
	CSceneObject scobj(obj);

	scobj.SID.setId(Count);
	glGenQueriesARB(1, &scobj.gluiQuery);

	SceneObjects.push_back(scobj);

	Count = SceneObjects.size();
	//iActive = Count - 1;
	iActive = -1;

	return Count - 1;
}

void CScene::BeginCellShading()
{
	bCellShading = true;

	glDisable(GL_LIGHTING);

	glLineWidth(5.0f);									// Set The Line Width

	if(bRenderWater)
	{
		glPolygonMode(GL_FRONT, GL_LINE);				// Draw Backfacing Polygons As Wireframes
		glCullFace(GL_FRONT);							// Don't Draw Any Back-Facing Polygons
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_LINE);				// Draw Backfacing Polygons As Wireframes
		glCullFace(GL_BACK);							// Don't Draw Any Front-Facing Polygons
	}

	glDepthFunc(GL_LEQUAL);								// Change The Depth Mode

	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);					// Set The Outline Color
}

void CScene::BeginRenderWater()
{
	bRenderWater = true;
}

void CScene::Clear()
{
	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		if(i->gluiQuery)
			glDeleteQueriesARB(1, &i->gluiQuery);

		i->bMarkedForDeletion = true;
	}

	try {
		//SceneObjects.clear();
		TimedOperations.clear();
	}
	catch (exception e) {
		throw e.what();
	}

	Count = 0;
	iActive = -1;
}

int CScene::CollisionWithBox(CGraphicObject* obj)
{
	if(Count < 0)
		return 0;

	int result = 0;

	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		if(i->go->BoxCollision(obj->mPosition, obj->fRadius))
		{
			result += 1;

			if(i->go->DeepCollision(obj->mPosition, obj->fRadius) == 0)
				result -= 1;
		}
	}

	return result;
}

int CScene::CollisionWithSphere(CGraphicObject* obj)
{
	if(Count < 0)
		return 0;

	int result = 0;

	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		if(i->go->SphereCollision(obj->mPosition, obj->fRadius))
		{
			result += 1;

			if(i->go->DeepCollision(obj->mPosition, obj->fRadius) == 0)
				result -= 1;
		}
	}

	return result;
}

/*
 * Delete(): delete object from scene
 */
int CScene::Delete()
{
	for(list<CSceneObject>::iterator it = SceneObjects.begin(); it != SceneObjects.end(); it++)
	{
		if(iActive == it->SID.getId())
		{
			glDeleteQueriesARB(1, &it->gluiQuery);						// Delete reserved occlusion query
			
			it->NodeFreeChildren();										// Remove parent entry from child nodes

			CSceneObject* psobj = GetObjectByID(it->SIDParent.getId());
			if(psobj)
				psobj->NodeRemove(it->SID.getId());						// Remove child entries from parent

			// Throws a bad error!!!
			//SceneObjects.erase(it);										// Remove object from scene
			(*it).bMarkedForDeletion = true;

			Count = SceneObjects.size();

			int oldactive = iActive;

			if(iActive > Count - 1)
				iActive = -1;
			
			return oldactive;
		}
	}

	return -1;
}

void CScene::EndCellShading()
{
	bCellShading = false;

	glDepthFunc(GL_LESS);								// Reset The Depth-Testing Mode

	glPolygonMode(GL_FRONT, GL_FILL);					// Reset Back-Facing Polygon Drawing Mode

	glEnable(GL_LIGHTING);

	// Set to maps global color
	glColor4f(Map.fAmbientColor[0], Map.fAmbientColor[1], Map.fAmbientColor[2], 0.0f);
}

void CScene::EndRenderWater()
{
	bRenderWater = false;
}

list<CSceneObject>::iterator CScene::getInsertIndex(list<CSceneObject>::iterator it)
{
	float dist1 = CVector3::distance(mCameraPosition, it->go->mPosition);
	float dist2;

	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		if(i == it)
			continue;

		dist2 = CVector3::distance(mCameraPosition, i->go->mPosition);
		if(dist1 > dist2)
			return i;
	}

	return SceneObjects.end();
}

/*
 * GetObjectByID(): returns a pointer to the given ID
 */
CSceneObject* CScene::GetObjectByID(int id)
{
	CSceneObject* result = NULL;

	for(list<CSceneObject>::iterator it = SceneObjects.begin(); it != SceneObjects.end(); it++)
	{
		if(it->SID.getId() == id)
		{
			result = &(*it);
			return result;
		}
	}

	return result;
}

bool CScene::GetRenderWater()
{
	return bRenderWater;
}

void CScene::Manage()
{
	TimedOperation_Manage();

	if(SceneObjects.size() <= 0)
		return;

	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glEnable(GL_ARB_occlusion_query);

	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		i->OcclusionQuery();
		i->go->Update(g_fElpasedTime);
	}

	glDisable(GL_ARB_occlusion_query);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glFlush();
}

int CScene::ObjectHover(CVector3 p3d)
{
	CVector3 bb1, bb2;
	bool result = false;
	int childid = -1;

	for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	{
		switch(i->go->VisibleVolumeType)
		{
		case BoundingVolume::Box:
			result = i->go->BoxCollision(p3d, 1.0f);
			break;
		case BoundingVolume::Cylinder:
			result = i->go->CylinderCollision(p3d, 1.0f);
			break;
		case BoundingVolume::Sphere:
			result = i->go->SphereCollisionPoint(p3d);
			break;
		default:
			result = i->go->BoxCollision(p3d, 1.0f);
			break;
		}

		if(result)
		{
			if(i->Nodes.size())
			{
				childid = i->Hover(p3d);
			}
			if(childid != -1)
				iHoverObject = childid;
			else
				iHoverObject = i->SID.getId();

			sprintf(sHoverMessage, "%d: %s", iHoverObject, GetObjectByID(iHoverObject)->go->getName());

			if(childid == -1 && i->Nodes.size() > 0)
			{
				sprintf(sHoverMessage, "%s ( ", sHoverMessage);

				for(vector<CSceneID>::iterator j = i->Nodes.begin(); j != i->Nodes.end(); j++)
				{
					sprintf(sHoverMessage, "%s%d ", sHoverMessage, j->getId());
				}

				sprintf(sHoverMessage, "%s)", sHoverMessage);
			}

			return iHoverObject;
		}
	}

	iHoverObject = -1;
	return iHoverObject;
}

void CScene::ObjectIDMove(int id, CVector3 pos)
{
	if(id < 0)
		return;

	int oldobj = iActive;
	iActive = id;

	ObjectMove(pos);

	iActive = oldobj;
}

void CScene::ObjectIDRotate(int id, CVector3 rot)
{
	if(id < 0)
		return;

	int oldobj = iActive;
	iActive = id;

	ObjectRotate(rot);

	iActive = oldobj;
}

void CScene::ObjectIDSetPosition(int id, CVector3 pos)
{
	if(id < 0)
		return;

	int oldobj = iActive;
	iActive = id;

	ObjectSetPosition(pos);

	iActive = oldobj;
}

void CScene::ObjectIDSetRotation(int id, CVector3 rot)
{
	if(id < 0)
		return;

	int oldobj = iActive;
	iActive = id;

	ObjectSetRotation(rot);

	iActive = oldobj;
}

void CScene::ObjectMove(CVector3 pos)
{
	if(iActive < 0)
		return;

	GetObjectByID(iActive)->Move(pos);
}

/*
 * ObjectNodeAttach(): attach child to parent
 */
bool CScene::ObjectNodeAttach(int child, int parent)
{
	CSceneObject* psco = GetObjectByID(parent);

	if(psco != NULL)
		return psco->NodeAttach(child);
	else
		return false;
}

/*
 * ObjectNodeRemove(): remove child from parent
 */
bool CScene::ObjectNodeRemove(int child, int parent)
{
	CSceneObject* psco = GetObjectByID(parent);

	if(psco != NULL)
		return psco->NodeRemove(child);
	else
		return false;
}

void CScene::ObjectRotate(CVector3 rot)
{
	if(iActive < 0)
		return;

	GetObjectByID(iActive)->Rotate(rot);
}

void CScene::ObjectScale(float scale)
{
	if(iActive < 0)
		return;

	GetObjectByID(iActive)->go->fScale += scale;
	GetObjectByID(iActive)->SetScale(GetObjectByID(iActive)->go->fScale);
}

int CScene::ObjectSelect(CVector3 p3d)
{
	iActive = ObjectHover(p3d);
	return iActive;
}

void CScene::ObjectSelectNext()
{
	iActive++;

	if(iActive >= (signed)SceneObjects.size())
	    iActive = -1;
}

void CScene::ObjectSelectPrevious()
{
	iActive--;

	if(iActive < -1)
	    iActive = SceneObjects.size() - 1;
}

void CScene::ObjectSetPosition(CVector3 pos)
{
	if(iActive < 0)
		return;

	GetObjectByID(iActive)->SetPosition(pos);
}

void CScene::ObjectSetRotation(CVector3 rot)
{
	CSceneObject* active = GetObjectByID(iActive);
	if(!active)
		return;

	if(Editor->bAllowMoveX)
		active->go->mRotation.x = rot.x;
	if(Editor->bAllowMoveY)
		active->go->mRotation.y = rot.y;
	if(Editor->bAllowMoveZ)
		active->go->mRotation.z = rot.z;

	if(active->go->mRotation.x > 360.0f)
		active->go->mRotation.x -= 360.0f;
	if(active->go->mRotation.x < 0.0f)
		active->go->mRotation.x += 360.0f;
	if(active->go->mRotation.y > 360.0f)
		active->go->mRotation.y -= 360.0f;
	if(active->go->mRotation.y < 0.0f)
		active->go->mRotation.y += 360.0f;
	if(active->go->mRotation.z > 360.0f)
		active->go->mRotation.z -= 360.0f;
	if(active->go->mRotation.z < 0.0f)
		active->go->mRotation.z += 360.0f;

	active->go->RebuildBoundingBox();
}

void CScene::ObjectSetScale(float scale)
{
	if(iActive < 0)
		return;

	GetObjectByID(iActive)->SetScale(scale);
}

void CScene::Render()
{
	if(!SceneObjects.size())
		return;

	SortByDistance();

	for(list<CSceneObject>::iterator it = SceneObjects.begin(); it != SceneObjects.end(); it++)
	{
		it->Render();
	}

	// Shadows
	//if(Engine->iShadows > 0 && !Scene.bCellShading)
	//{
	//	glPushMatrix();
	//		glDisable(GL_LIGHTING);
	//		glDisable(GL_TEXTURE_2D);

	//		glRotatef(-go->mRotation.x, 1.0f, 0.0f, 0.0f);
	//		glRotatef(-go->mRotation.y, 0.0f, 1.0f, 0.0f);
	//		glRotatef(-go->mRotation.z, 0.0f, 0.0f, 1.0f);

	//		float e[] = { 0.0f,		0.8f,		0.0f };		// Point of the plane
	//		float l[] = { 0.0f,		0.0f,		0.0f };		// Coordinates of the light source
	//		float n[] = { 0.0f,		-1.0f,		0.0f };		// Normal list for the plane

	//		l[0] = Light[LIGHT_SUN].fvPosition[0] - go->mPosition.x;
	//		l[1] = Light[LIGHT_SUN].fvPosition[1] - go->mPosition.y;
	//		l[2] = Light[LIGHT_SUN].fvPosition[2] - go->mPosition.z;

	//		glShadowProjection(l, e, n);
	//		glColor3f(0.01f, 0.01f, 0.01f);

	//		for(list<CSceneObject>::iterator i = SceneObjects.begin(); i != SceneObjects.end(); i++)
	//		{
	//			i->Render();
	//		}

	//		glEnable(GL_TEXTURE_2D);
	//		glEnable(GL_LIGHTING);
	//	glPopMatrix();
	//}
}

void CScene::Reset()
{
	Clear();
}

void CScene::setCameraPosition(const CVector3 &pos)
{
	mCameraPosition = pos;
}

void CScene::SortByDistance()
{
	if(SceneObjects.size() <= 1)
		return;

	return;

	list<CSceneObject> tmpList = SceneObjects;
	list<CSceneObject>::iterator insert;

	for(list<CSceneObject>::iterator it = SceneObjects.begin(); it != SceneObjects.end(); it++)
	{
		CSceneObject tmp(*it);
		tmpList.insert(getInsertIndex(it), tmp);

		it++;
	}

	tmpList.unique();
	SceneObjects = tmpList;
}

void CScene::SortByMaterial()
{
	if(Count < 2)
		return;

	int index = -1;
	CMaterial actualmat, oldmat;

	int round = 0;
	int i = 0;
	list<CSceneObject>::iterator so = SceneObjects.begin();
	list<CSceneObject>::iterator oldso = SceneObjects.begin();
	list<CSceneObject>::iterator tmpso;

	while(round < Count)
	{
		so = SceneObjects.begin();
		oldso = so;

		switch(so->go->iType)
		{
		case EditorSpace::max3ds:
			index = g_Manager_3DSObject.IndexOf(so->go->getName());

			actualmat = g_Manager_Material.Materials[g_Manager_3DSObject.Mesh[index]->iMaterial];
			oldmat = actualmat;
			break;
		case EditorSpace::md2:
			index = g_Manager_Entity.IndexOf(so->go->getName());

			actualmat = g_Manager_Material.Materials[g_Manager_Entity.Object[index].iMaterial];
			oldmat = actualmat;
			break;
		case EditorSpace::Static:
			index = g_Manager_StaticObject.IndexOf(so->go->getName());

			actualmat = g_Manager_Material.Materials[g_Manager_StaticObject.Object[index].iMaterial];
			oldmat = actualmat;
			break;
		default:
			oldmat = g_Manager_Material.Materials[0];
			break;
		}

		i = 1;
		so = SceneObjects.begin();
		so++;

		while(i < Count)
		{
			switch(so->go->iType)
			{
			case EditorSpace::max3ds:
				index = g_Manager_3DSObject.IndexOf(so->go->getName());

				actualmat = g_Manager_Material.Materials[g_Manager_3DSObject.Mesh[index]->iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					tmpso = oldso;
					oldso = so;
					so = tmpso;
				}

				oldmat = actualmat;
				break;
			case EditorSpace::md2:
				index = g_Manager_Entity.IndexOf(so->go->getName());

				actualmat = g_Manager_Material.Materials[g_Manager_Entity.Object[index].iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					tmpso = oldso;
					oldso = so;
					so = tmpso;
				}

				oldmat = actualmat;
				break;
			case EditorSpace::Static:
				index = g_Manager_StaticObject.IndexOf(so->go->getName());

				actualmat = g_Manager_Material.Materials[g_Manager_StaticObject.Object[index].iMaterial];

				if(actualmat.bBlend && ! oldmat.bBlend)
				{
					tmpso = oldso;
					oldso = so;
					so = tmpso;
				}

				oldmat = actualmat;
				break;
			}

			oldso = so;
			i++;
			so++;
		}

		round++;
	}
}

void CScene::TimedOperation_Add(char* cmd, int repeat)
{
	if(strlen(cmd) == 0)
		return;

	CTimedOperation tOp;
	tOp.setId(TimedOperations.size());
	tOp.iRepeat = repeat;
	sprintf(tOp.sCommand, "%s", cmd);

	TimedOperations.push_back(tOp);
}

void CScene::TimedOperation_Manage()
{
	if(TimedOperations.size() <= 0)
		return;

	for(list<CTimedOperation>::iterator it = TimedOperations.begin(); it != TimedOperations.end(); it++)
	{
		if(it->iRepeat--)
			ProcessCommand(it->sCommand);
		else
		{
			TimedOperations.erase(it);
			break;
		}
	}
}