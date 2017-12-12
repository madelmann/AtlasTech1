#ifndef _FLORA_H
#define _FLORA_H


#include "Main.h"


#define FLORA_MAX	1000


class CFlora
{
public:
		CFlora();
		void PreProcess();
		void Render();
		void RenderWithoutDisplayList();

		float		fRadius;
		float		fScale;
		int			ID;
		int			iMeshID;
		int			iTexture;
		CVector3	vPosition;
};

class CFloraManager
{
public:
		CFloraManager();
		~CFloraManager();
		int Add(float x, float z, char texturename[255]);
		int Delete(int index);
		void Render();
		void Spread();
		
		int		Count;
		int		iMax;
		CFlora	Flora[FLORA_MAX];
};

#endif