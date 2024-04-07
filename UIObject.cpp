
// Library includes

// Project includes
#include "UI\UIObject.h"


namespace UI {


CGraphicControl::CGraphicControl()
{
	fAlpha = 0.0f;
	fBlend = 0.0f;
	Click = NULL;
	Caption = "UIObject";
	Enabled = true;
	Height = 50.0f;
	Hidden = false;
	Hint = "";
	iColorState = 1;
	iTexture = 0;
	Left = 0.0f;
	mParent = NULL;
	Name = "UIObject";
	NumTextureStates = 1;
	oglLeft = 0.0f;
	oglTop = 0.0f;
	Tag = -1;
	Textposition = Bottom;
	TextureState = TEXTURESTATE_NORMAL;
	Top = 0.0f;
	Visible = false;
	vColor[0] = CVector3(1.0f, 1.0f, 1.0f);
	vColor[1] = CVector3(1.0f, 1.0f, 1.0f);
	vColor[2] = CVector3(1.0f, 1.0f, 1.0f);
	Width = 50.0f;
}

CGraphicControl::~CGraphicControl()
{
	Controls.clear();
}

void CGraphicControl::Add(CGraphicControl* control)
{
	control->mParent = this;
	control->Visible = true;

	if(control->Name.length() <= 0)
		control->Name = control->Caption;

	Controls.push_back(control);
}

bool CGraphicControl::CheckPosition(float x, float y)
{
	if(		x >= mParent->Left + Left && x <= mParent->Left + Left + Width
		&&	y >= mParent->Top + Top && y <= mParent->Top + Top + Height)
		return true;
	else
		return false;
}

void CGraphicControl::Clear()
{
	Controls.clear();
}

CGraphicControl* CGraphicControl::find(string control)
{
	CGraphicControl* result;

	for(vector<CGraphicControl*>::iterator i = Controls.begin(); i != Controls.end(); i += 1)
	{
		if(StringCompare((*i)->Name, control))
		{
			return (*i);
		}
		if((*i)->Controls.size())
		{
			result = (*i)->find(control);
			if(result)
				return result;
		}
	}

	return NULL;
}

void CGraphicControl::Hide()
{
	Hidden = true;
	Visible = false;

	for(vector<CGraphicControl*>::iterator i = Controls.begin(); i != Controls.end(); i += 1)
	{
		(*i)->Hidden = true;
	}
}

void CGraphicControl::Hover()
{
}

bool CGraphicControl::IsVisible()
{
	return !Hidden && Visible;
}

void CGraphicControl::OnClick()
{
	if(Hidden || !Visible || !Enabled)
		return;

	if(Click != NULL)
		Click();
}

void CGraphicControl::Render()
{
	if(Hidden || !Visible)
		return;

	glPushMatrix();

	// Render controls
	for(vector<CGraphicControl*>::iterator i = Controls.begin(); i != Controls.end(); i += 1)
	{
		(*i)->Render();
	}

	float left = oglLeft;
	float top = oglTop;

	if(mParent)
	{
		left = mParent->oglLeft + oglLeft;
		top = mParent->oglTop + oglTop - Engine->iScreenHeight;
	}

	if(iTexture != -1)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();								// Push on a new matrix so that we can just pop it off to go back to perspective mode
			glLoadIdentity();						// Reset the current matrix to our identify matrix

			// Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).
			// The last 2 parameters are the near and far planes.
			glOrtho(0, Engine->iScreenWidth, 0, Engine->iScreenHeight, 0, 10);

			glAlphaFunc(GL_GEQUAL, fAlpha);
			glEnable(GL_ALPHA_TEST);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_BLEND);

			float texcoordx = (float)(TextureState - 1) / (float)NumTextureStates;
			float texcoordx2 = (float)TextureState / (float)NumTextureStates;
			//float texcoordx2 = texcoordx + 1 / NumTextureStates;

			Texture_SetActiveID(iTexture);
			glBegin(GL_QUADS);
				glTexCoord2f(texcoordx,		1.0f); glVertex2f(left,			top);
				glTexCoord2f(texcoordx2,	1.0f); glVertex2f(left + Width,	top);
				glTexCoord2f(texcoordx2,	0.0f); glVertex2f(left + Width,	top - Height);
				glTexCoord2f(texcoordx,		0.0f); glVertex2f(left,			top - Height);
				//glTexCoord2f(0.0f,	0.0f); glVertex2f(left,			top);
				//glTexCoord2f(1.0f,	0.0f); glVertex2f(left + Width,	top);
				//glTexCoord2f(1.0f,	1.0f); glVertex2f(left + Width,	top - Height);
				//glTexCoord2f(0.0f,	1.0f); glVertex2f(left,			top - Height);
			glEnd();

			//glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		glPopMatrix();           
		glMatrixMode(GL_MODELVIEW);					// Go back to our model view matrix like normal
	}

	if(Caption.length() >= 0)
	{
		float x = (left - g_iScreenMiddleX) / g_iScreenMiddleX;
		float y = 0.0f;

		switch(Textposition)
		{
		case UI::Bottom:
			y = (top - Height - g_iScreenMiddleY) / g_iScreenMiddleY;
			break;
		case UI::BottomMiddle:
			x = (left - g_iScreenMiddleX) / g_iScreenMiddleX + ((Width * 0.5f - Font.GetFontSize() * 0.5f * Caption.length() * 0.5f) / g_iScreenMiddleX);
			y = (top - Height - g_iScreenMiddleY) / g_iScreenMiddleY;
			break;
		case UI::Middle:
			y = (top - g_iScreenMiddleY) / g_iScreenMiddleY - (Height * 0.5f / g_iScreenMiddleY) - (Font.GetFontSize() * 0.33f / g_iScreenMiddleY);
			break;
		case UI::MiddleMiddle:
			x = (left - g_iScreenMiddleX) / g_iScreenMiddleX + ((Width * 0.5f - Font.GetFontSize() * 0.5f * Caption.length() * 0.5f) / g_iScreenMiddleX);
			y = (top - g_iScreenMiddleY) / g_iScreenMiddleY - (Height * 0.5f / g_iScreenMiddleY) - (Font.GetFontSize() * 0.33f / g_iScreenMiddleY);
			break;
		case UI::Top:
			y = (top - g_iScreenMiddleY) / g_iScreenMiddleY;
			break;
		case UI::TopMiddle:
			x = (left - g_iScreenMiddleX) / g_iScreenMiddleX + ((Width * 0.5f - Font.GetFontSize() * 0.5f * Caption.length() * 0.5f) / g_iScreenMiddleX);
			y = (top - g_iScreenMiddleY) / g_iScreenMiddleY;
			break;
		}

		glColor3f(vColor[iColorState].x, vColor[iColorState].y, vColor[iColorState].z);
		glPrintText(x, y, &Caption);
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	glPopMatrix();
}

void CGraphicControl::SetAlpha(float a)
{
	fAlpha = a;
}

void CGraphicControl::SetBlending(float b)
{
	fBlend = b;
}

void CGraphicControl::SetClick(void (*c)())
{
	Click = c;
}

void CGraphicControl::SetColor(CVector3 color)
{
	vColor[0] = color;
	vColor[1] = color;
	vColor[2] = color;
}

void CGraphicControl::SetColor(CVector3 color1, CVector3 color2)
{
	vColor[0] = color1;
	vColor[1] = color2;
	vColor[2] = color2;
}

void CGraphicControl::SetColor(CVector3 color1, CVector3 color2, CVector3 color3)
{
	vColor[0] = color1;
	vColor[1] = color2;
	vColor[2] = color3;
}

void CGraphicControl::SetColorStates(int colorstate)
{
	iColorState = colorstate;
}

bool CGraphicControl::SetImage(char* Filename, int numtexstat)
{
	iTexture = Texture_Add(Filename);
	
	NumTextureStates = numtexstat;

	if(iTexture == -1)
		return false;
	else
		return true;
}

void CGraphicControl::SetPosition(float x, float y)
{
	Left = x;
	Top = y;

	oglLeft = x;
	oglTop = Engine->iScreenHeight - y;
}

void CGraphicControl::SetPosition(int x, int y)
{
	Left = (float)x;
	Top = (float)y;

	oglLeft = (float)x;
	oglTop = Engine->iScreenHeight - (float)y;
}

void CGraphicControl::SetSize(float w, float h)
{
	Height = h;
	Width = w;
}

void CGraphicControl::SetSize(int w, int h)
{
	Height = (float)h;
	Width = (float)w;
}

void CGraphicControl::SetTextureState(int state)
{
	TextureState = state;
}

void CGraphicControl::Show()
{
	Hidden = false;
	Visible = true;

	for(vector<CGraphicControl*>::iterator i = Controls.begin(); i != Controls.end(); i += 1)
	{
		(*i)->Hidden = false;
		(*i)->SetColorStates(MouseSpace::None);
		(*i)->SetTextureState(TEXTURESTATE_NORMAL);
	}
}

void CGraphicControl::Update()
{
	if(Hidden || !Visible)
		return;

	for(vector<CGraphicControl*>::iterator i = Controls.begin(); i != Controls.end(); i += 1)
	{
		if((*i)->CheckPosition((float)Mouse.mp.x, (float)Mouse.mp.y))
		{
			if(Mouse.Button1 == MouseSpace::Click)
			{
				if((*i)->Enabled)
				{
					(*i)->OnClick();
					(*i)->SetColorStates(1);
					(*i)->SetTextureState(2);
				}
				return;
			}
			else
			{
				if((*i)->Enabled)
				{
					switch(Mouse.Button1)
					{
					case MouseSpace::Down:
						(*i)->SetColorStates(2);
						(*i)->SetTextureState(3);
						break;
					case MouseSpace::None:
					case MouseSpace::Up:
					default:
						(*i)->SetColorStates(1);
						(*i)->SetTextureState(2);
						break;
					};
					
					(*i)->Hover();
				}
				return;
			}
		}
		else
		{
			(*i)->SetColorStates(0);
			(*i)->SetTextureState(1);
		}
	}

	if(Mouse.Button1 == MouseSpace::Click) {
		OnClick();
	}

	SetColorStates(Mouse.Button1);
	SetTextureState(Mouse.Button1 + 1);
}


CUIManager::CUIManager()
{
	ActiveForm = NULL;
	ActiveForms.clear();
	Count = 0;
	Forms.clear();
}

CUIManager::~CUIManager()
{
	Forms.clear();
}

void CUIManager::add(CGraphicControl* form)
{
	Forms.push_back(form);
	Count = Forms.size();
}

CGraphicControl* CUIManager::find(string control)
{
	CGraphicControl* result;

	for(vector<CGraphicControl*>::iterator i = Forms.begin(); i != Forms.end(); i += 1)
	{
		if(StringCompare((*i)->Name, control))
		{
			return (*i);
		}
		if((*i)->Controls.size())
		{
			result = (*i)->find(control);
			if(result)
				return result;
		}
	}

	return NULL;
}

CGraphicControl* CUIManager::getActive()
{
	return ActiveForm;
}

bool CUIManager::isActive(CGraphicControl* form)
{
	if(form == ActiveForm)
		return true;
	else
		return false;
}

void CUIManager::popUp(CGraphicControl* popup)
{
	if(!popup)
		return;

	ActiveForm = popup;
	ActiveForms.push_back(popup);
	popup->Show();
}

void CUIManager::popBack(CGraphicControl* popback)
{
	if(!popback || !popback->IsVisible())
		return;

	if(ActiveForms.size() == 1)
		return;

	popback->Hide();
	ActiveForm = NULL;

	ActiveForms.pop_back();
	ActiveForm = ActiveForms[ActiveForms.size() - 1];
	ActiveForm->Show();
}

void CUIManager::render()
{
	if(Count <= 0)
		return;

	if(!ActiveForm)
		return;

	ActiveForm->Render();

	//for(vector<CForm*>::iterator i = Forms.begin(); i != Forms.end(); i += 1)
	//{
	//	(*i)->Render();
	//}
}

void CUIManager::setActive(CGraphicControl* form)
{
	ActiveForm = form;
}

void CUIManager::update(float x, float y)
{
	if(Count <= 0)
		return;

	bool done = false;

	if(ActiveForm != NULL)
	{
		ActiveForm->Update();
		done = true;
	}

	if(!done)
	{
		for(vector<CGraphicControl*>::iterator i = Forms.begin(); i != Forms.end(); i += 1)
		{
			(*i)->Update();
		}
	}

	Mouse.Button1 = MouseSpace::None;
	Mouse.Button2 = MouseSpace::None;
	Mouse.Button3 = MouseSpace::None;
}

void CUIManager::update(int x, int y)
{
	update((float)x, (float)y);
}

}
