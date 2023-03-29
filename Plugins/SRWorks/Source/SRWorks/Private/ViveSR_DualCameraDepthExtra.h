// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViveSR_API.h"
#include "Array.h"
/**
 * 
 */
class ViveSR_DualCameraDepthExtra
{
public:
	ViveSR_DualCameraDepthExtra();
	~ViveSR_DualCameraDepthExtra();
	static ViveSR_DualCameraDepthExtra* Instance();
	static void Destroy();

	int DepthColliderTimeIndex;
	void InitialDepthCollider(int depthImageWidth, int depthImageHeight);
	void Release();
	bool GetDepthColliderFrameInfo();
	bool GetDepthColliderData(int &verticesNum, TArray<float> &verticesBuff, int &indicesNum, TArray<int> &indicesBuff);
private:
	static ViveSR_DualCameraDepthExtra *Mgr;

	const int NUM_DATA = 6;
	DataInfo *DataInfoDepthCollider = new DataInfo[NUM_DATA];;
	bool InitialDepthColliderPtrSize = false;

	TArray<float> RawDepthColliderVertices;
	TArray<int> RawColliderIndices;
	int ColliderVerticeNum;
	int ColliderIndicesNum;
	int ColliderBytePerVert;
};
