#include "stdafx.h"
#include "helper.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")			
wchar_t* spotlightDir = L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets\\";  //Source Directory
wchar_t* PicturesDir = L"\\Pictures\\Wins10\\";


wchar_t* getProfile(const char *env);
wchar_t* srcDir = getProfile("LOCALAPPDATA");
wchar_t* destDir = getProfile("USERPROFILE");

size_t addressBufferSize = 1024;


/**
	Gets a user's directory for any environment variable given.

	@param evironment variable
	@return wchar_t pointer of env variable's user directory on local machine
*/
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

		wcscpy_s(userProfile, addressBufferSize, wbuf);							//Copy to srcDirectory

		free(buf);
	}
	return userProfile;
}

/**
	Read all file names from windows spotlight source directory.

	@return vector of strings of file names
*/
std::vector<std::wstring> readFiles()
{
	
	wcscat_s(srcDir, addressBufferSize, spotlightDir);				// Get path of Windows Spotlight Images
	//wcout << srcDir << endl;

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
	return filenames;
}

/**
	Filter images according to specified width and height. Width and Height are 
	specified in helper.h

	@param  Image Source name
	@return true if the width and height match, false otherwise
*/
bool FilterImage(wchar_t srcname[1024])
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Image* image = new Image(srcname);
	UINT imageHeight = image->GetHeight();
	UINT imageWidth = image->GetWidth();
	delete image;
	GdiplusShutdown(gdiplusToken);
	return (imageHeight == desiredHeight && imageWidth == desiredWidth) ? true : false;
}

/**
	Rename source filename to new name specified by current date and iteration number

	@param iteration number of current file
	@return wstring of the new filename
*/

std::wstring RenameFile(int i)
{
	//Get current local time to rename copied files
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	//Define Name with daymonthyear
	std::string dd = std::to_string(ltm.tm_mday);
	std::string mm = std::to_string(1 + ltm.tm_mon);
	std::string yyyy = std::to_string(1900 + ltm.tm_year);

	std::string str = "";
	std::wstring widestr = L"";
	
	str.append(dd).append(mm).append(yyyy).append(std::to_string(i)).append(".jpg");
	widestr = std::wstring(str.begin(), str.end());

	return widestr;
}

/**
	Driver Function that copies all files from source to destination folder
*/

void copyFiles()
{
	std::vector<std::wstring> filenames;
	filenames = readFiles();

	wcscat_s(destDir, addressBufferSize, PicturesDir);
	//Create a new Directory to copy pictures to in case the destination directory was not created

	
	
	if (CreateDirectory(destDir, NULL))
	{
		std::wcout << "Output Directory: " << destDir << std::endl;
		std::cout << "Directory created successfully." << std::endl;
	}

	FILE *stream;

	int i = 0;
	for (auto item : filenames)
	{
		wchar_t srcname[1024] = L"";
		wchar_t newfile[1024] = L"";
		std::wstring newfilename = RenameFile(i);

		wcscat_s(srcname, addressBufferSize, srcDir);
		wcscat_s(srcname, addressBufferSize, item.c_str());

		wcscat_s(newfile, addressBufferSize, destDir);
		wcscat_s(newfile, addressBufferSize, newfilename.c_str());

		// Filter images copied to destination by Dimensions (1920 x 1080)
		bool matchesFilter = FilterImage(srcname);

		if (matchesFilter){
			bool b = CopyFile(srcname, newfile, FALSE);
			if (!b)
				std::wcout << "Error: " << GetLastError() << "for " << item << std::endl;
		}
		i++;
	}
	std::wcout << "Images Successfully copied to destination output: " << destDir << std::endl;
}