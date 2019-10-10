#ifndef _ANIMATION_H_
#define _ANIMATION_H_
#include <fbxsdk.h>
#include "mfm.h"

void GetTimeSettings(FbxScene *Scene, mfmlib::Animation* model);
void getBone(mfmlib::Skeleton *InBone, mfmlib::Mesh *InData, FbxMesh *FbxMesh);

#endif