
#pragma once

// Library includes

// Project includes

#define MAX_MATERIALS			100
#define VERSION_MATERIAL_1		1
#define VERSION_MATERIAL_2		2
#define VERSION_MATERIAL_ACTUAL	VERSION_MATERIAL_2


class CMaterial
{
public:
	CMaterial();

	bool	bAlphaTest;
	bool	bBlend;
	bool	bEnvironmentalMapping;
	float	fAlpha;
	float	fEmission;
	float	fShininess;
	float	fvAmbientColor[4];
	float	fvDiffuseColor[4];
	float	fvSpecularColor[4];
	int		ID;
	int		iFileVersion;
	int		iTexture;
	char	Name[255];
	char	Sound[255];
	char	Texture[255];
};


class CMaterialManager
{
public:
	CMaterialManager();
	int Add(char* Filename);
	int Apply(int index, bool bForceMaterialUse);
	void Clear();
	int IndexOf(char* Filename);

	int			Count;
	int			iLastUsed;
	CMaterial	Materials[MAX_MATERIALS];
};
