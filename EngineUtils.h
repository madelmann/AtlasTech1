
#pragma once

// Library includes

// Project includes
//#include "GLWrapper.h"
#include "3DSObjectManager.h"
#include "BasicInterpreter.h"
#include "Bot.h"
#include "Bullet.h"
#include "Camera.h"
#include "Chat.h"
#include "Console.h"
#include "Editor.h"
#include "Entity.h"
#include "FileManager.h"
//#include "Flora.h"
#include "Font.h"
#include "Frustum.h"
#include "Globals.h"
#include "Light.h"
#include "Main.h"
#include "Map.h"
#include "Material.h"
#include "Mouse.h"
#include "Normals.h"
#include "Object.h"
#include "ParticleSystem.h"
#include "PhysicManager.h"
#include "Player.h"
#include "QuadTree.h"
#include "ObjectManager.h"
#include "Regions.h"
#include "Scene.h"
#include "Script.h"
#include "Shader.h"
#include "Sound.h"
#include "StaticObject.h"
#include "Statistics.h"
#include "Tools.h"
#include "TGA.h"
#include "Texture.h"
#include "Water.h"
#include "UserInterface.h"



namespace EngineSpace {
	class CEngine;
}

// Global classes
//extern EngineSpace::CEngine*	Engine;
extern CBasicInterpreter	g_BasicInterpreter;
extern CBotManager			g_Manager_Bot;
extern CBulletManager		g_Manager_Bullet;
extern CFPSCamera			g_Camera;
extern CChatManager			g_Manager_Chat;
//extern CConsole*			Console;
extern EditorSpace::Editor* Editor;
extern CEntityManager		g_Manager_Entity;
extern CFileManager			g_Manager_File;
//extern CFloraManager		g_Manager_Flora;
extern CGLFont				Font;												// Font object used to display text to the screen
extern CFrustum				Frustum;
extern CLight				Light[8];
extern CMap					Map;
extern CMaterialManager		g_Manager_Material;
extern MouseSpace::CMouse	Mouse;
extern C3DSObjectManager	g_Manager_3DSObject;
extern CObjectManager		g_Manager_Object;
extern CParticleManager		g_Manager_ParticleSystem;
extern CPhysicManager		g_Manager_Physic;
extern CPlayerManager*		g_Manager_Player;
extern CQuadTree			QuadTree;
extern CRegionManager		g_Manager_Region;
extern CScene				Scene;
extern CScriptManager		g_Manager_Script;
extern CShaderManager*		g_Manager_Shader;
extern CSoundManager		g_Manager_Sound;
extern CStaticObjectManager	g_Manager_StaticObject;
extern CStatisticsManager	g_Manager_Statistic;
extern CTextureManager*		g_Manager_Texture;
extern CWater				Water;
//extern UI::CUIManager		GUI;

extern CGraphicObject* Entity_add(char* filename);
extern CSceneObject* GetObjectByID(int id);
//extern int Material_Add(char* Filename);
//extern int Material_Apply(int index, bool bForceMaterialUse);
extern GLuint Texture_Add(char* Filename);
extern GLuint Texture_IndexOf(char* Filename);
extern void Texture_SetActive(char* Filename);
//extern void Texture_SetActiveID(int id);
