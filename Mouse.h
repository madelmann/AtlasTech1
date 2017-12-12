#ifndef _MOUSE_H
#define _MOUSE_H


#include "Tools.h"


namespace MouseSpace {

using namespace std;

typedef enum MouseState {
	None = 0,
	Down = 1,
	Up = 2,
	Click = 3
};


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

	MouseState	Button1;
	MouseState	Button2;
	MouseState	Button3;
	double		Button1Begin;
	double		Button2Begin;
	double		Button3Begin;
	POINT		gmp;
	POINT		mp;
	CVector3	mp3d;
	POINT		Tile;
};

}

#endif