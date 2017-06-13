#pragma once


#include <Shlobj.h>
#include <sstream>
#include <ctime>
#include <iostream>
#include <vector>
#include <set>

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <Wincrypt.h>

#define desiredWidth 1920
#define desiredHeight 1080

void copyFiles();
std::set<std::string> filterDuplicates(wchar_t destFolder[1024]);
