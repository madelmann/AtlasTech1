#include "Main.h"


CMaterial::CMaterial()
{
	bAlphaTest = false;
	bBlend = false;
	bEnvironmentalMapping = false;
	fAlpha = 1.0f;
	fEmission = 0.0f;
	fShininess = 0.0f;
	//fvAmbientColor = (1.0f, 1.0f, 1.0f, 1.0f);
	//fvDiffuseColor = (0.0f, 0.0f, 0.0f, 0.0f);
	//fvSpecularColor = (0.0f, 0.0f, 0.0f, 0.0f);
	ID = -1;
	iFileVersion = VERSION_MATERIAL_ACTUAL;
	iTexture = -1;
	sprintf(Name, "");
	sprintf(Sound, "");
	sprintf(Texture, "");
}


CMaterialManager::CMaterialManager()
{
	Count = 0;
	iLastUsed = -1;
}

int CMaterialManager::Add(char* Filename)
{
	if(strlen(Filename) == 0)
		return -1;

	Filename = RemoveFileExt(Filename);

	int index = IndexOf(Filename);
	if(index != -1)
		return index;

	char FName[255] = "";
	sprintf(FName, "%sMaps\\%s\\Materials\\%s.material", g_sGameName, Game.sMapName, Filename);
	
	FILE *pFile = NULL;

	// Open a file pointer to the material file and check if it was found and opened 
	if(fopen_s(&pFile, FName, "rb") != NULL) 
	{
		sprintf(FName, "%sMaterials\\%s.material", g_sGameName, Filename);

		if(fopen_s(&pFile, FName, "rb") != NULL) 
		{
			// Display an error message saying the file was not found, then return NULL
			char buffer[255] = "";
			sprintf(buffer, "ERROR[CMaterialManager.Add]: '%s.material' not found!\n", Filename);
			Console->Output(buffer);
			return -1;
		}
	}
	
	// Read the header data and store it in our m_Header member variable
	fread(&Materials[Count].iFileVersion, 1, sizeof(int), pFile);

	switch(Materials[Count].iFileVersion)
	{
	case VERSION_MATERIAL_1:
		fread(&Materials[Count].bBlend, 1, sizeof(bool), pFile);
		fread(&Materials[Count].bEnvironmentalMapping, 1, sizeof(bool), pFile);
		fread(&Materials[Count].fShininess, 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].Name, 1, sizeof(char[255]), pFile);
		fread(&Materials[Count].Sound, 1, sizeof(char[255]), pFile);
		fread(&Materials[Count].Texture, 1, sizeof(char[255]), pFile);
		break;
	case VERSION_MATERIAL_ACTUAL:
		fread(&Materials[Count].bAlphaTest, 1, sizeof(bool), pFile);
		fread(&Materials[Count].bBlend, 1, sizeof(bool), pFile);
		fread(&Materials[Count].bEnvironmentalMapping, 1, sizeof(bool), pFile);
		fread(&Materials[Count].fAlpha, 1, sizeof(float), pFile);
		fread(&Materials[Count].fShininess, 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvAmbientColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvDiffuseColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[0], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[1], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[2], 1, sizeof(float), pFile);
		fread(&Materials[Count].fvSpecularColor[3], 1, sizeof(float), pFile);
		fread(&Materials[Count].Name, 1, sizeof(char[255]), pFile);
		fread(&Materials[Count].Sound, 1, sizeof(char[255]), pFile);
		fread(&Materials[Count].Texture, 1, sizeof(char[255]), pFile);
		break;
	}
	fclose(pFile);

	Materials[Count].ID = Count;
	Materials[Count].iTexture = Texture_Add(Materials[Count].Texture);

	Count++;

	return Count - 1;
}

int CMaterialManager::Apply(int index, bool bForceMaterialUse)
{
	if(index < 0)
		return -1;

	if(iLastUsed == index && !bForceMaterialUse)
		return -1;

	if(Engine->bColorMaterial)
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
		glColor4fv(Materials[index].fvAmbientColor);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glColor4fv(Materials[index].fvDiffuseColor);
		glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
		glColor4fv(Materials[index].fvSpecularColor);
		//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Materials[index].fShininess);
		glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
		glColor4fv(Materials[index].fvSpecularColor);
	}

	if(Materials[index].bBlend)
	{
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
	}

	if(!Engine->bColorMaterial)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   Materials[index].fvAmbientColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   Materials[index].fvDiffuseColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  Materials[index].fvSpecularColor);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, Materials[index].fShininess);
		glMaterialf (GL_FRONT_AND_BACK, GL_EMISSION,  Materials[index].fEmission);
	}

	if(Materials[index].bEnvironmentalMapping)
	{
		//glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For R To Sphere Mapping
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping

		//glEnable(GL_TEXTURE_GEN_R);									// Enable Texture Coord Generation For R
		glEnable(GL_TEXTURE_GEN_S);									// Enable Texture Coord Generation For S
		glEnable(GL_TEXTURE_GEN_T);									// Enable Texture Coord Generation For T
	}
	else
	{
		//glDisable(GL_TEXTURE_GEN_R);								// Disable Texture Coord Generation For R
		glDisable(GL_TEXTURE_GEN_S);								// Disable Texture Coord Generation For S
		glDisable(GL_TEXTURE_GEN_T);								// Disable Texture Coord Generation For T
	}

	if(Materials[index].bAlphaTest)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, Materials[index].fAlpha);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}

	iLastUsed = index;

	Texture_SetActiveID(Materials[index].iTexture);

	return 1;
}

void CMaterialManager::Clear()
{
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		Materials[i].ID = -1;
	}

	Count = 0;
	iLastUsed = -1;
}

int CMaterialManager::IndexOf(char* Filename)
{
	if(Count <= 0)
		return -1;

	for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(strcmpi(Filename, Materials[i].Name) == 0)
			return i;
	}

	return -1;
}