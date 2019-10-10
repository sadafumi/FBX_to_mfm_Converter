#pragma once
#include <string>
#include <stdio.h>

class tools
{
public:
	static void CatFilePath(char* Out, char* In);
	static void GetFileName(std::string* out, std::string* in);
	static void ChangeFileExtension(std::string* Out, std::string* In, std::string* Extension);
	static void ChangeFilePath(std::string* Out, char* In);
	static void Path_Editer(std::string* out_path, std::string* File_dir, std::string* Full_path, std::string* in_put);
	static void Check_Export_Folder(std::string* out_path, char* File_Path);
};

