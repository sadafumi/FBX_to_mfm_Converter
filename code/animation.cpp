#include "animation.h"
#include <fbxsdk.h>


fbxsdk::FbxTime g_period;
int g_nStartFrame;
int g_nStopFrame;
bool g_bAnim;//�A�j���[�V���������邩�ǂ���



int g_idxNum;
int* g_idxTemp;
float* g_weightTemp;


void arrangeBone(mfmlib::Skeleton*InBone, int clusterNum);
void getAnimation(mfmlib::Skeleton*InBone, FbxCluster *cluster, int Id);

void getBone(mfmlib::Skeleton *InBone, mfmlib::Mesh *InData, FbxMesh *FbxMesh)
{

	// �X�L���̐����擾
	int skinCount = FbxMesh->GetDeformerCount(FbxDeformer::eSkin);

	if (skinCount >0)//�X�L��������
	{
		printf("�A�j���[�V��������");
		skinCount = 1;
		InBone->Animations.resize(skinCount);
	}
	else
	{
		printf("�A�j���[�V�����Ȃ���\n");
		return;
	}

	for (int i = 0; i < skinCount; ++i) 
	{
		FbxSkin* skin = (FbxSkin*)(FbxMesh->GetDeformer(i, FbxDeformer::eSkin));

		// fbx�ł̓{�[���̂��Ƃ��N���X�^�Ƃ����炵���ł�
		int clusterNum = skin->GetClusterCount();
		printf("[�{�[�����F(%d)]\n", clusterNum);
		InBone->Bones.resize(clusterNum);

		if (clusterNum > 0)
		{
			InBone->Bones.resize(InData->Vertex.size());
		}
		else
		{
			return;
		}

		//������
		for (unsigned int k = 0; k < InData->Vertex.size(); k++)
		{
			InBone->Bones[k].weight.resize(BONE_MAX);
			InBone->Bones[k].index.resize(BONE_MAX);
			for (int j = 0; j < BONE_MAX; j++)
			{
				InBone->Bones[k].weight[j] = 0.0f;
				InBone->Bones[k].index[j] = 0;
			}
		}

		//���̃{�[�����e�����钸�_�̃C���f�b�N�X�ԍ��ƁA���̃E�F�C�g���擾
		for (int j = 0; j < clusterNum; ++j)
		{
			FbxCluster* cluster = skin->GetCluster(j);
			int	pointNum = cluster->GetControlPointIndicesCount();
			g_idxTemp = new int[pointNum];
			g_weightTemp = new float[pointNum];
			g_idxNum = pointNum;
			int*	pointAry = cluster->GetControlPointIndices();
			double* weightAry = cluster->GetControlPointWeights();

			for (int k = 0; k < pointNum; k++)
			{
				g_idxTemp[k] = pointAry[k];
				g_weightTemp[k] = (float)weightAry[k];
			}


			arrangeBone(InBone, j);

			delete[] g_idxTemp;
			delete[] g_weightTemp;
			//�A�j���[�V�����̂��߂̃t���[�����Ƃ̍s��������Ƃ��܂�
			if (g_bAnim)
			{
				getAnimation(InBone, cluster, j);
			}
		}
	}
	//�����Ŏ擾�����̂͂����܂ŃC���f�b�N�X�ԍ��i���_�̃C���f�b�N�X�ƑΉ��j�Ȃ̂ŁA���̃c�[�����܂��͊j�e�v���O�������ŏ���������K�v������܂�
}

void GetTimeSettings(FbxScene *Scene, mfmlib::Animation* model)
{
	//�^�C�����[�h�擾
	FbxGlobalSettings &Set = Scene->GetGlobalSettings();

	FbxTime::EMode timeMode = Set.GetTimeMode();

	FbxArray<FbxString*> takeNameArray;



	Scene->FillAnimStackNameArray(takeNameArray);//�e�C�N���Ńe�C�N�����擾���Ă����܂��B������\�����悤�Ǝv������΂������B�B�B
													//	printf("%s", takeNameArray);

	int numTake = takeNameArray.GetCount();// �e�C�N��
	printf("�e�C�N��%d\n", numTake);

	int i;
	for (i = 0; i < numTake; i++)
	{
		// �e�C�N������e�C�N�����擾
		FbxTakeInfo* currentTakeInfo = Scene->GetTakeInfo(*(takeNameArray[i]));
		if (currentTakeInfo)
		{
			fbxsdk::FbxTime start = currentTakeInfo->mLocalTimeSpan.GetStart();//�A�j���[�V�������n�܂鎞��
			fbxsdk::FbxTime stop = currentTakeInfo->mLocalTimeSpan.GetStop();//�I��鎞��
			g_period.SetTime(0, 0, 0, 1, 0, timeMode);//fbx�̎��Ԃ͕b�Ƃ����������̂���Ȃ��̂ŁA1�t���[��������̎��Ԑ�������޽
			g_nStartFrame = (int)(start.Get() / g_period.Get());//1�t���[��������Ŋ���΁A�t���[�����ɂȂ�܂�
			g_nStopFrame = (int)(stop.Get() / g_period.Get());
			model->Frame = g_nStopFrame - g_nStartFrame + 1;
			printf("�X�^�[�g���̃t���[����:%d\n", g_nStartFrame);
			printf("�G���h���̃t���[����:%d\n", g_nStopFrame);
			break;//����1�e�C�N�����Ƃ�Ȃ�
		}
	}

	if (i == numTake)//�A�j���[�V������񂪂Ƃ�Ȃ�������
	{
		g_bAnim = false;
		model->Frame = 1;//�O���Ƃ����킢����B�B�B
		printf("�A�j���[�V�������Ȃ�\n");
	}
	else
	{
		g_bAnim = true;
	}

	takeNameArray.Clear();
	takeNameArray = NULL;
	return;


}

void getAnimation(mfmlib::Skeleton*InBone, FbxCluster *cluster, int Id)
{

	//�����p�����擾
	FbxAMatrix initMat;
	cluster->GetTransformLinkMatrix(initMat);

	int frameCount = g_nStopFrame - g_nStartFrame + 1;
	InBone->Animations[Id].Matrixs.resize(frameCount);

	//�s��Z�b�g
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			InBone->Animations[Id].init.Set(r, c, (float)initMat.Get(r, c));
		}
	}

	for (int i = 0; i < frameCount; ++i)
	{
		FbxAMatrix mat;
		FbxTime time = (g_nStartFrame + i) * g_period.Get();
		mat = cluster->GetLink()->EvaluateGlobalTransform(time);
		//opengl��directx�̍s��̕ϊ����ėv��̂��I�H�Ƃ肠��������Ă݂�

		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				InBone->Animations[Id].Matrixs[i].Set(r, c, (float)mat.Get(r, c));
			}
		}
	}
}

void arrangeBone(mfmlib::Skeleton*InBone, int clusterNum)
{
	//for (int i = 0; i < fModel->MeshData[meshNum].BoneNum.GetNum(); i++)
	{
		for (int j = 0; j < g_idxNum; j++)
		{
			int veridx = g_idxTemp[j];
			for (int k = 0; k < BONE_MAX; k++)
			{
				InBone->Bones[veridx].index.resize(BONE_MAX);
				InBone->Bones[veridx].weight.resize(BONE_MAX);
				if (InBone->Bones[veridx].weight[k] == 0.0f)
				{
					InBone->Bones[veridx].weight[k] = g_weightTemp[j];
					InBone->Bones[veridx].index[k] = clusterNum;

					break;
				}
			}

		}

	}

}

