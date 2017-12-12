#ifndef _FILEMANAGER_H
#define _FILEMANAGER_H


#include "Main.h"


class CFileManager
{
public:
	int Searchfiles(char *path, char *ext);

	vector<string> filelist;
};


#endif