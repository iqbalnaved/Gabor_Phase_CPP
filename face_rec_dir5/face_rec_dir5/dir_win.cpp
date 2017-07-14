#include "facerec.h"


// Set Solution Properties->Configuration properties->General->Character Set = MultiByte Characters Set
// to use this function.
bool ListDirectoryContents(const char *sDir, vector<string> *file_list)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char sPath[2048];
	// Specify afile mask. *.* = We want everything!
	sprintf(sPath, "%s\\*.*", sDir);
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", sDir);
		return false;
	}
	do
	{
		// Find first file will always return "." and ".." the first two directories.
		if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
		{
			// Build up our file path using the passed in [sDir] and the file/foldername 
			// we just found:
			sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
			//Is the entity a File or Folder?
			if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("Directory:%s\n", sPath);
				//ListDirectoryContents(sPath); // Recursive call for listing subdirectories
			}
			else
			{
				//wprintf(L"File:%s\n", sPath);
				//wprintf(L"File:%s\n", fdFile.cFileName); // print file names only
				file_list->push_back(sPath);				
			}
		}
	} while(FindNextFile(hFind, &fdFile)); // find the next file.
	
	FindClose(hFind); 
	return true;
}