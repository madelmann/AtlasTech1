#include "Main.h"
#include <io.h>
#include <vector>
#include "FileManager.h"


int CFileManager::Searchfiles(char *path, char *ext)
{

	filelist.clear();

	char buffer[100];

	sprintf(buffer, "%s\\*.%s", path, ext);

	//char **result = NULL;

    struct _finddata_t file;
	long hFile;

	/* Find first file in current directory */
	hFile = _findfirst(buffer, &file);
	if( hFile == -1L )
		return -1;
	else
		filelist.push_back(file.name);
	
	/* Find the rest of the files */
	while( _findnext( hFile, &file ) == 0 )
	{
		filelist.push_back(file.name);
	}

	_findclose( hFile );

	return 0;

}