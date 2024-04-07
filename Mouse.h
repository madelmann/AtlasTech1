
#pragma once

// Library includes

// Project includes
#include "Tools.h"


namespace MouseSpace {

using namespace std;

typedef enum {
	None = 0,
	Down = 1,
	Up = 2,
	Click = 3
} MouseState_t;


class CMouse
{
public:
	CMouse()
	{
		Button1 = None;
		Button2 = None;
		Button3 = None;
		mp.x = 0;
		mp.y = 0;
		mp3d.x = 0.0f;
		mp3d.y = 0.0f;
		mp3d.z = 0.0f;
	}

	MouseState_t	Button1;
	MouseState_t	Button2;
	MouseState_t	Button3;
	double			Button1Begin;
	double			Button2Begin;
	double			Button3Begin;
	POINT			gmp;
	POINT			mp;
	CVector3		mp3d;
	POINT			Tile;
};

}
