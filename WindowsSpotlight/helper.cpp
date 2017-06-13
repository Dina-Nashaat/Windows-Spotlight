#include "stdafx.h"
#include "helper.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")			
wchar_t* spotlightDir = L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets\\";  //Source Directory
wchar_t* PicturesDir = L"\\Pictures\\Windows Spotlight\\";

wchar_t* getProfile(const char *env);
wchar_t* srcDir = getProfile("LOCALAPPDATA");
wchar_t* destDir = getProfile("USERPROFILE");

size_t addressBufferSize = 1024;

std::string cryptFile(std::wstring filename);

/**
	Gets a user's directory for any environment variable given.

	@param evironment variable
	@return wchar_t pointer of env variable's user directory on local machine
*/
wchar_t* getProfile(const char *env)
{
	wchar_t *userProfile = new wchar_t[1024];
	char* buf = NULL;

	size_t sz = 0;
	// Get User default System Drive 
	if (_dupenv_s(&buf, &sz, env) == 0 && buf != NULL)
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
std::vector<std::wstring> readFiles(wchar_t* directory)
{
	
	wchar_t* search = new wchar_t[1024];
	wcscpy_s(search, addressBufferSize, directory);
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
	wchar_t srcDirectory[1024];
	wcscpy_s(srcDirectory, addressBufferSize, srcDir);
	wcscat_s(srcDirectory, addressBufferSize, spotlightDir);

	wchar_t destDirectory[1024];
	wcscpy_s(destDirectory, addressBufferSize, destDir);
	wcscat_s(destDirectory, addressBufferSize, PicturesDir);

	std::vector<std::wstring> filenames;
	filenames = readFiles(srcDirectory);

	//Create a new Directory to copy pictures to in case the destination directory was not created
	if (CreateDirectory(destDirectory, NULL))
	{
		std::wcout << "Output Directory: " << destDirectory << std::endl;
		std::cout << "Directory created successfully." << std::endl;
	}

	//Retrieve all files in destination directory
	std::set<std::string> filteredSet = filterDuplicates(destDirectory);

	int i = 0;
	int copyCount = 0;
	for (auto item : filenames)
	{
		wchar_t srcname[1024] = L"";
		wchar_t newfile[1024] = L"";
		std::wstring newfilename = RenameFile(i);

		wcscat_s(srcname, addressBufferSize, srcDirectory);
		wcscat_s(srcname, addressBufferSize, item.c_str());

		wcscat_s(newfile, addressBufferSize, destDirectory);
		wcscat_s(newfile, addressBufferSize, newfilename.c_str());

		// Filter images copied to destination by Dimensions (1920 x 1080)
		std::string	fileHash = cryptFile(srcname);
		if (!filteredSet.insert(fileHash).second)
		{
			//std::cout << "File already exists ";
			//std::wcout << newfile << std::endl;
			i++;
			continue;
		}
		bool matchesFilter = FilterImage(srcname);
		
		if (matchesFilter){
			bool b = CopyFile(srcname, newfile, FALSE);
			if (!b)
				std::wcout << "Error: " << GetLastError() << "for " << item << std::endl;
			else
				copyCount++;
		}
		i++;
	}
	std::wcout << copyCount << " Image(s) copied to destination output: " << destDirectory << std::endl;
}

/**
Check for duplicate images
*/
std::set<std::string> filterDuplicates(wchar_t destFolder[1024])
{
	//Get all files in destination folder
	std::vector<std::wstring> filenames = readFiles(destFolder);

	std::set<std::string> hashSet;
	std::string fileHash;
	wchar_t file_dest[1024];
	
	//Hash each file in the directory
	for (auto file : filenames)
	{
		wcscpy_s(file_dest, addressBufferSize, destFolder);
		wcscat_s(file_dest, addressBufferSize, file.c_str());
		
		fileHash = cryptFile(file_dest);
		
		if (fileHash.empty())
			continue;
		
		if (!hashSet.insert(fileHash).second) //If the value was not stored because it already exists
		{
			std::cout << "Duplicate Detected for file: ";
			std::wcout << file << std::endl;
			LPCWSTR file_delete_path = file_dest;
			DeleteFile(file_delete_path);
		}
	}
	return hashSet;
}

/**
Crypt any given file to md5 string

@param file path
@return a string of hashed file
*/
std::string cryptFile(std::wstring filepath)
{
	HANDLE hFile = NULL;
	hFile = CreateFile(filepath.c_str(),                // Filename to Read
		GENERIC_WRITE | GENERIC_READ,					// Open for Read and Write
		NULL,								            // Do not share
		NULL,											// default security
		OPEN_EXISTING,									// Open Existing File Only
		FILE_ATTRIBUTE_NORMAL,							// Normal file
		NULL);											// no attr. template

	//Check if file is valid handle
	DWORD dwStatus = 0;
	if (INVALID_HANDLE_VALUE == hFile){
		dwStatus = GetLastError();
		//printf("File Open Error %d ", dwStatus);
	}

	HCRYPTPROV hProv = 0;
	if (!CryptAcquireContext(&hProv,					//A pointer to a handle of a CSP		
		NULL,											//NULL: The application has no access to the persisted private keys of public/private key pairs. 
		NULL,											//Name of the CSP to be used (Default)
		PROV_RSA_FULL,									//Type of provider to acquire.
		CRYPT_VERIFYCONTEXT))							//do not require access to persisted private keys
	{
		dwStatus = GetLastError();
		//printf("CryptAcquireContext Error %d", dwStatus);
	}


	//Initiate has stream of data
	HCRYPTHASH hHash = 0;
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		//printf("CryptAcquireContext Error: %d\n", dwStatus);
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return "";
	}

	BOOL bResult = FALSE;
	BYTE rgbFile[1024];
	DWORD cbRead = 0;
	while (bResult = ReadFile(hFile, rgbFile, 1024,
		&cbRead, NULL)){
		if (0 == cbRead)
			break;

		if (!CryptHashData(hHash, rgbFile, cbRead, 0))
		{
			dwStatus = GetLastError();
			//printf("CryptHashData Error: %d\n", dwStatus);
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CloseHandle(hFile);
			return "";
		}
	}

	if (!bResult)
	{
		dwStatus = GetLastError();
		//printf("ReadFile failed: %d\n", dwStatus);
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return "";
	}
	
	DWORD cbHash = 0;
	const int hashLen = 16;
	cbHash = hashLen;					//MD5 Length
	BYTE rgbHash[hashLen];
	CHAR rgbDigits[] = "0123456789abcdef";
	CHAR hash;
	std::string hashValue;
	int j = 0;
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		for (DWORD i = 0; i < cbHash; i++)
		{
			hash = rgbDigits[rgbHash[i] >> 4];
			hashValue += hash;

			hash = rgbDigits[rgbHash[i] & 0xf];
			hashValue += hash;
		}
	}
	else
	{
		dwStatus = GetLastError();
		//printf("CryptGetHashParam Error: %d\n", dwStatus);
		return "";
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return hashValue;
}