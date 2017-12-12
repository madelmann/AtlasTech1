#ifndef _MISC_LOGGER_H_
#define _MISC_LOGGER_H_


// Memory leak check - Begin
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>

#include <iostream>
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

// Memory leak check - End


#include <assert.h>


static const char* MessageDebug = "[DBG ]";
static const char* MessageError = "[ERR ]";
static const char* MessageWarning = "[WARN]";

static const int LoudnessDebug = 0;
static const int LoudnessWarning = 1;
static const int LoudnessError = 2;

class Logger
{
public:
	Logger(char* filename);
	~Logger();

	void LogDebug(char* message, ...);
	void LogError(char* message, ...);
	void LogWarn (char* message, ...);

	void setLoudness(int loudness);

private:
	char*	mLogFile;
	FILE*	mFilePointer;
	int		mLoudness;
};


const int MaxInfoLength = 256;
const int MaxInfoDepth = 64;

static int g_infoStackDepth = 0;
static char g_infoStack[MaxInfoDepth][MaxInfoLength + 1];


void push_info_stack( const char * string )
{
    assert( g_infoStackDepth < MaxInfoDepth );
    strncpy( &g_infoStack[g_infoStackDepth][0], string, MaxInfoLength );
    g_infoStack[g_infoStackDepth][MaxInfoLength] = '\0';
    g_infoStackDepth++;
}

void pop_info_stack()
{
    assert( g_infoStackDepth > 0 );
    g_infoStack[g_infoStackDepth][0] = '\0';
    g_infoStackDepth--;
}

class InfoPushPopHelper
{
    InfoPushPopHelper( const char * string )
    {
        push_info_stack( string );
    }

    ~InfoPushPopHelper()
    {
        pop_info_stack();
    }
};


#define INFO( format, ... )                                     \
    char buffer[MaxInfoLength+1];                               \
    snprintf( buffer, MaxInfoLength+1, format, __VA_ARGS__ );   \
    InfoPushPopHelper infoPushPop( buffer );


#endif