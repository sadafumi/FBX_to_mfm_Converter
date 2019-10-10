//-----------------------------------------------------------------------------
// AT12A242 �@��j���@2017/6/27 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ���C���w�b�_			�@�@
//-----------------------------------------------------------------------------
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
		std::vector<int> index;//directx�ł����Ƃ���unsigned�����ǐ��̐������͂���񂩂炽�Ԃ���v
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
		//�f�[�^���o�����ꂷ��N���X�Q
		std::vector<Float3> Vertex;
		std::vector<Float3> Normals;
		std::vector<UV> UV;
		std::vector<int> Index;
		int MaterialIndex;
		int PrimitiveIndex;
	private:
	};

	//�t�@�C����IO�Ɏg���N���X
	//�֐��Ƃ��Ē�`���Ċ֐���IO���s��
	class Model
	{
	public:
		std::vector<Mesh>		Meshs;		//���b�V���̎��f�[�^
		std::vector<Material>	Material;	//�}�e���A���̎��f�[�^
		std::vector<Skeleton>	Bone;		//Mesh�̐�����

		bool FileSaveMeshData(const char* Filename);//�t�@�C�������w�肵�ă��b�V��������ۑ�����
		bool FileLoadMeshData(const char* Filename);//�t�@�C�������w�肵�ă��b�V��������ۑ�����
		bool FileSaveFullData(const char* Filename);//�t�@�C�������w�肵�Ă��ׂĂ̏���ۑ�����
		bool FileLoadFullData(const char* Filename);//�t�@�C�������w�肵�Ă��ׂĂ̏���ۑ�����
	private:
		bool WriteMesh(Mesh* data, FILE* fp);
		bool ReadMesh(Mesh* data, FILE* fp);
		bool WriteAnimation(Skeleton* data, FILE* fp);
		bool ReadAnimation(Skeleton* data,FILE *fp);
	};
}


#endif //__MFM_H__
