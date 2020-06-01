
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

		bool FileSaveMeshData(const char* Filename);//ファイル名を指定してメッシュだけを保存する
		bool FileLoadMeshData(const char* Filename);//ファイル名を指定してメッシュだけを保存する
		bool FileSaveFullData(const char* Filename);//ファイル名を指定してすべての情報を保存する
		bool FileLoadFullData(const char* Filename);//ファイル名を指定してすべての情報を保存する
	private:
		bool WriteMesh(Mesh* data, FILE* fp);
		bool ReadMesh(Mesh* data, FILE* fp);
		bool WriteAnimation(Skeleton* data, FILE* fp);
		bool ReadAnimation(Skeleton* data,FILE *fp);
	};
}


#endif //__MFM_H__
