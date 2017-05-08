// WindowsSpotlight.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "helper.h"


int main()
{
	std::vector<std::wstring> filenames;
	
	filenames = readFiles();

	copyFiles(filenames);

	system("pause");

	return 0;		
}

