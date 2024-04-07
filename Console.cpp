
// Library includes
#include <stdio.h>
#include <cstring>
#include <Windows.h>

// Project includes
#include "Console.h"
#include "Tools.h"


CConsole::CConsole()
{
	bLogging = true;
	bVisible = false;
	sprintf(AutocompleteCmd, "");
	sprintf(cCommand, "");
	sprintf(cLastCommand, "");
	sprintf(cLogFile, "Atlas.log");
	for(int y = 0; y < MAX_LINES; y += 1)
	{
		sprintf(cOutput[y], "");
		for(int x = 0; x < MAX_COLUMNS; x += 1)
			sprintf(&cOutputGraphic[y][x], " ");
	}
	fBetweenLines = 0.06f;
	iAutocompleteIndex = 0;
	iCursorX = 0;
	iCursorY = 0;
	iOutputMode = 0;
	iVisibleLines = MAX_LINES;

	Logging_Start();
}

CConsole::~CConsole()
{
	Logging_Stop();

	bLogging = false;
	bVisible = false;
	sprintf(AutocompleteCmd, "");
	sprintf(cCommand, "");
	sprintf(cLastCommand, "");
	sprintf(cLogFile, "Atlas.log");
	for(int y = 0; y < MAX_LINES; y += 1)
	{
		sprintf(cOutput[y], "");
		for(int x = 0; x < MAX_COLUMNS; x += 1)
			sprintf(&cOutputGraphic[y][x], " ");
	}
	fBetweenLines = 0.06f;
	iAutocompleteIndex = 0;
	iCursorX = 0;
	iCursorY = 0;
	iOutputMode = 0;
	iVisibleLines = MAX_LINES;
}

void CConsole::Autocomplete()
{
	FILE *pFile;
	char oneline[255];

	if(strlen(cCommand) <= 0)
		return;

	if(strlen(AutocompleteCmd) == 0)
		sprintf(AutocompleteCmd, "%s", cCommand);

	sprintf(oneline, "%sHelp\\Autocomplete.txt", g_sGameName);
	fopen_s(&pFile, oneline, "rt");
	if(!pFile)
	{
		return;
	}

	int c = strlen(AutocompleteCmd);
	char buffer[MAX_COLUMNS] = "";

	for(int i = 0; i < iAutocompleteIndex; i += 1)
	{
		sprintf(oneline, "");
		readstr(pFile, oneline);
	}

	do
	{
		iAutocompleteIndex++;

		sprintf(oneline, "");
		readstr(pFile, oneline);

		sprintf(buffer, "%s", oneline);

		buffer[c] = '\0';
		if(strcmp(buffer, AutocompleteCmd) == 0)
		{
			sprintf(cCommand, "%s", oneline);
			break;
		}
	} while(strcmp(oneline, "") != 0);

	fclose(pFile);
}

void CConsole::Clear()
{
	sprintf(AutocompleteCmd, "");
	sprintf(cCommand, "");
	sprintf(cLastCommand, "");
	for(int y = 0; y < MAX_LINES; y += 1)
	{
		sprintf(cOutput[y], "");
		for(int x = 0; x < MAX_COLUMNS; x += 1)
			sprintf(&cOutputGraphic[y][x], " ");
	}
	iAutocompleteIndex = 0;
	iCursorX = 0;
	iCursorY = 0;
}

int CConsole::getOutputMode()
{
	return iOutputMode;
}

bool CConsole::IsVisible() const
{
	return bVisible;
}

void CConsole::KeyPressed(unsigned int Key)
{
	switch(Key)
	{
	//case VK_ADD:
	//	break;
	case VK_BACK:
		{
			if(strlen(cCommand) <= 0)
				break;

			char buffer[MAX_COLUMNS];
			sprintf(buffer, "");

			for(unsigned int i = 0; i < strlen(cCommand) - 1; i += 1)
			{
				sprintf(buffer, "%s%c", buffer, cCommand[i]);
			}

			sprintf(cCommand, "%s", buffer);
		}
		break;
	//case VK_DOWN:
	//	{
		//char buffer[MAX_COLUMNS] = "";
		//sprintf(buffer, "%s", cLastCommand);
		//sprintf(cLastCommand, "%s", cCommand);
		//sprintf(cCommand, "%s", buffer);
	//	}
		break;
	case VK_ESCAPE:
		sprintf(AutocompleteCmd, "");
		sprintf(cCommand, "");
		iAutocompleteIndex = 0;
		break;
	case VK_F7:
		sprintf(cCommand, "MAP.LOAD ");
		break;
	case VK_F8:
		sprintf(cCommand, "MAP.SAVE ");
		break;
	case VK_F9:
		bVisible = false;
		break;
	case VK_RETURN:
		ProcessCommand(cCommand);

		if(strlen(cCommand) != 0)
		{
			sprintf(cLastCommand, "%s", cCommand);
			sprintf(cCommand, "");
		}

		sprintf(AutocompleteCmd, "");
		iAutocompleteIndex = 0;
		break;
	//case VK_SUBTRACT:
	//	break;
	case VK_TAB:
		Autocomplete();
		break;
	case VK_UP:
		{
			char buffer[80] = "";
			sprintf(buffer, "%s", cCommand);
			sprintf(cCommand, "%s", cLastCommand);
			sprintf(cLastCommand, "%s", buffer);
		}
		break;
	case 188:	// ,
		if(strlen(cCommand) >= MAX_COLUMNS)
			break;

		sprintf(cCommand, "%s,", cCommand);
		break;
	case 189:	// -
		if(strlen(cCommand) >= MAX_COLUMNS)
			break;

		sprintf(cCommand, "%s-", cCommand);
		break;
	case 190:	// .
		if(strlen(cCommand) >= MAX_COLUMNS)
			break;

		sprintf(cCommand, "%s.", cCommand);
		break;
	default:
		if(strlen(cCommand) >= MAX_COLUMNS)
			break;

		sprintf(cCommand, "%s%c", cCommand, Key);
		break;
	}

	if(Key != VK_TAB)
	{
		sprintf(AutocompleteCmd, "%s", cCommand);
		iAutocompleteIndex = 0;
	}
}

bool CConsole::Logging_Start()
{
	FILELogFile = NULL;
	FILELogFile = fopen(cLogFile, "w+t");

	if ( !FILELogFile ) {
		Output("ERROR[Console->Clear]: Could not open log file!\n");
		return false;
	}
	
	bLogging = true;
	return true;
}

void CConsole::Logging_Stop()
{
	fclose(FILELogFile);

	bLogging = false;
}

void CConsole::Output(char *text, ...)
{
	char	strText[255];
	va_list	argumentPtr;								// This will hold the pointer to the argument list

	va_start(argumentPtr, text);						// Parse the arguments out of the string

	vsprintf(strText, text, argumentPtr);				// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	if(strlen(strText) > MAX_COLUMNS)
		strText[MAX_COLUMNS] = '\0';

	for(unsigned int i = 1; i < MAX_LINES; i += 1)
	{
		strcpy(cOutput[i - 1], cOutput[i]);
	}

	sprintf(cOutput[MAX_LINES - 1], "%s", strText);

	if ( bLogging ) {
		if ( !FILELogFile ) {
			Logging_Start();
		}

		fprintf(FILELogFile, cOutput[MAX_LINES - 1]);
	}
}

void CConsole::OutputGraphic(char *cText, int X, int Y)
{
	if(X < 0 || X > MAX_COLUMNS || Y < 0 || Y > MAX_LINES)
		return;
	if(X + strlen(cText) > MAX_COLUMNS)
		cText[MAX_COLUMNS - X] = '\0';

	for(unsigned int i = X; i < X + strlen(cText); i += 1)
		cOutputGraphic[Y][i] = cText[i - X];

	if(bLogging)
	{
		if(FILELogFile == NULL)
			Logging_Start();

		fprintf(FILELogFile, cOutput[MAX_LINES - 1]);
	}
}

void CConsole::Render()
{
	if(!bVisible)
		return;

	//if(!Font.Initialize(g_hDC, 18, "Arial"))
	//	return;

	glColor3f(1.0f, 1.0f, 1.0f);

	for(int i = 0; i < MAX_LINES; i += 1)
	{
		switch(iOutputMode)
		{
		case 0:
			glPrintText(-0.96f, 0.92f - i * fBetweenLines, cOutput[i]);
			break;
		case 1:
			glPrintText(-0.96f, 0.92f - i * fBetweenLines, cOutputGraphic[i]);
			break;
		}
	}

	if(strlen(cCommand) > MAX_COLUMNS)
		cCommand[MAX_COLUMNS] = '\0';

	glPrintText(-0.96f, 0.92f - MAX_LINES * fBetweenLines, "%s_", cCommand);

	//if(!Font.Initialize(g_hDC, 25, "Arial"))
	//	return;
}

void CConsole::setOutputMode(int mode)
{
	iOutputMode = mode;
}

void CConsole::setVisible(bool toogle)
{
	bVisible = toogle;
}
