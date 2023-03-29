// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ViveSR_API.h"
#include "ViveSR_Enums.h"
#include "ViveSR_UEnums.h"
/**
 * 
 */

class ViveSR_DualCameraImageCapture
{
public:
	ViveSR_DualCameraImageCapture();
	~ViveSR_DualCameraImageCapture();
	static ViveSR_DualCameraImageCapture* Instance();
	static void DestroyImageCapture();

	CameraParams CameraParameters;
	int DistortedImageWidth = 612;
	int DistortedImageHeight = 460;
	int DistortedImageChannel = 4;
	int UndistortedImageWidth = 1150;
	int UndistortedImageHeight = 750;
	int UndistortedImageChannel = 4;
	int DepthImageWidth = 640;
	int DepthImageHeight = 480;
	int DepthImageChannel = 1;
	int DepthDataSize = 4;
	unsigned int DistortedFrameIndex = 0;
	unsigned int UndistortedFrameIndex = 0;
	unsigned int DepthFrameIndex = 0;
	unsigned int DistortedTimeIndex = 0;
	unsigned int UndistortedTimeIndex = 0;
	unsigned int DepthTimeIndex = 0;
	float RawDistortedPoseLeft[16], RawUndistortedPoseLeft[16], RawDepthPose[16];

	double UndistortedCx_L;
	double UndistortedCy_L;
	double UndistortedCx_R;
	double UndistortedCy_R;
	float* UndistortionMap_L;
	float* UndistortionMap_R;

	FMatrix DistortedPoseLeft, DistortedPoseRight;
	FMatrix UndistortedPoseLeft, UndistortedPoseRight;
	FMatrix DepthPose;
	FVector GetDistortedLocalPosition(DualCameraIndex eye);
	FRotator GetDistortedLocalRotation(DualCameraIndex eye);
	FVector GetUndistortedLocalPosition(DualCameraIndex eye);
	FRotator GetUndistortedLocalRotation(DualCameraIndex eye);
	FVector GetDepthLocalPosition();
	FRotator GetDepthLocalRotation();

	void Initial();
	void Release();
	void GetDistortedTexture(UTexture2D*& imageLeft, UTexture2D*& imageRight, int& frameIndex, int& timeIndex);
	void GetUndistortedTexture(UTexture2D*& imageLeft, UTexture2D*& imageRight, int& frameIndex, int& timeIndex);
	void GetDepthTexture(UTexture2D*& imageDepth, int& frameIndex, int& timeIndex);

	void UpdateDistortedImage();
	void UpdateUndistortedImage();
	void UpdateDepthImage();

	bool DepthProcessing = false;
	bool DepthRefinement = false;
	bool DepthEdgeEnhance = false;
	ViveSR::Depth::DepthCase DepthCase;
	float DepthConfidenceThreshold;
	float DepthDenoiseGuidedFilter;
	float DepthDenoiseMedianFilter;
	int EnableDepthProcess(bool active);
	int EnableDepthRefinement(bool active);
	int EnableDepthEdgeEnhance(bool active);
	int SetDepthCase(ViveSR::Depth::DepthCase depthCase);
	int SetDepthConfidenceThreshold(float value);
	int SetDepthDenoiseGuidedFilter(int value);
	int SetDepthDenoiseMedianFilter(int value);

	int GetCameraQualityInfo(ViveSR::SeeThrough::Param item, ParamInfo *paramInfo);
	int SetCameraQualityInfo(ViveSR::SeeThrough::Param item, ParamInfo *paramInfo);

private:
	static ViveSR_DualCameraImageCapture *Mgr;

	void GetParameters();
	void TextureRegionCleanUp(uint8* rawData, const FUpdateTextureRegion2D* region) {};
	TFunction<void(uint8*, const FUpdateTextureRegion2D*)> texCleanUpFP = [this](uint8* rawData, const FUpdateTextureRegion2D* region) { TextureRegionCleanUp(rawData, region); };

	UTexture2D* textureDistortedLeft = nullptr;
	UTexture2D* textureDistortedRight = nullptr;
	UTexture2D* textureUndistortedLeft = nullptr;
	UTexture2D* textureUndistortedRight = nullptr;
	UTexture2D* textureDepth = nullptr;

	FUpdateTextureRegion2D* textureDistortedRegion = nullptr;
	FUpdateTextureRegion2D* textureUndistortedRegion = nullptr;
	FUpdateTextureRegion2D* textureDepthRegion = nullptr;

	DataInfo *DataInfoDistorted = nullptr, *FrameInfoDistorted = nullptr;
	DataInfo *DataInfoUndistorted = nullptr, *FrameInfoUndistorted = nullptr;
	DataInfo *DataInfoDepth = nullptr, *FrameInfoDepth = nullptr;
	bool InitialDistortedPtrSize = false, InitialUndistortedPtrSize = false, InitialDepthPtrSize = false;
	const int NUM_DATA_DISTORTED = 7, NUM_DATA_UNDISTORTED = 7, NUM_DATA_DEPTH = 5;
	unsigned int LastDistortedFrameIndex = 0, LastUndistortedFrameIndex = 0, LastDepthFrameIndex = 0;

	uint8 *RawDistortedLeftBGRA = nullptr, *RawDistortedRightBGRA = nullptr, *RawAllDistortedData;
	uint8 *RawUndistortedLeftBGRA = nullptr, *RawUndistortedRightBGRA = nullptr, *RawAllUndistortedData;
	uint8 *RawDepth = nullptr;

	FVector Position(FMatrix m);
};
