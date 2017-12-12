#ifndef _MAP_H
#define _MAP_H


#include "Tools.h"
#include "Main.h"
//#include "vector3f.h"


#define MAP_VERSION_1		8
#define MAP_VERSION_2		2
#define MAP_VERSION_3		3
#define MAP_VERSION_4		4
#define MAP_VERSION_ACTUAL	MAP_VERSION_4

#define MAP_MAXOBJECTS		100


struct TMapHeader
{ 
	int magic;					// This is used to identify the file
	int version;				// The version number of the file
};


struct TObjectData
{
	float	fAngleX;
	float	fAngleY;
	float	fAngleZ;
	float	fScale;
	int		iObjectIndex;
	int		iObjectType;
	char	Objectname[255];
	char	Texturename[255];
	float	x;
	float	y;
	float	z;
};


class CMap
{
public:
		CMap();

		void AddObject(int objtype, char* objname, float x, float y, float z, float scale);
		void ClearObjectData();
		void CreateFolders(char Filename[255]);
		bool DeleteIndex(long id);
		bool Load(char Filename[255]);
		void LoadSkybox();
		void New();
		void PreLoadMaterials();
		void PreLoadModels();
		void PreLoadObjects();
		void PreLoadParticleSystems();
		void PreLoadStaticObjects();
		void PreLoadTextures();
		void ProcessWaves();
		bool Save(char Filename[255]);
		void SetGravity(CVector3 grav);
		void SortByMaterial();
		void UnLoad();
		void UpdateDataFromScene();

		TMapHeader	m_Header;					// The file header
		bool		bFirstFrame;				// 
		bool		bFogAffectsSkybox;			// Indicates if the fog has effects on the skybox
		bool		bSinglePlayer;				// Indicates if this map has been designed for single- or multiplayer games
		bool		bWaterEnabled;				// Enable/disable water
		bool		bWaterNormals;				// Indicates that the water should have normals applied
		bool		bWaterReflections;			// Indicates that the water should reflect the environment
		bool		bWaterRefractions;			// Indicates that the water has refractions
		float		fAmbientColor[3];			// Ambient color
		float		fFloorOffset;				// Floor offset
		float		fFogDensity;				// The density of the maps's fog
		float		fFogDepth;					// Fog depth
		float		fRoofOffset;				// Roof offset
		float		fWaterFlow;					// The speed of the water flow
		float		fWaterHeight;				// The height of the map's water
		float		fWaterHeightBase;			// The height of the map's water
		float		fWaveHeight;				// The height of the map's waves
		int			iMaterial;					// Material index
		int			iObjectCount;				// The number of objects
		int			iVersion;					// Map version number, used to identify updated maps (download)
		int			iWaveInterval;				// The time between 2 waves in ms
		TObjectData	Object[MAP_MAXOBJECTS];		// Holds all the map's object data
		char		sAlphaMap[255];				// Alpha texture
		char		sBaseTexture[255];			// Base texture
		char		sFloorMap[255];				// The filename of the heightmap for the floor
		char		sMapName[255];				// The actual map file
		char		sRoofMap[255];				// The filename of the heightmap for the roof
		char		sSecondTexture[255];		// Second texture
		CVector3	vGravity;					// Gravity
		CVector3	vSinglePlayerStartAngle;	// The player's start view (single player)
		CVector3	vSinglePlayerStartPoint;	// The player's start position (single player)
		CVector3	vWaterColor;				// Water color
		CVector3	vWind;						// Wind direction
};


#endif