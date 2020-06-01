
#ifndef __MFM_H__
#define __MFM_H__

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>

#define BONE_MAX (4)

namespace mfmlib
{
	struct Float3
	{
		float x;
		float y;
		float z;
	};

	struct UV
	{
		float U;
		float V;
	};

	struct Matrix
	{
	public:
		void Set(int r, int c, float in) { Element[r][c] = in; };
		float Get(int r, int c) { return Element[r][c]; };
	private:
		float Element[4][4];
	};

	struct Bone
	{
	public:
		std::vector<float> weight;
		std::vector<int> index;//directxでつかうときはunsignedだけど正の数しかはいらんからたぶん大丈夫
	};
	struct Material
	{
		std::string name;
	};

	class Animation
	{
	public:
		int Frame;
		std::string name;
		Matrix init;
		std::vector<Matrix> Matrixs;
	private:
	};
	class Skeleton
	{
	public:
		std::vector<Animation> Animations;
		std::vector<Bone> Bones;
	};

	class Mesh
	{
	public:
		//データを出し入れするクラス群
		std::vector<Float3> Vertex;
		std::vector<Float3> Normals;
		std::vector<UV> UV;
		std::vector<int> Index;
		int MaterialIndex;
		int PrimitiveIndex;
	private:
	};

	//ファイルのIOに使うクラス
	//関数として定義して関数でIOを行う
	class Model
	{
	public:
		std::vector<Mesh>		Meshs;		//メッシュの実データ
		std::vector<Material>	Material;	//マテリアルの実データ
		std::vector<Skeleton>	Bone;		//Meshの数だけ

		bool FileSaveMeshData(const char* Filename) 
		{
			FILE* fp = NULL;

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
		};
		bool FileLoadMeshData(const char* Filename) 
		{
			FILE* fp = NULL;

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
		};
		bool FileSaveFullData(const char* Filename) 
		{
			FILE* fp = NULL;

			if ((fp = fopen(Filename, "wb")) == NULL)
			{
				printf("ファイル(%s)が開けませんでした\n", Filename);
				return true;
			}
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

			int Bones_arrey_size = (int)this->Bone.size();
			fwrite(&Bones_arrey_size, sizeof(size_t), 1, fp);
			if (this->Bone.size() != 0)
			{
				for (size_t i = 0; i < Meshs.size(); i++)
				{

					if (this->Bone[i].Bones.size() != 0)
					{
						WriteAnimation(&this->Bone[i], fp);
					}
				}
			}
			fclose(fp);
			return false;
		};
		bool FileLoadFullData(const char* Filename) 
		{
			FILE* fp = NULL;

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
			int Bones_arrey_size;
			fread(&Bones_arrey_size, sizeof(size_t), 1, fp);
			if (Bones_arrey_size != 0)
			{
				for (int i = 0; i < Bones_arrey_size; i++)
				{
					this->Bone.resize(Bones_arrey_size);
					if (this->Bone[i].Bones.size() != 0)
					{
						ReadAnimation(&this->Bone[i], fp);
					}
				}
			}
			fclose(fp);
			return false;
		};
	private:
		bool WriteMesh(Mesh* data, FILE* fp) 
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
		};
		bool ReadMesh(Mesh* data, FILE* fp) 
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
		};
		bool WriteAnimation(Skeleton* data, FILE* fp) 
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
		};
		bool ReadAnimation(Skeleton* data, FILE* fp) 
		{
			int Bones_size;
			fread(&Bones_size, sizeof(int), 1, fp);
			if (Bones_size != 0)
			{
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
			}
			return false;
		};
	};
}


#endif //__MFM_H__
