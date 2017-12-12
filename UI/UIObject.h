#ifndef _UIOBJECT_H
#define _UIOBJECT_H


#include <string>
#include <vector>
#include <windows.h>

#include "..\Font.h"
#include "..\Mouse.h"
#include "..\Tools.h"


namespace UI {

using namespace std;
using namespace MouseSpace;


#define CLICK_TIME			1000

#define TEXTURESTATE_NORMAL	1
#define TEXTURESTATE_DOWN	2
#define TEXTURESTATE_HOVER	3

typedef enum {
	Bottom = 0,
	BottomMiddle = 1,
	Middle = 2,
	MiddleMiddle = 3,
	Top = 4,
	TopMiddle = 5
} TextPosition_t;


class CGraphicControl
{
public:
	CGraphicControl();
	virtual ~CGraphicControl();

	void add(CGraphicControl* control);
	bool CheckPosition(float x, float y);
	void Clear();
	CGraphicControl* find(string control);
	void Hide();
	void Hover();
	void init();
	bool IsVisible();
	void OnClick();
 	void Render();
	void SetAlpha(float a);
	void SetBlending(float b);
	void SetClick(void (*c)());
	void SetColor(CVector3 color);
	void SetColor(CVector3 color1, CVector3 color2);
	void SetColor(CVector3 color1, CVector3 color2, CVector3 color3);
	void SetColorStates(int colorstates);
	bool SetImage(char* Filename, int numtexstat);
	void SetPosition(float x, float y);
	void SetPosition(int x, int y);
	void SetScreen(CScreen* screen);
	void SetSize(float w, float h);
	void SetSize(int w, int h);
	void SetTextureState(int state);
	void Show();
	void Update(CMouse *mouse);

	string				Caption;
	void				(*Click)();
	std::vector<CGraphicControl*>	Controls;
	bool				Enabled;
	float				Height;
	string				Hint;
	float				Left;
	string				Name;
	int					Tag;
	TextPosition_t		Textposition;
	float				Top;
	bool				Visible;
	float				Width;

private:
	float				fAlpha;
	float				fBlend;
	bool				Hidden;
	int					iColorState;
	int					iTexture;
	CGraphicControl*	mParent;
	CScreen*			mScreen;
	int					NumTextureStates;
	float				oglLeft;
	float				oglTop;
	int					TextureState;
	CVector3			vColor[3];
};


class CUIManager
{
public:
	CUIManager();
	~CUIManager();

	void add(CGraphicControl* form);
	CGraphicControl* find(string control);
	CGraphicControl* getactive();
	bool isactive(CGraphicControl* form);
	void popback(CGraphicControl* popback);
	void popup(CGraphicControl* popup);
	void render();
	void setactive(CGraphicControl* form);
	void setscreen(CScreen* screen);
	void update(CMouse *mouse);


	CGraphicControl*				ActiveForm;
	CScreen*						mScreen;

private:
	std::vector<CGraphicControl*>	ActiveForms;
	int								Count;
	std::vector<CGraphicControl*>	Forms;
};

};

#endif