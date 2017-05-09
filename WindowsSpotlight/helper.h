#pragma once

#include <Shlobj.h>
#include <sstream>
#include <ctime>
#include <iostream>
#include <vector>

#define desiredWidth 1920
#define desiredHeight 1080


wchar_t* getProfile(const char *env);
std::vector<std::wstring> readFiles();
void copyFiles(std::vector<std::wstring> filenames);
