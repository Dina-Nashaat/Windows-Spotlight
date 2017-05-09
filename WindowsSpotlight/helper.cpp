


#include "stdafx.h"
#include "helper.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


/* Returns the source directory of windows spotlight images */
wchar_t* spotlightDir = L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets\\";
wchar_t* PicturesDir = L"\\Pictures\\Wins10\\";

wchar_t* srcDir = getProfile("LOCALAPPDATA");
wchar_t* destDir = getProfile("USERPROFILE");
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


std::vector<std::wstring> readFiles()
{

	wcscat_s(srcDir, addressBufferSize, spotlightDir);				// Get path of Windows Spotlight Images
	//wcout << srcDir << endl;

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
	return filenames;
}


void copyFiles(std::vector<std::wstring> filenames) 
{
	//Create a new Directory to copy pictures to in case the destination directory was not created
	if (CreateDirectory(destDir, NULL))
	{
		std::wcout << "Output Directory: " << destDir << std::endl;
		std::cout << "Directory created successfully." << std::endl;
	}

	//Get current local time to rename copied files
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm,&now);
	std::string dd = std::to_string(ltm.tm_mday);
	std::string mm = std::to_string(1 + ltm.tm_mon);
	std::string yyyy = std::to_string(1900 + ltm.tm_year);

	FILE *stream;

	int i = 0;
	for (auto item : filenames)
	{
		std::string str;
		std::wstring widestr;
		wchar_t newfile[1024] = L"";
		wchar_t srcname[1024] = L"";
		str.append(dd).append(mm).append(yyyy).append(std::to_string(i)).append(".jpg");
		widestr = std::wstring(str.begin(), str.end());

		wcscat_s(srcname, addressBufferSize, srcDir);
		wcscat_s(srcname, addressBufferSize, item.c_str());

		wcscat_s(newfile, addressBufferSize, destDir);
		wcscat_s(newfile, addressBufferSize, widestr.c_str());


		// Filter images copied to destination by Dimensions (1920 x 1080)
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		Image* image = new Image(srcname);
		UINT imageHeight = image->GetHeight();
		UINT imageWidth = image->GetWidth();
		
		delete image;
		GdiplusShutdown(gdiplusToken);

		if (imageHeight == desiredHeight && imageWidth == desiredWidth)
		{
			bool b = CopyFile(srcname, newfile, FALSE);
			if (!b)
				std::wcout << "Error: " << GetLastError() << "for " << item << std::endl;
		}
		i++;
	}
	std::wcout << "Images Successfully copied to destination output: " << destDir << std::endl;
}