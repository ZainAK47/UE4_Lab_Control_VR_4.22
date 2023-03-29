// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_DualCameraDepthExtra.h"
#include "ViveSR_Enums.h"
#include "SRWorks.h"

ViveSR_DualCameraDepthExtra* ViveSR_DualCameraDepthExtra::Mgr = nullptr;
ViveSR_DualCameraDepthExtra::ViveSR_DualCameraDepthExtra()
{
}
ViveSR_DualCameraDepthExtra::~ViveSR_DualCameraDepthExtra()
{
}
ViveSR_DualCameraDepthExtra* ViveSR_DualCameraDepthExtra::Instance()
{
	if (Mgr == nullptr)
	{
		Mgr = new ViveSR_DualCameraDepthExtra();
	}
	return Mgr;
}
void ViveSR_DualCameraDepthExtra::Destroy()
{
	if (Mgr != nullptr)
	{
		delete Mgr;
		Mgr = nullptr;
	}
}

void ViveSR_DualCameraDepthExtra::InitialDepthCollider(int depthImageWidth, int depthImageHeight)
{
	RawDepthColliderVertices.Init(0, depthImageWidth * depthImageHeight * 3);
	RawColliderIndices.Init(0, depthImageWidth * depthImageHeight * 6);

	DataInfoDepthCollider = new DataInfo[NUM_DATA];
	DataInfoDepthCollider[0].mask = ViveSR::Depth::DataMask::TIME_STP;
	DataInfoDepthCollider[1].mask = ViveSR::Depth::DataMask::NUM_VERTICES;
	DataInfoDepthCollider[2].mask = ViveSR::Depth::DataMask::BYTEPERVERT;
	DataInfoDepthCollider[3].mask = ViveSR::Depth::DataMask::VERTICES;
	DataInfoDepthCollider[4].mask = ViveSR::Depth::DataMask::NUM_INDICES;
	DataInfoDepthCollider[5].mask = ViveSR::Depth::DataMask::INDICES;
	DataInfoDepthCollider[0].ptr = &DepthColliderTimeIndex;
	DataInfoDepthCollider[1].ptr = &ColliderVerticeNum;
	DataInfoDepthCollider[2].ptr = &ColliderBytePerVert;
	DataInfoDepthCollider[3].ptr = RawDepthColliderVertices.GetData();
	DataInfoDepthCollider[4].ptr = &ColliderIndicesNum;
	DataInfoDepthCollider[5].ptr = RawColliderIndices.GetData();
	InitialDepthColliderPtrSize = false;
}
void ViveSR_DualCameraDepthExtra::Release() {
	if (DataInfoDepthCollider) delete[] DataInfoDepthCollider;
}

bool ViveSR_DualCameraDepthExtra::GetDepthColliderFrameInfo()
{
	int sr_module_id = FSRWorksModule::Get().MODULE_ID_DEPTH;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialDepthColliderPtrSize) {
		result = ViveSR_GetMultiDataSize(sr_module_id, DataInfoDepthCollider, NUM_DATA);
		InitialDepthColliderPtrSize = result == ViveSR::SR_Error::WORK;
	}

	result = ViveSR_GetMultiData(sr_module_id, DataInfoDepthCollider, 1); // Only TimeIndex
	if (result != ViveSR::SR_Error::WORK) {
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update DEPTHCOLLIDER TimeIndex Error %d"), result);
	}
	return result == ViveSR::SR_Error::WORK;
}

bool ViveSR_DualCameraDepthExtra::GetDepthColliderData(int &verticesNum, TArray<float> &verticesBuff, int &indicesNum, TArray<int> &indicesBuff)
{
	int sr_module_id = FSRWorksModule::Get().MODULE_ID_DEPTH;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialDepthColliderPtrSize) {
		result = ViveSR_GetMultiDataSize(sr_module_id, DataInfoDepthCollider, NUM_DATA);
		InitialDepthColliderPtrSize = result == ViveSR::SR_Error::WORK;
	}

	result = ViveSR_GetMultiData(sr_module_id, DataInfoDepthCollider, NUM_DATA);
	if (result != ViveSR::SR_Error::WORK)
	{
		ColliderVerticeNum = 0;
		ColliderIndicesNum = 0;
		ColliderBytePerVert = 0;
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update DEPTHCOLLIDER Error %d"), result);
	}
	verticesNum = ColliderVerticeNum;
	verticesBuff = RawDepthColliderVertices;
	indicesNum = ColliderIndicesNum;
	indicesBuff = RawColliderIndices;
	return result == ViveSR::SR_Error::WORK;
}