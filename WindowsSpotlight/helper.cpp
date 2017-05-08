


#include "stdafx.h"
#include "helper.h"


/* Returns the source directory of windows spotlight images */
wchar_t* spotlightDir = L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets\\";
wchar_t* PicturesDir = L"\\Pictures\\Wins10";
size_t addressBufferSize = 1024;

wchar_t* getProfile(const char *env)
{
	wchar_t *userProfile = new wchar_t[1024];
	char* buf = nullptr;

	size_t sz = 0;
	// Get User default System Drive 
	if (_dupenv_s(&buf, &sz, env) == 0 && buf != nullptr)
	{
		size_t newsize = strlen(buf) + 1;
		wchar_t* wbuf = new wchar_t[newsize];
		size_t convertedChars = 0;

		mbstowcs_s(&convertedChars, wbuf, newsize, buf, _TRUNCATE); //Convert char* to wchar_t*
		
		wcscpy_s(userProfile,addressBufferSize,wbuf);							//Copy to srcDirectory
		
		free(buf);
	}
	return userProfile;
}

void readFiles()
{
	wchar_t* srcDir = getProfile("LOCALAPPDATA");
	wcscat_s(srcDir, addressBufferSize, spotlightDir);				// Get path of Windows Spotlight Images
	//wcout << srcDir << endl;

	wchar_t* destDir = getProfile("USERPROFILE");								// Get Path of User Pictures Folder
	wcscat_s(destDir, addressBufferSize, PicturesDir);				// Get path of Windows Spotlight Images
	//wcout << destDir << endl;

	//Read file by file and push names into filenames vector
	wchar_t* search = new wchar_t[1024];
	wcscpy_s(search, addressBufferSize, srcDir);
	wcscat_s(search, addressBufferSize, L"*");
	
	WIN32_FIND_DATA findFileData;
	int i = 0;

	std::vector<std::wstring> filenames;
	HANDLE hFind;
	hFind = FindFirstFile(search, &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			filenames.push_back(findFileData.cFileName);
		} while (FindNextFile(hFind, &findFileData));
		FindClose(hFind);
	}
	
	//Create a new Directory to store in case the destination directory was not given
	if (CreateDirectory(destDir, NULL))
	{
		std::wcout << "Output Directory: " << destDir << std::endl;
		std::cout << "Directory created successfully." << std::endl;
	}
		

}