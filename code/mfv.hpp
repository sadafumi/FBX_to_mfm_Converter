#pragma once

#include <string>
#include <vector>
#include <stdio.h>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"

namespace mfv
{
	class model
	{
	public:
		enum Stride
		{
			Str_vertex,
			Str_color,
			Str_uv,
			Str_normal,
			Str_weight,
			STRIDE_MAX
		};
		struct Vertex
		{
			float Pos_x;
			float Pos_y;
			float Pos_z;
			float Pos_w;

			float Normal_x;
			float Normal_y;
			float Normal_z;

			float U;
			float V;
		};

	private:
		struct Weight
		{
			std::vector<float> weight;
			std::vector<unsigned int> index;
		};
		struct Animation
		{
			glm::mat4 init;
			std::vector<glm::mat4> perFrame;
		};
		struct Bone
		{
			std::vector<Weight> weights;
			std::vector<Animation> index;
		};
		struct material
		{
			std::string name;
		};
		struct mash
		{
			int vertex_size;
			std::vector<float> vertexs;
			std::vector<Vertex> vertex_arry;
			//Vertex* vertex_arry;
			material tex_file;
			std::vector<Bone> Bones;
		};
	public:
		int stride_size;
		std::vector<Stride> strides;
		std::vector<mash> meshs;
		bool Save_File(std::string file_name)
		{
			//ファイルを確保
			FILE* fp = NULL;
			if ((fp = fopen(file_name.data(), "wb")) == NULL)
			{
				printf("ファイル(%s)が開けませんでした\n", file_name.data());
				return true;
			}

			//型の形を書き込む
			fwrite(&this->stride_size, sizeof(int), 1, fp);
			int stridesize = (int)this->strides.size();
			fwrite(&stridesize, sizeof(int), 1, fp);
			fwrite(&this->strides[0], sizeof(int), stridesize, fp);

			//Meshを書き込み
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
		};
		bool Load_File(std::string file_name)
		{
			//ファイルの読み込み
			FILE* fp = NULL;
			if ((fp = fopen(file_name.data(), "rb")) == NULL)
			{
				printf("ファイル(%s)が開けませんでした\n", file_name.data());
				return true;
			}

			//型を読み込み
			fread(&this->stride_size, sizeof(int), 1, fp);
			int stridesize;
			fread(&stridesize, sizeof(int), 1, fp);
			this->strides.resize(stridesize);
			fread(&this->strides[0], sizeof(int), stridesize, fp);

			//Meshを読み込み
			int mesh_size;
			fread(&mesh_size, sizeof(int), 1, fp);
			this->meshs.resize(mesh_size);
			for (int i = 0; i < mesh_size; i++)
			{
				int vertexsize;
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
		};
	};
}

