#include "stdafx.h"
#include "helper.h"

using namespace std;
/* Returns the source directory of windows spotlight images */
wchar_t* spotlightDir = L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets\\";
wchar_t* PicturesDir = L"\\Pictures";
size_t addressBufferSize = 1024;

wchar_t* getProfile()
{
	wchar_t *userProfile = new wchar_t[1024];
	char* buf = nullptr;

	size_t sz = 0;
	// Get User default System Drive 
	if (_dupenv_s(&buf, &sz, "LOCALAPPDATA") == 0 && buf != nullptr)
	{
		size_t newsize = strlen(buf) + 1;
		wchar_t* wbuf = new wchar_t[newsize];
		size_t convertedChars = 0;

		mbstowcs_s(&convertedChars, wbuf, newsize, buf, _TRUNCATE); //Convert char* to wchar_t*
		
		wcscpy_s(userProfile,1024,wbuf);							//Copy to srcDirectory
		
		free(buf);
	}
	return userProfile;
}
void readFiles()
{
	wchar_t* srcDir = getProfile();
	wcscat_s(srcDir, addressBufferSize, spotlightDir);				// Get path of Windows Spotlight Images
	wcout << srcDir << endl;
	wchar_t* destDir = getProfile();								// Get Path of User Pictures Folder
	wcscat_s(destDir, addressBufferSize, PicturesDir);				// Get path of Windows Spotlight Images
	wcout << destDir << endl;
}