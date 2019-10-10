#include "mfm.h"


namespace mfmlib
{
	//データをファイルと出しれする関数
	bool Model::FileSaveMeshData(const char * Filename)
	{
		FILE *fp = NULL;

		if ((fp = fopen(Filename, "wb")) == NULL)
		{
			printf("ファイル(%s)が開けませんでした\n", Filename);
			return true;
		}

		//Meshs数を渡す
		int mesh_size = (int)Meshs.size();
		fwrite(&mesh_size, sizeof(int), 1, fp);

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			WriteMesh(&Meshs[i], fp);
		}
		int material_size = (int)Material.size();
		fwrite(&material_size, sizeof(int), 1, fp);
		for (size_t i = 0; i < Material.size(); i++)
		{
			fwrite(this->Material[i].name.data(), sizeof(TCHAR), MAX_PATH, fp);
		}

		fclose(fp);
		return false;
	}
	bool Model::FileLoadMeshData(const char* Filename)
	{
		FILE *fp = NULL;

		if ((fp = fopen(Filename, "rb")) == NULL)
		{
			printf("ファイルが開けませんでした\n");
			return true;
		}
		//Meshs数を受け取る
		int mesh_size;
		fread(&mesh_size, sizeof(int), 1, fp);
		Meshs.resize(mesh_size);

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			ReadMesh(&Meshs[i], fp);
		}
		int material_size;
		fread(&material_size, sizeof(int), 1, fp);
		Material.resize(material_size);
		for (size_t i = 0; i < Material.size(); i++)
		{
			char* name = new char[MAX_PATH];
			fread(name, sizeof(char), MAX_PATH, fp);
			this->Material[i].name.append(name);
			delete[] name;
		}
		fclose(fp);
		return false;
	}
	bool Model::FileSaveFullData(const char * Filename)
	{
		FILE *fp = NULL;

		if ((fp = fopen(Filename, "wb")) == NULL)
		{
			printf("ファイル(%s)が開けませんでした\n", Filename);
			return true;
		}
		int mesh_size = (int)Meshs.size();
		fwrite(&mesh_size, sizeof(int), 1, fp);

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			WriteMesh(&Meshs[i],fp);
		}

		int material_size = (int)Material.size();
		fwrite(&material_size, sizeof(int), 1, fp);
		for (size_t i = 0; i < Material.size(); i++)
		{
			fwrite(this->Material[i].name.data(), sizeof(TCHAR), MAX_PATH, fp);
		}

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			if (this->Bone.size() != 0) 
			{
				WriteAnimation(&this->Bone[i], fp);
			}
		}
		fclose(fp);
		return false;
	}
	bool Model::FileLoadFullData(const char* Filename)
	{
		FILE *fp = NULL;

		if ((fp = fopen(Filename, "rb")) == NULL)
		{
			printf("ファイルが開けませんでした\n");
			return true;
		}
		//Meshs数を受け取る
		int mesh_size;
		fread(&mesh_size, sizeof(int), 1, fp);
		Meshs.resize(mesh_size);

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			ReadMesh(&Meshs[i], fp);
		}
		int material_size;
		fread(&material_size, sizeof(int), 1, fp);
		Material.resize(material_size);
		for (size_t i = 0; i < Material.size(); i++)
		{
			char* name = new char[MAX_PATH];
			fread(name, sizeof(char), MAX_PATH, fp);
			this->Material[i].name.append(name);
			delete[] name;
		}

		for (size_t i = 0; i < Meshs.size(); i++)
		{
			ReadAnimation(&this->Bone[i],fp);
		}
		fclose(fp);
		return false;
	}
	bool Model::WriteMesh(Mesh* data, FILE* fp)
	{
		fwrite(&data->MaterialIndex, sizeof(int), 1, fp);
		fwrite(&data->PrimitiveIndex, sizeof(int), 1, fp);

		int vertex_arrey_size = (int)data->Vertex.size();
		fwrite(&vertex_arrey_size, sizeof(size_t), 1, fp);
		fwrite(data->Vertex.data(), sizeof(Float3), vertex_arrey_size, fp);

		int index_arrey_size = (int)data->Index.size();
		fwrite(&index_arrey_size, sizeof(int), 1, fp);
		fwrite(data->Index.data(), sizeof(int), index_arrey_size, fp);

		int normal_arrey_size = (int)data->Normals.size();
		fwrite(&normal_arrey_size, sizeof(int), 1, fp);
		fwrite(data->Normals.data(), sizeof(Float3), normal_arrey_size, fp);

		int UV_arrey_size = (int)data->UV.size();
		fwrite(&UV_arrey_size, sizeof(int), 1, fp);
		fwrite(data->UV.data(), sizeof(UV), UV_arrey_size, fp);
		return false;
	}
	bool Model::ReadMesh(Mesh* data, FILE* fp)
	{
		fread(&data->MaterialIndex, sizeof(int), 1, fp);
		fread(&data->PrimitiveIndex, sizeof(int), 1, fp);

		int vertex_arrey_size;
		fread(&vertex_arrey_size, sizeof(size_t), 1, fp);
		data->Vertex.resize(vertex_arrey_size);
		fread(data->Vertex.data(), sizeof(Float3), vertex_arrey_size, fp);

		int index_arrey_size;
		fread(&index_arrey_size, sizeof(int), 1, fp);
		data->Index.resize(index_arrey_size);
		fread(data->Index.data(), sizeof(int), index_arrey_size, fp);

		int normal_arrey_size;
		fread(&normal_arrey_size, sizeof(int), 1, fp);
		data->Normals.resize(normal_arrey_size);
		fread(data->Normals.data(), sizeof(Float3), normal_arrey_size, fp);

		int UV_arrey_size;
		fread(&UV_arrey_size, sizeof(int), 1, fp);
		data->UV.resize(UV_arrey_size);
		fread(data->UV.data(), sizeof(UV), UV_arrey_size, fp);
		return false;
	}
	bool Model::WriteAnimation(Skeleton* data, FILE* fp)
	{
		int Bones_size = (int)data->Bones.size();
		fwrite(&Bones_size, sizeof(int), 1, fp);
		for (int vc = 0; vc < Bones_size; vc++)
		{
			for (int bc = 0; bc < BONE_MAX; bc++)
			{
				fwrite(&data->Bones[vc].index[bc], sizeof(int), 1, fp);
				fwrite(&data->Bones[vc].weight[bc], sizeof(float), 1, fp);
			}
		}
		int Animation_Size = (int)data->Animations.size();
		fwrite(&Animation_Size, sizeof(int), 1, fp);
		for (int cn = 0; cn < Animation_Size; cn++)
		{
			fwrite(&data->Animations[cn].Frame, sizeof(int), 1, fp);
			fwrite(data->Animations[cn].name.data(), sizeof(char), MAX_PATH, fp);
			fwrite(&data->Animations[cn].init, sizeof(Matrix), 1, fp);
			for (int fn = 0; fn < data->Animations[cn].Frame; fn++)
			{
				fwrite(&data->Animations[cn].Matrixs[fn], sizeof(Matrix), 1, fp);
			}
		}
		return false;
	}
	bool Model::ReadAnimation(Skeleton* data, FILE* fp)
	{
		int Bones_size;
		fread(&Bones_size, sizeof(int), 1, fp);
		data->Bones.resize(Bones_size);
		for (int vc = 0; vc < Bones_size; vc++)
		{
			for (int bc = 0; bc < BONE_MAX; bc++)
			{
				fread(&data->Bones[vc].index[bc], sizeof(int), 1, fp);
				fread(&data->Bones[vc].weight[bc], sizeof(float), 1, fp);
			}
		}
		int Animation_Size;
		fread(&Animation_Size, sizeof(int), 1, fp);
		data->Animations.resize(Animation_Size);
		for (int cn = 0; cn < Animation_Size; cn++)
		{
			fread(&data->Animations[cn].Frame, sizeof(int), 1, fp);
			char* name = new char[MAX_PATH];
			fread(name, sizeof(char), MAX_PATH, fp);
			data->Animations[cn].name.append(name);
			delete[] name;
			fread(&data->Animations[cn].init, sizeof(Matrix), 1, fp);
			for (int fn = 0; fn < data->Animations[cn].Frame; fn++)
			{
				fread(&data->Animations[cn].Matrixs[fn], sizeof(Matrix), 1, fp);
			}
		}
		return false;
	}
}
