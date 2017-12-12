#ifndef _EDITOR_H
#define _EDITOR_H


#include "Main.h"
#include "Texture.h"

namespace EditorSpace {

enum Mode_t {
	Selection	= 0,
	max3ds		= 1,
	md2			= 2,
	Static		= 3,
	Bot			= 4,
	Particle	= 5,
	Water		= 6,
	Fog			= 7,
	Light		= 8,
	Terrain		= 9,
	Translation	= 10,
	Rotation	= 11
};


class Editor: public CSingleton< Editor >
{
	//friend class CSingleton< Editor >;

public:
	Editor(CTextureManager *texturemanager);
	~Editor() { }

	void Allow();
	void DeInit();
	Mode_t GetMode();
	void Init();
	Mode_t NextMode();
	bool PreLoadData();
	void PreLoadMaterials();
	void PreLoadModels();
	void PreLoadObjects();
	void PreLoadParticleSystems();
	void PreLoadStaticObjects();
	void PreLoadTextures();
	void SetMode(Mode_t mode);

	bool	bAllowed;
	bool	bAllowMoveX;
	bool	bAllowMoveY;
	bool	bAllowMoveZ;
	bool	bEnabled;
	bool	bInitialisized;

private:
	Mode_t			mMode;
	CTextureManager	*mTextureManager;
};

}

#endif