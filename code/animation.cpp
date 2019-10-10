#include "animation.h"
#include <fbxsdk.h>


fbxsdk::FbxTime g_period;
int g_nStartFrame;
int g_nStopFrame;
bool g_bAnim;//アニメーションがあるかどうか



int g_idxNum;
int* g_idxTemp;
float* g_weightTemp;


void arrangeBone(mfmlib::Skeleton*InBone, int clusterNum);
void getAnimation(mfmlib::Skeleton*InBone, FbxCluster *cluster, int Id);

void getBone(mfmlib::Skeleton *InBone, mfmlib::Mesh *InData, FbxMesh *FbxMesh)
{

	// スキンの数を取得
	int skinCount = FbxMesh->GetDeformerCount(FbxDeformer::eSkin);

	if (skinCount >0)//スキンがある
	{
		printf("アニメーションあり");
		skinCount = 1;
		InBone->Animations.resize(skinCount);
	}
	else
	{
		printf("アニメーションなしﾅﾘ\n");
		return;
	}

	for (int i = 0; i < skinCount; ++i) 
	{
		FbxSkin* skin = (FbxSkin*)(FbxMesh->GetDeformer(i, FbxDeformer::eSkin));

		// fbxではボーンのことをクラスタというらしいです
		int clusterNum = skin->GetClusterCount();
		printf("[ボーン数：(%d)]\n", clusterNum);
		InBone->Bones.resize(clusterNum);

		if (clusterNum > 0)
		{
			InBone->Bones.resize(InData->Vertex.size());
		}
		else
		{
			return;
		}

		//初期化
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

		//そのボーンが影響する頂点のインデックス番号と、そのウェイトを取得
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
			//アニメーションのためのフレームごとの行列もげっとします
			if (g_bAnim)
			{
				getAnimation(InBone, cluster, j);
			}
		}
	}
	//ここで取得したのはあくまでインデックス番号（頂点のインデックスと対応）なので、このツール内または核各プログラム内で書き換える必要があります
}

void GetTimeSettings(FbxScene *Scene, mfmlib::Animation* model)
{
	//タイムモード取得
	FbxGlobalSettings &Set = Scene->GetGlobalSettings();

	FbxTime::EMode timeMode = Set.GetTimeMode();

	FbxArray<FbxString*> takeNameArray;



	Scene->FillAnimStackNameArray(takeNameArray);//テイク名でテイク情報を取得していきます。文字列表示しようと思ったらばぐった。。。
													//	printf("%s", takeNameArray);

	int numTake = takeNameArray.GetCount();// テイク数
	printf("テイク数%d\n", numTake);

	int i;
	for (i = 0; i < numTake; i++)
	{
		// テイク名からテイク情報を取得
		FbxTakeInfo* currentTakeInfo = Scene->GetTakeInfo(*(takeNameArray[i]));
		if (currentTakeInfo)
		{
			fbxsdk::FbxTime start = currentTakeInfo->mLocalTimeSpan.GetStart();//アニメーションが始まる時間
			fbxsdk::FbxTime stop = currentTakeInfo->mLocalTimeSpan.GetStop();//終わる時間
			g_period.SetTime(0, 0, 0, 1, 0, timeMode);//fbxの時間は秒とかそういうのじゃないので、1フレームあたりの時間数をﾄﾙﾉﾃﾞｽ
			g_nStartFrame = (int)(start.Get() / g_period.Get());//1フレームあたりで割れば、フレーム数になります
			g_nStopFrame = (int)(stop.Get() / g_period.Get());
			model->Frame = g_nStopFrame - g_nStartFrame + 1;
			printf("スタート時のフレーム数:%d\n", g_nStartFrame);
			printf("エンド時のフレーム数:%d\n", g_nStopFrame);
			break;//今は1テイクしかとらない
		}
	}

	if (i == numTake)//アニメーション情報がとれなかったら
	{
		g_bAnim = false;
		model->Frame = 1;//０わりとかこわいから。。。
		printf("アニメーション情報なし\n");
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

	//初期姿勢を取得
	FbxAMatrix initMat;
	cluster->GetTransformLinkMatrix(initMat);

	int frameCount = g_nStopFrame - g_nStartFrame + 1;
	InBone->Animations[Id].Matrixs.resize(frameCount);

	//行列セット
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
		//openglとdirectxの行列の変換って要るのか！？とりあえずやってみる

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

