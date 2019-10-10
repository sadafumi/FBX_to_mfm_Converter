#include "tools.h"
#include <Windows.h>
#include <direct.h>

void tools::CatFilePath(char* Out, char* In)
{
	char drive[MAX_PATH + 1]
		, dir[MAX_PATH + 1]
		, fname[MAX_PATH + 1]
		, ext[MAX_PATH + 1];

	_splitpath(In, drive, dir, fname, ext);//パス名を構成要素に分解します
	std::string drive_cahr(drive);
	std::string dir_cahr(dir);
	std::string out;
	out = drive_cahr + dir_cahr;
	memcpy(Out, out.data(), sizeof(char) * MAX_PATH);
}

void tools::GetFileName(std::string* out, std::string* in)
{
	char drive[MAX_PATH + 1]
		, dir[MAX_PATH + 1]
		, fname[MAX_PATH + 1]
		, ext[MAX_PATH + 1];

	_splitpath(in->data(), drive, dir, fname, ext);//パス名を構成要素に分解します
	std::string name_cahr(fname);
	std::string extcahr(ext);
	out->append(name_cahr + extcahr);
}

void tools::ChangeFileExtension(std::string* Out, std::string* In, std::string* Extension)
{
	int PeriodPos = 0;
	std::string buff(*In);
	PeriodPos = In->find_last_of(".");
	buff.replace(PeriodPos +1, Extension->size(), Extension->data());
	Out->append(buff);
}

void tools::ChangeFilePath(std::string* Out, char* In)
{
	int CharCon = 0;
	int HitPos = 0;
	char Buff[MAX_PATH];
	ZeroMemory(&Buff, sizeof(char) * MAX_PATH);
	strcpy(Buff, In);
	for (size_t i = 0; i < MAX_PATH; i++)
	{
		if (In[i] == '\0')
		{
			Buff[i] = In[i];
			i = MAX_PATH;
		}
		else
		{
			Buff[i] = In[i];
		}
	}

	for (int i = MAX_PATH; i > 0; i--)
	{
		if (Buff[i] == '\\')
		{
			HitPos = i + 1;
			i = 0;
			CharCon += 1;
		}
		else
		{
			CharCon++;
		}
	}
	Out->append(Buff);
	for (size_t i = 0; i < (size_t)CharCon; i++)
	{
		Out[i] = Buff[HitPos + i];
		if (Buff[HitPos + i] == '\0')
		{
			i = MAX_PATH;
		}
	}
}

void tools::Path_Editer(std::string* out_path, std::string* File_dir, std::string* Full_path, std::string* in_put)
{
	std::string out_put_address;
	std::string file_name;

	ChangeFileExtension(&out_put_address, Full_path, in_put);
	GetFileName(&file_name, &out_put_address);
	std::string out_File_name(file_name);
	std::string buff("\\");

	out_path->append(File_dir->data() + buff + out_File_name);
}

void tools::Check_Export_Folder(std::string* out_path, char* File_Path)
{
	std::string out_dir(File_Path);
	std::string out_Fileder_name("Exit_Model");
	std::string out_File_dir(out_dir + out_Fileder_name);
	FILE* fp;
	fopen_s(&fp, out_File_dir.data(), "r");
	if (fp == NULL)
	{
		_mkdir(out_File_dir.data());
	}
	else
	{
		fclose(fp);
	}
	out_path->append(out_File_dir);
}
