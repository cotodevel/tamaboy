#include "winDir.h"

//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#ifdef _WIN32
#include <windows.h> // WinApi header
#include <tchar.h> 
#include <strsafe.h>
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <functional>   // std::greater
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#ifndef _MSC_VER
#include <dirent.h>
#endif
#include <vector>
#include <limits.h>
using namespace std; // std::cout, std::cin

/*
C++ example:
string curDirPath = "/";
std::vector<std::string> files = list_directory(curDirPath);
cout<<"dir contents: " << "(" << files.size() << ") \n";
int i = 0;
for(i = 0; i < files.size(); i++){
cout << files.at(i) << "\n";
}   
*/

std::vector<std::string> list_directory(const std::string &directory)
{
    std::vector<std::string> dir_list;
    #ifdef _MSC_VER
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    hFind = FindFirstFileA(full_path.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Invalid handle value! Please check your path...");

    while (FindNextFileA(hFind, &findData) != 0)
    {
        dir_list.push_back(std::string(findData.cFileName));
    }
    FindClose(hFind);
    #endif
    
    #ifndef _MSC_VER
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL) { //""c:\\src\\""
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        //printf ("%s\n", ent->d_name);
        dir_list.push_back(std::string(ent->d_name));
      }
      closedir (dir);
    } else {
      /* could not open directory */
      //printf ("dir fail");
    }
    #endif
    return dir_list;
}


#ifndef _MSC_VER
#include <dirent.h>
#endif
std::vector<dirItem> list_directoryByType(const std::string &directory){
    std::vector<dirItem> dir_list;
    #ifdef _MSC_VER
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    hFind = FindFirstFileA(full_path.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE){
        throw std::runtime_error("Invalid handle value! Please check your path...");
	}
    while (FindNextFileA(hFind, &findData) != 0){
		int objType = 0;
		std::string fileDirName;
		//dir
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0){
			objType = FT_DIR;
			if(std::string(findData.cFileName) == ".."){
				fileDirName = std::string(findData.cFileName);
			}
			else{
				fileDirName = std::string("/") + std::string(findData.cFileName);
			}
		}
		//file
		else if ((findData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0){
			objType = FT_FILE;
			fileDirName = std::string(findData.cFileName);
		}
		else{
			objType = FT_NONE;
			fileDirName = std::string("");
		}
		dirItem item(fileDirName, objType);
		dir_list.push_back(item);
    }
    FindClose(hFind);
    #endif
    
    #ifndef _MSC_VER
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL) { //""c:\\src\\""
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        //printf ("%s\n", ent->d_name);
        dir_list.push_back(std::string(ent->d_name));
      }
      closedir (dir);
    } else {
      /* could not open directory */
      //printf ("dir fail");
    }
    #endif
    return dir_list;
}


std::string first_numberstring(std::string const & str)
{
    char const* digits = "0123456789";
    std::size_t const n = str.find_first_of(digits);
    if (n != std::string::npos){
		std::size_t const m = str.find_first_not_of(digits, n);
		return str.substr(n, m != std::string::npos ? m-n : m);
    }
    return std::string("-1"); //no number found in the word search. Since this is a numerical context we return, treat it as the highest integer result (preceding 0)
}

bool compareFunction (std::string a, std::string b) {
	string aDest = getFileName(a, false);
	string bDest = getFileName(b, false);
	return std::stoi(first_numberstring(aDest))<std::stoi(first_numberstring(bDest));
}

std::vector<std::string> findFiles(const std::string &directory, const std::string &extension){
	std::vector<std::string> filesFound = list_directory(directory);
	std::vector<std::string> filesFoundByExtension;
	for(vector<string>::const_iterator i = filesFound.begin(); i != filesFound.end(); ++i) {
		std::string file = string(*i);
		if(file.substr(file.find_last_of(".") + 1) == extension){
			filesFoundByExtension.push_back(string(directory + file));	
		}
	}
	std::sort(filesFoundByExtension.begin(),filesFoundByExtension.end(),compareFunction); //sort by lowest to highest number in filename
	return filesFoundByExtension;
}

std::string getFileName(std::string filePath, bool withExtension)
{
	char seperator = '\\';
    // Get last dot position
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.rfind(seperator);
    if(sepPos != std::string::npos)
    {
        return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos) );
    }
    return "";
}

std::string getFileNameNoExtension(std::string filename){
	size_t lastindex = filename.find_last_of(".");
	string rawname = filename.substr(0, lastindex);
	return rawname;
}

void getCWDWin(char * outPath, char* pathToNavigate){
	#ifdef _MSC_VER
	//Output Directory
	LPWSTR Buffer[MAX_PATH];
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH, LPSTR(Buffer));
	char converted[MAX_PATH];
	wcstombs(converted, LPWSTR(Buffer), wcslen(LPWSTR(Buffer)) + 1);
	char outputFullPath[256+1];
	strcpy(outputFullPath, (string(converted) + string(pathToNavigate)).c_str() );
	strcpy(outPath, outputFullPath);
	#endif
	
	#ifndef _MSC_VER
	char cwd[256];
	strcpy(cwd, pathToNavigate);
	getcwd(cwd, strlen(cwd)+1);
	strcpy(outPath, cwd);
	#endif
}
