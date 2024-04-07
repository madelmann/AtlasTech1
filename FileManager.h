
#pragma once

// Library includes
#include <string>
#include <vector>

// Project includes


class CFileManager
{
public:
	int Searchfiles(char *path, char *ext);

	std::vector<std::string> filelist;
};
