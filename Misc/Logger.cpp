#include "Logger.h"

#include <iostream>
#include <stdarg.h>


Logger::Logger(char *filename)
: mFilePointer(NULL),
  mLogFile(filename),
  mLoudness(0)
{
	fopen_s(&mFilePointer, mLogFile, "wt");
}

Logger::~Logger()
{
	fclose(mFilePointer);
}

void Logger::LogDebug(char* message, ...)
{
	if ( mLoudness > LoudnessDebug )
		return;

	char* output = "";

	va_list	argumentPtr;								// This will hold the pointer to the argument list

	va_start(argumentPtr, message);						// Parse the arguments out of the string

	vsprintf(output, message, argumentPtr);				// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	fprintf_s(mFilePointer, output);
}

void Logger::LogError(char* message, ...)
{
	if ( mLoudness > LoudnessError )
		return;

	char* output = "";

	va_list	argumentPtr;								// This will hold the pointer to the argument list

	va_start(argumentPtr, message);						// Parse the arguments out of the string

	vsprintf(output, message, argumentPtr);				// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	fprintf_s(mFilePointer, output);
}

void Logger::LogWarn(char* message, ...)
{
	if ( mLoudness > LoudnessWarning )
		return;

	char* output = "";

	va_list	argumentPtr;								// This will hold the pointer to the argument list

	va_start(argumentPtr, message);						// Parse the arguments out of the string

	vsprintf(output, message, argumentPtr);				// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	fprintf_s(mFilePointer, output);
}

void Logger::setLoudness(int loudness)
{
	mLoudness = loudness;
}