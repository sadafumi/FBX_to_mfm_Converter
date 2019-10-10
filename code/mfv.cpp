#include "mfv.h"
#include <Windows.h>

bool mfv::model::Save_File(std::string file_name)
{
	//�t�@�C�����m��
	FILE* fp = NULL;
	if ((fp = fopen(file_name.data(), "wb")) == NULL)
	{
		printf("�t�@�C��(%s)���J���܂���ł���\n", file_name.data());
		return true;
	}

	//�^�̌`����������
	fwrite(&this->stride_size, sizeof(int), 1, fp);
	int stridesize = (int)this->strides.size();
	fwrite(&stridesize, sizeof(int), 1, fp);
	fwrite(&this->strides[0], sizeof(int), stridesize, fp);

	//Mesh����������
	int mesh_size = (int)this->meshs.size();
	fwrite(&mesh_size, sizeof(int), 1, fp);
	for (size_t i = 0; i < (size_t)mesh_size; i++)
	{
		int vertexsize = this->meshs[i].vertex_size;
		fwrite(&vertexsize, sizeof(int), 1, fp);
		fwrite(&this->meshs[i].vertex_arry[0], sizeof(Vertex), vertexsize, fp);
		int vertex_arrey_size = (int)this->meshs[i].vertexs.size();
		fwrite(&vertex_arrey_size, sizeof(int), 1, fp);
		fwrite(this->meshs[i].vertexs.data(), sizeof(float), vertex_arrey_size, fp);
		fwrite(this->meshs[i].tex_file.name.data(), sizeof(char), MAX_PATH, fp);
	}
	fclose(fp);
	return false;
}

bool mfv::model::Load_File(std::string file_name)
{
	//�t�@�C���̓ǂݍ���
	FILE* fp = NULL;
	if ((fp = fopen(file_name.data(), "rb")) == NULL)
	{
		printf("�t�@�C��(%s)���J���܂���ł���\n", file_name.data());
		return true;
	}

	//�^��ǂݍ���
	fread(&this->stride_size, sizeof(int), 1, fp);
	int stridesize;
	fread(&stridesize, sizeof(int), 1, fp);
	this->strides.resize(stridesize);
	fread(&this->strides[0], sizeof(int), stridesize, fp);

	//Mesh��ǂݍ���
	int mesh_size;
	fread(&mesh_size, sizeof(int), 1, fp);
	this->meshs.resize(mesh_size);
	for (int i = 0; i < mesh_size; i++)
	{
		int vertexsize;//���_�T�C�Y��
		fread(&vertexsize, sizeof(int), 1, fp);
		this->meshs[i].vertex_size = vertexsize;
		this->meshs[i].vertex_arry.resize(vertexsize);

		fread(&this->meshs[i].vertex_arry[0], sizeof(Vertex), vertexsize, fp);
		fread(&vertexsize, sizeof(int), 1, fp);
		this->meshs[i].vertexs.resize(vertexsize);
		fread(this->meshs[i].vertexs.data(), sizeof(float), this->meshs[i].vertexs.size(), fp);

		char* name = new char[MAX_PATH];
		fread(name, sizeof(char), MAX_PATH, fp);
		this->meshs[i].tex_file.name.append(name);
		delete[] name;
	}
	fclose(fp);
	return false;
}
