// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_DualCameraImageCapture.h"
#include "SRWorks.h"
#include "ViveSR_API.h"
#include "UnrealMathUtility.h"
#include <algorithm>

ViveSR_DualCameraImageCapture* ViveSR_DualCameraImageCapture::Mgr = nullptr;
ViveSR_DualCameraImageCapture::ViveSR_DualCameraImageCapture()
{
}

ViveSR_DualCameraImageCapture::~ViveSR_DualCameraImageCapture()
{
	Release();
}
void ViveSR_DualCameraImageCapture::Initial()
{
	GetParameters();
	// Distorted
	RawDistortedLeftBGRA = new uint8[DistortedImageWidth * DistortedImageHeight * DistortedImageChannel];
	RawDistortedRightBGRA = new uint8[DistortedImageWidth * DistortedImageHeight * DistortedImageChannel];
	std::fill_n(RawDistortedLeftBGRA, DistortedImageWidth * DistortedImageHeight * DistortedImageChannel, 0);
	std::fill_n(RawDistortedRightBGRA, DistortedImageWidth * DistortedImageHeight * DistortedImageChannel, 0);

	textureDistortedLeft = UTexture2D::CreateTransient(DistortedImageWidth, DistortedImageHeight);
	textureDistortedRight = UTexture2D::CreateTransient(DistortedImageWidth, DistortedImageHeight);
	textureDistortedLeft->AddToRoot();
	textureDistortedRight->AddToRoot();
	textureDistortedLeft->UpdateResource();
	textureDistortedRight->UpdateResource();
	textureDistortedRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, DistortedImageWidth, DistortedImageHeight);

	// Undistorted
	RawUndistortedLeftBGRA = new uint8[UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel];
	RawUndistortedRightBGRA = new uint8[UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel];
	std::fill_n(RawUndistortedLeftBGRA, UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel, 0);
	std::fill_n(RawUndistortedRightBGRA, UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel, 0);

	textureUndistortedLeft = UTexture2D::CreateTransient(UndistortedImageWidth, UndistortedImageHeight);
	textureUndistortedRight = UTexture2D::CreateTransient(UndistortedImageWidth, UndistortedImageHeight);
	textureUndistortedLeft->AddToRoot();
	textureUndistortedRight->AddToRoot();
	textureUndistortedLeft->UpdateResource();
	textureUndistortedRight->UpdateResource();
	textureUndistortedRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, UndistortedImageWidth, UndistortedImageHeight);

	// Depth
	RawDepth = new uint8[DepthImageWidth * DepthImageHeight * DepthImageChannel * sizeof(float)];
	textureDepth = UTexture2D::CreateTransient(DepthImageWidth, DepthImageHeight, EPixelFormat::PF_R32_FLOAT);
	textureDepth->AddToRoot();
	textureDepth->UpdateResource();
	textureDepthRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, DepthImageWidth, DepthImageHeight);

	DataInfoDistorted = new DataInfo[NUM_DATA_DISTORTED];
	for (int i = 0; i < NUM_DATA_DISTORTED; i++) { DataInfoDistorted[i].mask = i; }
	RawAllDistortedData = (uint8*)malloc(DistortedImageWidth * DistortedImageHeight * DistortedImageChannel * 2 + 4 + 4 + 8 * 16);
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_LEFT].ptr	= RawAllDistortedData;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_RIGHT].ptr	= RawAllDistortedData + DistortedImageWidth * DistortedImageHeight * DistortedImageChannel;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_LEFT].ptr = NULL;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_RIGHT].ptr= NULL;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::FRAME_SEQ].ptr				= &DistortedFrameIndex;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::TIME_STP].ptr				= &DistortedTimeIndex;
	DataInfoDistorted[ViveSR::SeeThrough::DataMask::POSE_LEFT].ptr				= RawDistortedPoseLeft;
	FrameInfoDistorted = new DataInfo[1];
	FrameInfoDistorted[0].mask = ViveSR::SeeThrough::DataMask::FRAME_SEQ;
	FrameInfoDistorted[0].ptr = &DistortedFrameIndex;

	DataInfoUndistorted = new DataInfo[NUM_DATA_UNDISTORTED];
	for (int i = 0; i < NUM_DATA_UNDISTORTED; i++) { DataInfoUndistorted[i].mask = i; }
	RawAllUndistortedData = (uint8*)malloc(UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel * 2 + 4 + 4 + 8 * 16);
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_LEFT].ptr		= NULL;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_RIGHT].ptr	= NULL;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_LEFT].ptr	= RawAllUndistortedData;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_RIGHT].ptr	= RawAllUndistortedData + UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::FRAME_SEQ].ptr				= &UndistortedFrameIndex;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::TIME_STP].ptr					= &UndistortedTimeIndex;
	DataInfoUndistorted[ViveSR::SeeThrough::DataMask::POSE_LEFT].ptr				= RawUndistortedPoseLeft;
	FrameInfoUndistorted = new DataInfo[1];
	FrameInfoUndistorted[0].mask = ViveSR::SeeThrough::DataMask::FRAME_SEQ;
	FrameInfoUndistorted[0].ptr = &UndistortedFrameIndex;

	DataInfoDepth = new DataInfo[NUM_DATA_DEPTH];
	for (int i = 0; i < NUM_DATA_DEPTH; i++) { DataInfoDepth[i].mask = i; }
	DataInfoDepth[ViveSR::Depth::DataMask::LEFT_FRAME].ptr = NULL;
	DataInfoDepth[ViveSR::Depth::DataMask::DEPTH_MAP ].ptr = RawDepth;
	DataInfoDepth[ViveSR::Depth::DataMask::FRAME_SEQ ].ptr = &DepthFrameIndex;
	DataInfoDepth[ViveSR::Depth::DataMask::TIME_STP  ].ptr = &DepthTimeIndex;
	DataInfoDepth[ViveSR::Depth::DataMask::POSE      ].ptr = RawDepthPose;
	FrameInfoDepth = new DataInfo[1];
	FrameInfoDepth[0].mask = ViveSR::Depth::DataMask::FRAME_SEQ;
	FrameInfoDepth[0].ptr = &DepthFrameIndex;
	InitialDistortedPtrSize = false;
	InitialUndistortedPtrSize = false;
	InitialDepthPtrSize = false;
}
void ViveSR_DualCameraImageCapture::Release() {
	DepthProcessing = false;
	DepthRefinement = false;
	DepthEdgeEnhance = false;
	if (RawDistortedLeftBGRA) delete[] RawDistortedLeftBGRA;
	if (RawDistortedRightBGRA) delete[] RawDistortedRightBGRA;
	if (textureDistortedRegion) FMemory::Free(textureDistortedRegion);
	if (RawUndistortedLeftBGRA) delete[] RawUndistortedLeftBGRA;
	if (RawUndistortedRightBGRA) delete[] RawUndistortedRightBGRA;
	if (textureUndistortedRegion) FMemory::Free(textureUndistortedRegion);
	if (RawDepth) delete[] RawDepth;
	if (textureDepthRegion) FMemory::Free(textureDepthRegion);
	if (DataInfoDistorted) delete[] DataInfoDistorted;
	if (FrameInfoDistorted) delete[] FrameInfoDistorted;
	if (DataInfoUndistorted) delete[] DataInfoUndistorted;
	if (FrameInfoUndistorted) delete[] FrameInfoUndistorted;
	if (DataInfoDepth) delete[] DataInfoDepth;
	if (FrameInfoDepth) delete[] FrameInfoDepth;
	if (UndistortionMap_L) delete[] UndistortionMap_L;
	if (UndistortionMap_R) delete[] UndistortionMap_R;
	if (RawAllUndistortedData) free(RawAllUndistortedData);
}

void ViveSR_DualCameraImageCapture::GetParameters()
{
	ViveSR_GetCameraParams(&CameraParameters);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_WIDTH, &DistortedImageWidth);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_HEIGHT, &DistortedImageHeight);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_CHANNEL, &DistortedImageChannel);
	//UE_LOG(LogTemp, Log, TEXT("[ViveSR] DISTORTED parameters: width: %d, height: %d, channel: %d"), DistortedImageWidth, DistortedImageHeight, DistortedImageChannel);

	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_WIDTH, &UndistortedImageWidth);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_HEIGHT, &UndistortedImageHeight);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_CHANNEL, &UndistortedImageChannel);
	//UE_LOG(LogTemp, Log, TEXT("[ViveSR] UNDISTORTED parameters: width: %d, height: %d, channel: %d"), UndistortedImageWidth, UndistortedImageHeight, UndistortedImageChannel);

	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::OUTPUT_WIDTH, &DepthImageWidth);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::OUTPUT_HEIGHT, &DepthImageHeight);
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::OUTPUT_CHAANEL_1, &DepthImageChannel);

	int undistortionMapSize = 0;
	ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::UNDISTORTION_MAP_SIZE, &undistortionMapSize);
	UndistortionMap_L = new float[undistortionMapSize / sizeof(float)];
	UndistortionMap_R = new float[undistortionMapSize / sizeof(float)];
	ViveSR_GetParameterFloatArray(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::UNDISTORTION_MAP_L, &UndistortionMap_L);
	ViveSR_GetParameterFloatArray(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::UNDISTORTION_MAP_R, &UndistortionMap_R);

	float* rawUndistortedCxCyArray = new float[8];
	ViveSR_GetParameterFloatArray(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, ViveSR::SeeThrough::Param::UNDISTORTION_CENTER, &rawUndistortedCxCyArray);
	double* undistortedCxCyArray = (double*)rawUndistortedCxCyArray;
	UndistortedCx_L = undistortedCxCyArray[0];
	UndistortedCy_L = undistortedCxCyArray[1];
	UndistortedCx_R = undistortedCxCyArray[2];
	UndistortedCy_R = undistortedCxCyArray[3];
	delete[] rawUndistortedCxCyArray;
}

ViveSR_DualCameraImageCapture* ViveSR_DualCameraImageCapture::Instance()
{
	if (Mgr == nullptr)
	{
		Mgr = new ViveSR_DualCameraImageCapture();
	}
	return Mgr;
}

void ViveSR_DualCameraImageCapture::DestroyImageCapture()
{
	if (Mgr != nullptr)
	{
		delete Mgr;
		Mgr = nullptr;
	}
}

void ViveSR_DualCameraImageCapture::GetDistortedTexture(UTexture2D*& imageLeft, UTexture2D*& imageRight, int& frameIndex, int& timeIndex)
{
	textureDistortedLeft->UpdateTextureRegions(0, 1, textureDistortedRegion, static_cast<uint32>(DistortedImageWidth * sizeof(uint8) * 4), sizeof(uint8) * 4, RawDistortedLeftBGRA, texCleanUpFP);
	textureDistortedRight->UpdateTextureRegions(0, 1, textureDistortedRegion, static_cast<uint32>(DistortedImageWidth * sizeof(uint8) * 4), sizeof(uint8) * 4, RawDistortedRightBGRA, texCleanUpFP);
	imageLeft = textureDistortedLeft;
	imageRight = textureDistortedRight;
	frameIndex = DistortedFrameIndex;
	timeIndex = DistortedTimeIndex;
}

void ViveSR_DualCameraImageCapture::GetUndistortedTexture(UTexture2D*& imageLeft, UTexture2D*& imageRight, int& frameIndex, int& timeIndex)
{
	textureUndistortedLeft->UpdateTextureRegions(0, 1, textureUndistortedRegion, static_cast<uint32>(UndistortedImageWidth * sizeof(uint8) * 4), sizeof(uint8) * 4, RawUndistortedLeftBGRA, texCleanUpFP);
	textureUndistortedRight->UpdateTextureRegions(0, 1, textureUndistortedRegion, static_cast<uint32>(UndistortedImageWidth * sizeof(uint8) * 4), sizeof(uint8) * 4, RawUndistortedRightBGRA, texCleanUpFP);

	imageLeft = textureUndistortedLeft;
	imageRight = textureUndistortedRight;
	frameIndex = UndistortedFrameIndex;
	timeIndex = UndistortedTimeIndex;
}

void ViveSR_DualCameraImageCapture::GetDepthTexture(UTexture2D*& imageDepth, int& frameIndex, int& timeIndex)
{
	textureDepth->UpdateTextureRegions(0, 1, textureDepthRegion, static_cast<uint32>(DepthImageWidth * sizeof(float) * DepthImageChannel), sizeof(float), RawDepth, texCleanUpFP);
	imageDepth = textureDepth;
	frameIndex = DepthFrameIndex;
	timeIndex = DepthTimeIndex;
}

void ViveSR_DualCameraImageCapture::UpdateDistortedImage()
{
	int sr_module_id = FSRWorksModule::Get().MODULE_ID_SEETHROUGH;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialDistortedPtrSize) {
		int res0 = ViveSR_GetMultiDataSize(sr_module_id, DataInfoDistorted, NUM_DATA_DISTORTED);
		int res1 = ViveSR_GetMultiDataSize(sr_module_id, FrameInfoDistorted, 1);
		InitialDistortedPtrSize = res0 == res1 && res0 == ViveSR::SR_Error::WORK;
	}

	result = ViveSR_GetMultiData(sr_module_id, FrameInfoDistorted, 1);
	if (LastDistortedFrameIndex == DistortedFrameIndex) return;
	else LastDistortedFrameIndex = DistortedFrameIndex;

	result = ViveSR_GetMultiData(sr_module_id, DataInfoDistorted, NUM_DATA_DISTORTED);
	if (result == ViveSR::SR_Error::WORK)
	{
		int startIndex = 0, length = 0;
		length = DistortedImageWidth * DistortedImageHeight * DistortedImageChannel;
		memcpy(RawDistortedLeftBGRA, RawAllDistortedData, length);

		startIndex += length;
		length = DistortedImageWidth * DistortedImageHeight * DistortedImageChannel;
		memcpy(RawDistortedRightBGRA, RawAllDistortedData + startIndex, length);

		DistortedPoseLeft = FMatrix(
			FPlane(RawDistortedPoseLeft[0], RawDistortedPoseLeft[4], RawDistortedPoseLeft[8], RawDistortedPoseLeft[12]),
			FPlane(RawDistortedPoseLeft[1], RawDistortedPoseLeft[5], RawDistortedPoseLeft[9], RawDistortedPoseLeft[13]),
			FPlane(RawDistortedPoseLeft[2], RawDistortedPoseLeft[6], RawDistortedPoseLeft[10], RawDistortedPoseLeft[14]),
			FPlane(RawDistortedPoseLeft[3], RawDistortedPoseLeft[7], RawDistortedPoseLeft[11], RawDistortedPoseLeft[15]));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update DISTORTED Error %d"), result);
	}
}

void ViveSR_DualCameraImageCapture::UpdateUndistortedImage()
{
	int sr_module_id = FSRWorksModule::Get().MODULE_ID_SEETHROUGH;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialUndistortedPtrSize) {
		int res0 = ViveSR_GetMultiDataSize(sr_module_id, DataInfoUndistorted, NUM_DATA_UNDISTORTED);
		int res1 = ViveSR_GetMultiDataSize(sr_module_id, FrameInfoUndistorted, 1);
		InitialUndistortedPtrSize = res0 == res1 && res0 == ViveSR::SR_Error::WORK;
	}

	result = ViveSR_GetMultiData(sr_module_id, FrameInfoUndistorted, 1);
	if (LastUndistortedFrameIndex == UndistortedFrameIndex) return;
	else LastUndistortedFrameIndex = UndistortedFrameIndex;

	result = ViveSR_GetMultiData(sr_module_id, DataInfoUndistorted, NUM_DATA_UNDISTORTED);
	if (result == ViveSR::SR_Error::WORK)
	{
		int startIndex = 0, length = 0;
		length = UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel;
		memcpy(RawUndistortedLeftBGRA, RawAllUndistortedData, length);

		startIndex += length;
		length = UndistortedImageWidth * UndistortedImageHeight * UndistortedImageChannel;
		memcpy(RawUndistortedRightBGRA, RawAllUndistortedData + startIndex, length);

		UndistortedPoseLeft = FMatrix(
			FPlane(RawUndistortedPoseLeft[0], RawUndistortedPoseLeft[4], RawUndistortedPoseLeft[8], RawUndistortedPoseLeft[12]),
			FPlane(RawUndistortedPoseLeft[1], RawUndistortedPoseLeft[5], RawUndistortedPoseLeft[9], RawUndistortedPoseLeft[13]),
			FPlane(RawUndistortedPoseLeft[2], RawUndistortedPoseLeft[6], RawUndistortedPoseLeft[10], RawUndistortedPoseLeft[14]),
			FPlane(RawUndistortedPoseLeft[3], RawUndistortedPoseLeft[7], RawUndistortedPoseLeft[11], RawUndistortedPoseLeft[15]));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update UNDISTORTED Error %d (Get Data)"), result);
	}
}
void ViveSR_DualCameraImageCapture::UpdateDepthImage()
{
	int sr_module_id = FSRWorksModule::Get().MODULE_ID_DEPTH;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialDepthPtrSize) {
		int res0 = ViveSR_GetMultiDataSize(sr_module_id, DataInfoDepth, NUM_DATA_DEPTH);
		int res1 = ViveSR_GetMultiDataSize(sr_module_id, FrameInfoDepth, 1);
		InitialDepthPtrSize = res0 == res1 && res0 == ViveSR::SR_Error::WORK;
	}

	result = ViveSR_GetMultiData(sr_module_id, FrameInfoDepth, 1);
	if (LastDepthFrameIndex == DepthFrameIndex) return;
	else LastDepthFrameIndex = DepthFrameIndex;
	
	result = ViveSR_GetMultiData(sr_module_id, DataInfoDepth, NUM_DATA_DEPTH);
	if (result == ViveSR::SR_Error::WORK)
	{
		DepthPose = FMatrix(
			FPlane(RawDepthPose[0], RawDepthPose[4], RawDepthPose[8], RawDepthPose[12]),
			FPlane(RawDepthPose[1], RawDepthPose[5], RawDepthPose[9], RawDepthPose[13]),
			FPlane(RawDepthPose[2], RawDepthPose[6], RawDepthPose[10], RawDepthPose[14]),
			FPlane(RawDepthPose[3], RawDepthPose[7], RawDepthPose[11], RawDepthPose[15]));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update DEPTH Error %d"), result);
	}
}
#pragma region Utility
FVector ViveSR_DualCameraImageCapture::Position(FMatrix m)
{
	return FVector(m.M[2][3], m.M[0][3], m.M[1][3]);
}
FRotator ThreeAxisRot(double r11, double r12, double r21, double r31, double r32)
{
	return FRotator(atan2(r31, r32), asin(r21), atan2(r11, r12));
}
FRotator Quaternion2Euler(FQuat q)
{
	return ThreeAxisRot(-2 * (q.X*q.Y - q.W*q.Z),
		q.W*q.W - q.X*q.X + q.Y*q.Y - q.Z*q.Z,
		2 * (q.Y*q.Z + q.W*q.X),
		-2 * (q.X*q.Z - q.W*q.Y),
		q.W*q.W - q.X*q.X - q.Y*q.Y + q.Z*q.Z);
}
FRotator FMatrixToRotator(FMatrix Pose)
{
	if (!((FMath::Abs(1.f - Pose.GetScaledAxis(EAxis::X).SizeSquared()) <= KINDA_SMALL_NUMBER) && (FMath::Abs(1.f - Pose.GetScaledAxis(EAxis::Y).SizeSquared()) <= KINDA_SMALL_NUMBER) && (FMath::Abs(1.f - Pose.GetScaledAxis(EAxis::Z).SizeSquared()) <= KINDA_SMALL_NUMBER)))
	{
		Pose.RemoveScaling(KINDA_SMALL_NUMBER);
	}
	FQuat Orientation(Pose);
	FRotator ZXY = Quaternion2Euler(Orientation);
	return FRotator(FMath::RadiansToDegrees(ZXY.Yaw),
		-FMath::RadiansToDegrees(ZXY.Pitch),
		FMath::RadiansToDegrees(ZXY.Roll));
}
#pragma endregion

FVector ViveSR_DualCameraImageCapture::GetDistortedLocalPosition(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return Position(DistortedPoseLeft);
	else if (eye == DualCameraIndex::RIGHT)
		return Position(DistortedPoseRight);
	else
		return FVector(0, 0, 0);
}
FRotator ViveSR_DualCameraImageCapture::GetDistortedLocalRotation(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return FMatrixToRotator(DistortedPoseLeft);
	else if (eye == DualCameraIndex::RIGHT)
		return FMatrixToRotator(DistortedPoseRight);
	else
		return FRotator(0, 0, 0);
}
FVector ViveSR_DualCameraImageCapture::GetUndistortedLocalPosition(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return Position(UndistortedPoseLeft);
	else if (eye == DualCameraIndex::RIGHT)
		return Position(UndistortedPoseRight);
	else
		return FVector(0, 0, 0);
}
FRotator ViveSR_DualCameraImageCapture::GetUndistortedLocalRotation(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return FMatrixToRotator(UndistortedPoseLeft);
	else if (eye == DualCameraIndex::RIGHT)
		return FMatrixToRotator(UndistortedPoseRight);
	else
		return FRotator(0, 0, 0);
}
FVector ViveSR_DualCameraImageCapture::GetDepthLocalPosition()
{
	return Position(DepthPose);
}
FRotator ViveSR_DualCameraImageCapture::GetDepthLocalRotation()
{
	return FMatrixToRotator(DepthPose);
}

#pragma region Depth LinkMethod / Commands / Parameters
int ViveSR_DualCameraImageCapture::EnableDepthProcess(bool active)
{
	int result = ViveSR_ChangeModuleLinkStatus(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, FSRWorksModule::Get().MODULE_ID_DEPTH, DepthProcessing ? ViveSR::SRWorks_Link_Method::SR_NONE : ViveSR::SRWorks_Link_Method::SR_ACTIVE);
	if (result == ViveSR::SR_Error::WORK) DepthProcessing = active;
	return result;
}
int ViveSR_DualCameraImageCapture::EnableDepthRefinement(bool active)
{
	int result = ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Cmd::ENABLE_REFINEMENT, active);
	if (result == ViveSR::SR_Error::WORK) DepthRefinement = active;
	return result;
}
int ViveSR_DualCameraImageCapture::EnableDepthEdgeEnhance(bool active)
{
	int result = ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Cmd::ENABLE_EDGE_ENHANCE, active);
	if (result == ViveSR::SR_Error::WORK) DepthEdgeEnhance = active;
	return result;
}
int ViveSR_DualCameraImageCapture::SetDepthCase(ViveSR::Depth::DepthCase depthCase)
{
	int result = ViveSR::SR_Error::FAILED;
	if (FSRWorksModule::Get().IsFrameworkStarted) {
		result = ViveSR_SetCommandInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Cmd::CHANGE_DEPTH_CASE, (int)depthCase);
	}
	else {
		result = ViveSR_SetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::DEPTH_USING_CASE, (int)depthCase);
	}
	if (result == ViveSR::SR_Error::WORK) DepthCase = depthCase;
	return result;
}
int ViveSR_DualCameraImageCapture::SetDepthConfidenceThreshold(float value)
{
	int result = ViveSR_SetParameterFloat(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::CONFIDENCE_THRESHOLD, value);
	if (result == ViveSR::SR_Error::WORK) DepthConfidenceThreshold = value;
	return result;
}
int ViveSR_DualCameraImageCapture::SetDepthDenoiseGuidedFilter(int value)
{
	int result = ViveSR_SetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::DENOISE_GUIDED_FILTER, value);
	if (result == ViveSR::SR_Error::WORK) DepthDenoiseGuidedFilter = value;
	return result;
}
int ViveSR_DualCameraImageCapture::SetDepthDenoiseMedianFilter(int value)
{
	int result = ViveSR_SetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::DENOISE_MEDIAN_FILTER, value);
	if (result == ViveSR::SR_Error::WORK) DepthDenoiseMedianFilter = value;
	return result;
}
#pragma endregion

#pragma region Camera Commands / Parameters
int ViveSR_DualCameraImageCapture::GetCameraQualityInfo(ViveSR::SeeThrough::Param item, ParamInfo *paramInfo)
{
	if (item < ViveSR::SeeThrough::CAMERA_BRIGHTNESS || item > ViveSR::SeeThrough::CAMERA_IRIS) return ViveSR::SR_Error::INVAILD_INPUT;
	return ViveSR_GetParameterStruct(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, item, (void**)(&paramInfo));
}
int ViveSR_DualCameraImageCapture::SetCameraQualityInfo(ViveSR::SeeThrough::Param item, ParamInfo *paramInfo)
{
	if (item < ViveSR::SeeThrough::CAMERA_BRIGHTNESS || item > ViveSR::SeeThrough::CAMERA_IRIS) return ViveSR::SR_Error::INVAILD_INPUT;
	return ViveSR_SetParameterStruct(FSRWorksModule::Get().MODULE_ID_SEETHROUGH, item, (void*)(paramInfo));
}
#pragma endregion