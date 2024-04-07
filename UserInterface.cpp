
// Library includes

// Project includes
#include "Main.h"
#include "UserInterface.h"


UI::CUIManager		GUI;

// User interface - Begin
UI::CGraphicControl		Gameform;
//UI::CGraphicControl		Gameform_
UI::CGraphicControl		Mainform;
UI::CGraphicControl		Mainform_ModsButton;
UI::CGraphicControl		Mainform_MultiplayerButton;
UI::CGraphicControl		Mainform_NewGameButton;
UI::CGraphicControl		Mainform_QuitButton;
UI::CGraphicControl		Mainform_SettingsButton;
//UI::CGraphicControl		Mainform_
UI::CGraphicControl		Modsform;
//UI::CGraphicControl		Modsform_
UI::CGraphicControl		Multiplayerform;
UI::CGraphicControl		Multiplayerform_BackButton;
UI::CGraphicControl		Multiplayerform_ConnectButton;
//UI::CGraphicControl		Multiplayerform_
UI::CGraphicControl		Settingsform;
//UI::CGraphicControl		Settingsform_
// User interface - End


void InitGameform()
{
	// Gameform
	//Gameform.SetScreen(GUI.mScreen);
	//Gameform.Caption = "Gameform";
	//Gameform.Name = "Gameform";
	//Gameform.SetImage("Screen_Menu.bmp", 1);
	//Gameform.SetPosition(0.0f, 0.0f);
	//Gameform.SetSize(GUI.mScreen->Width, GUI.mScreen->Height);
	//GUI.add(&Gameform);
}

void InitMainform()
{
	// Mainform
	Mainform.SetScreen(GUI.mScreen);
	Mainform.Caption = "Mainform";
	Mainform.Name = "Mainform";
	Mainform.SetImage("Screen_Menu.bmp", 1);
	Mainform.SetPosition(0.0f, 0.0f);
	Mainform.SetSize(GUI.mScreen->Width, GUI.mScreen->Height);
	GUI.add(&Mainform);

	// Mainform: New game button
	Mainform_NewGameButton.Caption = "New Game";
	Mainform_NewGameButton.SetAlpha(0.8f);
	Mainform_NewGameButton.SetClick(&Mainform_NewGameClick);
	Mainform_NewGameButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Mainform_NewGameButton.SetImage("Interface\\Dummy.bmp", 3);
	Mainform_NewGameButton.SetPosition(160, 380);
	Mainform_NewGameButton.SetSize(110, 20);
	Mainform.add(&Mainform_NewGameButton);

	// Mainform: Multiplayer button
	Mainform_MultiplayerButton.Caption = "Multiplayer";
	Mainform_MultiplayerButton.SetAlpha(0.8f);
	Mainform_MultiplayerButton.SetClick(&Mainform_MultiplayerClick);
	Mainform_MultiplayerButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Mainform_MultiplayerButton.SetImage("Interface\\Dummy.bmp", 3);
	Mainform_MultiplayerButton.SetPosition(160, 405);
	Mainform_MultiplayerButton.SetSize(110, 20);
	Mainform.add(&Mainform_MultiplayerButton);

	// Mainform: Settings button
	Mainform_SettingsButton.Caption = "Settings";
	Mainform_SettingsButton.SetAlpha(0.8f);
	Mainform_SettingsButton.SetClick(&Mainform_SettingsClick);
	Mainform_SettingsButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Mainform_SettingsButton.SetImage("Interface\\Dummy.bmp", 3);
	Mainform_SettingsButton.SetPosition(160, 430);
	Mainform_SettingsButton.SetSize(110, 20);
	Mainform.add(&Mainform_SettingsButton);

	// Mainform: Mods button
	Mainform_ModsButton.Caption = "Mods";
	Mainform_ModsButton.SetAlpha(0.8f);
	Mainform_ModsButton.SetClick(&Mainform_ModsClick);
	Mainform_ModsButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Mainform_ModsButton.SetImage("Interface\\Dummy.bmp", 3);
	Mainform_ModsButton.SetPosition(160, 455);
	Mainform_ModsButton.SetSize(60, 20);
	Mainform.add(&Mainform_ModsButton);

	// Mainform: Quit button
	Mainform_QuitButton.Caption = "Quit";
	Mainform_QuitButton.SetAlpha(0.8f);
	Mainform_QuitButton.SetClick(&Mainform_QuitClick);
	Mainform_QuitButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Mainform_QuitButton.SetImage("Interface\\Dummy.bmp", 3);
	Mainform_QuitButton.SetPosition(160, 480);
	Mainform_QuitButton.SetSize(60, 20);
	Mainform.add(&Mainform_QuitButton);
}

void InitModsform()
{
	// Modsform
	Modsform.SetScreen(GUI.mScreen);
	Modsform.Caption = "Modsform";
	Modsform.Name = "Modsform";
	Modsform.SetImage("Screen_Menu.bmp", 1);
	Modsform.SetPosition(0.0f, 0.0f);
	Modsform.SetSize(GUI.mScreen->Width, GUI.mScreen->Height);
	GUI.add(&Modsform);
}

void InitMultiplayerform()
{
	// Multiplayerform
	Multiplayerform.SetScreen(GUI.mScreen);
	Multiplayerform.Caption = "Multiplayerform";
	Multiplayerform.Name = "Multiplayerform";
	Multiplayerform.SetImage("Screen_Menu.bmp", 1);
	Multiplayerform.SetPosition(0.0f, 0.0f);
	Multiplayerform.SetSize(GUI.mScreen->Width, GUI.mScreen->Height);
	GUI.add(&Multiplayerform);

	// Back button
	Multiplayerform_BackButton.Caption = "Back";
	//Multiplayerform_BackButton.SetAlpha(0.8f);
	Multiplayerform_BackButton.SetClick(&Back);
	Multiplayerform_BackButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Multiplayerform_BackButton.SetImage("Interface\\Dummy.bmp", 3);
	Multiplayerform_BackButton.SetPosition(160, 560);
	Multiplayerform_BackButton.SetSize(60, 20);
	Multiplayerform.add(&Multiplayerform_BackButton);

	// Connect Button
	Multiplayerform_ConnectButton.Caption = "Connect";
	//Multiplayerform_BackButton.SetAlpha(0.8f);
	//Multiplayerform_ConnectButton.SetClick(&Back);
	Multiplayerform_ConnectButton.SetColor(CVector3(0.0f, 1.0f, 0.0f), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
	Multiplayerform_ConnectButton.SetImage("Interface\\Dummy.bmp", 3);
	Multiplayerform_ConnectButton.SetPosition(700, 560);
	Multiplayerform_ConnectButton.SetSize(90, 20);
	Multiplayerform.add(&Multiplayerform_ConnectButton);
}

void InitSettingsform()
{
	// Settingsform
	Settingsform.SetScreen(GUI.mScreen);
	Settingsform.Caption = "Settingsform";
	Settingsform.Name = "Settingsform";
	Settingsform.SetImage("Screen_Menu.bmp", 1);
	Settingsform.SetPosition(0.0f, 0.0f);
	Settingsform.SetSize(GUI.mScreen->Width, GUI.mScreen->Height);
	GUI.add(&Settingsform);
}

void InitUserInterface()
{
	GUI.setscreen(&Engine->Screen);

	InitGameform();
	InitMainform();
	InitMultiplayerform();
	InitModsform();
	InitSettingsform();

	GUI.popup(&Mainform);
}


void Back()
{
	GUI.popback(GUI.ActiveForm);
}

void Mainform_NewGameClick()
{
	 Game.New();
}

void Mainform_ModsClick()
{
	UI::CGraphicControl *control = GUI.find("Modsform");

	if(control)
		GUI.popup(control);
}

void Mainform_MultiplayerClick()
{
	UI::CGraphicControl *control = GUI.find("Multiplayerform");

	if(control)
		GUI.popup(control);
}

void Mainform_QuitClick()
{
	PostQuitMessage(0);
}

void Mainform_SettingsClick()
{
	UI::CGraphicControl *control = GUI.find("Settingsform");

	if(control)
		GUI.popup(control);
}