#ifndef _CONSOLE_H
#define _CONSOLE_H


#include "Singleton.h"

#include "Main.h"
#include "Engine.h"


#define MAX_LINES 31
#define MAX_COLUMNS 80


class CConsole: public CSingleton< CConsole >
{
	friend class CSingleton< CConsole >;

public:
	CConsole();
	~CConsole();

	void Autocomplete();
	void Clear();
	int getOutputMode();
	bool IsVisible() const;
	void KeyPressed(WPARAM Key);
	bool Logging_Start();
	void Logging_Stop();
	void Output(char *text, ...);
	void OutputGraphic(char *cText, int X, int Y);
	void Render();
	void setOutputMode(int mode);
	void setVisible(bool toogle);

private:
	bool	bLogging;
	bool	bVisible;
	char	AutocompleteCmd[MAX_COLUMNS + 1];
	char	cCommand[MAX_COLUMNS + 1];
	char	cLastCommand[MAX_COLUMNS + 1];
	char	cLogFile[255];
	char	cOutput[MAX_LINES][MAX_COLUMNS + 1];
	char	cOutputGraphic[MAX_LINES][MAX_COLUMNS + 1];
	FILE	*FILELogFile;
	float	fBetweenLines;
	int		iAutocompleteIndex;
	int		iCursorX;
	int		iCursorY;
	int		iOutputMode;
	int		iVisibleLines;
};

#endif