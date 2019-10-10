#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <fbxsdk.h>
#include "mfm.hpp"
#include "mfv.hpp"
#include "animation.h"
#include <direct.h>
#include "tools.h"

bool LoadFbx(std::string* FlieLink, mfmlib::Model* Mesh);
bool SetMeshDate(mfmlib::Mesh *InMesh, FbxMesh *FbxMesh);
bool SetMaterialDate(mfmlib::Material *InMesh, FbxSurfaceMaterial *FbxMesh);
void Change_Vertex_Data(mfmlib::Model* in_data, mfv::model* out_data);

int main(int argc, char *argv[])
{
	printf("引数の数%d\n", argc);
	for (int i = 0; i < argc; i++)
	{
		printf("引数の中身%s\n", argv[i]);
	}
	std::string path;
	mfmlib::Model* g_pModel;
	g_pModel = new mfmlib::Model;

	if (argv[1] == NULL)
	{
		std::cout << "FBXファイルのアドレスを入力\n";
		std::cout << "日本語ディレクトリはやめましょう";
		std::cin >> path;
		std::cout << path << "\n";
	}
	else
	{
		path.append(argv[1]);
	}
	if (LoadFbx(&path, g_pModel))
	{
		std::cout << "FBXファイルの読み込みに失敗しました\n";
	}
	else
	{
		mfv::model* Out_mfv;
		Out_mfv = new mfv::model;
		Change_Vertex_Data(g_pModel, Out_mfv);
		char exePath[MAX_PATH];
		if (0 != GetModuleFileName(NULL, exePath, MAX_PATH))
		{
			tools::CatFilePath(exePath, exePath);
		}
		{
			std::string out_mfm_path;
			std::string out_mfv_path;
			std::string out_exit_path;

			std::string Extension_mfm("mfm");
			std::string Extension_mfv("mtv");

			tools::Check_Export_Folder(&out_exit_path,exePath);
			tools::Path_Editer(&out_mfm_path, &out_exit_path, &path, &Extension_mfm);
			tools::Path_Editer(&out_mfv_path, &out_exit_path, &path, &Extension_mfv);

			g_pModel->FileSaveFullData(out_mfm_path.data());
			std::cout << "mfmファイルを正常に保存できました\n";

			mfmlib::Model test;

			Out_mfv->Save_File(out_mfv_path.data());
			std::cout<< "mfvファイルを正常に保存できました\n";

			test.FileLoadFullData(out_mfm_path.data());
		}
		delete Out_mfv;
	}
	std::cout << "終了\n";
	getchar();
	delete g_pModel;
}

bool CreateFbxManager(FbxManager** manager)
{
	*manager = FbxManager::Create();

	return *manager != nullptr;
}

bool CreateFbxImporter(FbxManager** manager, FbxImporter** importer)
{
	*importer = FbxImporter::Create(*manager, "");

	return *importer != nullptr;
}

bool CreateFbxScene(FbxManager** manager, FbxScene** scene)
{
	*scene = FbxScene::Create(*manager, "");

	return *scene != nullptr;
}
bool GetFbxMesh(FbxScene* scene, mfmlib::Model* Mesh) 
{
	// シーンに含まれるメッシュの解析
	auto meshCount = scene->GetSrcObjectCount<FbxMesh>();
	printf("Meshsの数(%d)\n", meshCount);
	Mesh->Meshs.resize(meshCount);
	for (size_t i = 0; i < (size_t)meshCount; i++)
	{
		auto* mesh = scene->GetSrcObject<FbxMesh>(i);
		printf("Meshs(%s)\n", mesh->GetName());
	}
	for (int i = 0; i < meshCount; ++i)
	{
		auto* mesh = scene->GetSrcObject<FbxMesh>(i);
		SetMeshDate(&Mesh->Meshs[i], mesh);
	}
	return false;
};
bool GetFbxMaterial(FbxScene* scene, mfmlib::Model* Mesh) 
{
	// シーンに含まれるマテリアルの解析
	auto materialCount = scene->GetMaterialCount();
	printf("materialの数(%d)\n", materialCount);
	Mesh->Material.resize(materialCount);
	for (int i = 0; i < materialCount; ++i)
	{
		FbxSurfaceMaterial* material = scene->GetMaterial(i);
		printf("material(%s)\n", material->GetName());
	}
	for (int i = 0; i < materialCount; i++)
	{
		FbxSurfaceMaterial* material = scene->GetMaterial(i);
		SetMaterialDate(&Mesh->Material[i], material);
	}
	return false;
};
bool GetFbxBones(FbxScene* scene)
{
	scene = scene;
	return false;
};
bool GetFbxAnimation(FbxScene* scene, mfmlib::Model* Mesh) 
{
	//ボーンやアニメーションの解析
	auto meshCount = scene->GetSrcObjectCount<FbxMesh>();
	Mesh->Bone.resize(meshCount);
	
	for (int i = 0; i < meshCount; ++i)
	{
		auto* mesh = scene->GetSrcObject<FbxMesh>(i);
		getBone(&Mesh->Bone[i], &Mesh->Meshs[i], mesh);
		GetTimeSettings(scene, &Mesh->Bone[i].Animations[0]);
	}
	return false;
};
bool LoadFbx(std::string* flie_link, mfmlib::Model* mesh)
{
	FbxManager* manager = nullptr;
	FbxImporter* importer = nullptr;
	FbxScene* scene = nullptr;

	// FBXの設定
	if (!CreateFbxManager(&manager))					return true;
	if (!CreateFbxImporter(&manager, &importer))		return true;
	if (!CreateFbxScene(&manager,  &scene))	return true;


	if(!importer->Initialize(flie_link->data(),-1, manager->GetIOSettings())) return true;
	importer->Import(scene);
	FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, true);
	printf("ポリゴンの三頂点化\n");
	geometryConverter.SplitMeshesPerMaterial(scene, true);
	printf("マテリアルごとにメッシュを分割\n");

	if(GetFbxMesh(scene, mesh)) return true;
	if(GetFbxMaterial(scene, mesh))return true;
	//GetFbxAnimation(scene, mesh);

	printf("==============================\n");
	printf("使用されているテクスチャ\n");
	for (size_t i = 0; i < (size_t)mesh->Material.size(); i++)
	{
		std::
		printf("Material番号(%d)テクスチャ名[%s]\n", i, mesh->Material[i].name.data());
	}
	printf("==============================\n");

	return false;
}
bool SetMeshDate(mfmlib::Mesh *in_data, FbxMesh *fbx_mesh)
{
	int MaterialNum;
	int *IndexAry = fbx_mesh->GetPolygonVertices();			//インデックス配列のポインタ
	FbxVector4* src = fbx_mesh->GetControlPoints();			// 頂点座標配列

	FbxArray<FbxVector4> normals;
	fbx_mesh->GetPolygonVertexNormals(normals);

	FbxStringList uvsetName;
	fbx_mesh->GetUVSetNames(uvsetName);
	FbxArray<FbxVector2> uvsets;
	fbx_mesh->GetPolygonVertexUVs(uvsetName.GetStringAt(0), uvsets);

	in_data->UV.resize(uvsets.Size());
	if (uvsetName.GetCount() > 0)
	{
		for (size_t i = 0; i < (size_t)uvsets.Size(); i++)
		{
			in_data->UV[i].U = (float)uvsets[i][0];
			in_data->UV[i].V = 1.0f - (float)uvsets[i][1];
		}
	}

	in_data->Index.resize(fbx_mesh->GetPolygonVertexCount());
	if (fbx_mesh->GetPolygonVertexCount() == 0)
	{
		std::cout << "インデックスがありません\n";
		return true;
	}
	for (size_t i = 0; i < (size_t)fbx_mesh->GetPolygonVertexCount(); i++)
	{
		in_data->Index[i] = IndexAry[i];
	}
	if (fbx_mesh->GetControlPointsCount() == 0)
	{
		std::cout << "頂点がありません\n";
		return true;
	}
	in_data->Vertex.resize(fbx_mesh->GetControlPointsCount());
	for (size_t i = 0; i < (size_t)fbx_mesh->GetControlPointsCount(); i++)
	{
		in_data->Vertex[i].x = (float)src[i][0];
		in_data->Vertex[i].y = (float)src[i][1];
		in_data->Vertex[i].z = (float)src[i][2];
	}
	
	in_data->Normals.resize(normals.Size());
	if (normals.Size() == 0)
	{
		std::cout<< "法線がありません\n";
		return true;
	}
	for (size_t i = 0; i < (size_t)normals.Size(); i++)
	{
		in_data->Normals[i].x = (float)normals[i][0];
		in_data->Normals[i].y = (float)normals[i][1];
		in_data->Normals[i].z = (float)normals[i][2];
	}

	//FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);

	FbxLayerElementMaterial *Material = fbx_mesh->GetLayer(0)->GetMaterials();
	//if( Material->GetMappingMode() == FbxLayerElement::eAllSame)
	if (Material != nullptr)
	{
		if (Material->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
		{
			MaterialNum = 0;
			printf("MaterialNum(%d)\n", MaterialNum);
		}
		else
		{
			MaterialNum = Material->GetIndexArray().GetAt(0);
			printf("MaterialNum(%d)\n", MaterialNum);
		}

		in_data->MaterialIndex = MaterialNum;
	}
	in_data->PrimitiveIndex = fbx_mesh->GetPolygonCount();
	return false;
}
bool SetMaterialDate(mfmlib::Material *in_data, FbxSurfaceMaterial *fbx_mesh)
{


	FbxProperty prop = fbx_mesh->FindProperty(FbxSurfaceMaterial::sDiffuse);
	int num = prop.GetSrcObjectCount<FbxLayeredTexture>();
	int Texnum = prop.GetSrcObjectCount<FbxFileTexture>();
	FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(0);

	printf("ベース(%d)レイヤード(%d)\n", Texnum, num);
	if (num > 0 || Texnum > 0)
	{
		in_data->name.append(texture->GetRelativeFileName());
	}
	else
	{
		in_data->name.append("");
	}
	return false;
}

void Change_Vertex_Data(mfmlib::Model* in_data, mfv::model* out_data)
{
	if (in_data->Meshs[0].Vertex.size() != 0)
	{
		out_data->strides.push_back(mfv::model::Str_vertex);
		out_data->stride_size += sizeof(float) * 4;
	}
	if (in_data->Meshs[0].Normals.size() != 0)
	{
		out_data->strides.push_back(mfv::model::Str_normal);
		out_data->stride_size += sizeof(float) * 3;
	}
	if (in_data->Meshs[0].UV.size() != 0)
	{
		out_data->strides.push_back(mfv::model::Str_uv);
		out_data->stride_size += sizeof(float) * 2;
	}
	//if (in_data->BoneData[0].VertexData.Num != 0)
	//{
	//	out_data->strides.push_back(mfv::model::Str_weight);
	//	out_data->stride_size += sizeof(float) * 2;
	//}

	std::cout << "Meshs_num["<< in_data->Meshs.size() << "]\n";
	out_data->meshs.resize(in_data->Meshs.size());
	for (unsigned int i = 0; i < in_data->Meshs.size(); i++)
	{
		printf("Meshs_count[%d]\n", i);
		printf("vertex_size[%8d]\n", in_data->Meshs[i].Index.size());
		out_data->meshs[i].vertex_size = in_data->Meshs[i].Index.size();
		out_data->meshs[i].vertex_arry.resize(in_data->Meshs[i].Index.size());
		for (unsigned int vertex_index = 0; vertex_index < in_data->Meshs[i].Index.size(); vertex_index++)
		{
			int index = in_data->Meshs[i].Index[vertex_index];

			out_data->meshs[i].vertex_arry[vertex_index].Pos_x = in_data->Meshs[i].Vertex[index].x;
			out_data->meshs[i].vertex_arry[vertex_index].Pos_y = in_data->Meshs[i].Vertex[index].y;
			out_data->meshs[i].vertex_arry[vertex_index].Pos_z = in_data->Meshs[i].Vertex[index].z;
			out_data->meshs[i].vertex_arry[vertex_index].Pos_w = 1.0f;

			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Vertex[index].x);
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Vertex[index].y);
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Vertex[index].z);
			out_data->meshs[i].vertexs.push_back(1.0f);

			out_data->meshs[i].vertex_arry[vertex_index].Normal_x = in_data->Meshs[i].Normals[vertex_index].x;
			out_data->meshs[i].vertex_arry[vertex_index].Normal_y = in_data->Meshs[i].Normals[vertex_index].y;
			out_data->meshs[i].vertex_arry[vertex_index].Normal_z = in_data->Meshs[i].Normals[vertex_index].z;

			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Normals[vertex_index].x);
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Normals[vertex_index].y);
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].Normals[vertex_index].z);


			out_data->meshs[i].vertex_arry[vertex_index].U = in_data->Meshs[i].UV[vertex_index].U;
			out_data->meshs[i].vertex_arry[vertex_index].V = in_data->Meshs[i].UV[vertex_index].V;
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].UV[vertex_index].U);
			out_data->meshs[i].vertexs.push_back(in_data->Meshs[i].UV[vertex_index].V);
		}
		std::string Extension("mtf");
		out_data->meshs[i].tex_file.name.append(in_data->Material[in_data->Meshs[i].MaterialIndex].name);
		tools::ChangeFileExtension(&out_data->meshs[i].tex_file.name, &in_data->Material[in_data->Meshs[i].MaterialIndex].name, &Extension);
	}
}


