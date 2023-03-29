// Fill out your copyright notice in the Description page of Project Settings.

#include "SRWorksFunctionLibrary.h"
#include "SRWorks.h"
#include "ViveSR_API.h"
#include "ViveSR_Enums.h"
#include "ViveSR_UEnums.h"
#include "ViveSR_DualCameraImageCapture.h"
#include "ViveSR_DualCameraDepthExtra.h"
#include "ViveSR_RigidReconstruction.h"
#include "Paths.h"

#pragma region Framework
FString USRWorksFunctionLibrary::GetSRWorksVersion()
{
	return ViveSR_GetVersion();
}
int USRWorksFunctionLibrary::EnableLog(bool active)
{
    return ViveSR_EnableLog(active);
}
int USRWorksFunctionLibrary::SetLogLevel(SRLogLevel level)
{
	return ViveSR_SetLogLevel((int)level);
}
#pragma endregion

#pragma region DualCamera
bool USRWorksFunctionLibrary::GetDepthProcessing()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthProcessing;
}
int USRWorksFunctionLibrary::EnableDepthProcess(bool active)
{
	return ViveSR_DualCameraImageCapture::Instance()->EnableDepthProcess(active);
}
bool USRWorksFunctionLibrary::GetDepthRefinement()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthRefinement;
}
int USRWorksFunctionLibrary::EnableDepthRefinement(bool active)
{
	return ViveSR_DualCameraImageCapture::Instance()->EnableDepthRefinement(active);
}
bool USRWorksFunctionLibrary::GetDepthEdgeEnhance()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthEdgeEnhance;
}
int USRWorksFunctionLibrary::EnableDepthEdgeEnhance(bool active)
{
	return ViveSR_DualCameraImageCapture::Instance()->EnableDepthEdgeEnhance(active);
}
UDepthCase USRWorksFunctionLibrary::GetDepthCase()
{
	return (UDepthCase)ViveSR_DualCameraImageCapture::Instance()->DepthCase;
}
int USRWorksFunctionLibrary::SetDepthCase(UDepthCase depthCase)
{
	return ViveSR_DualCameraImageCapture::Instance()->SetDepthCase((ViveSR::Depth::DepthCase)depthCase);
}
float USRWorksFunctionLibrary::GetDepthConfidenceThreshold()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthConfidenceThreshold;
}
int USRWorksFunctionLibrary::SetDepthConfidenceThreshold(float value)
{
	return ViveSR_DualCameraImageCapture::Instance()->SetDepthConfidenceThreshold(value);
}
int USRWorksFunctionLibrary::GetDepthDenoiseG()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthDenoiseGuidedFilter;
}
int USRWorksFunctionLibrary::SetDepthDenoiseG(int value)
{
	return ViveSR_DualCameraImageCapture::Instance()->SetDepthDenoiseGuidedFilter(value);
}
int USRWorksFunctionLibrary::GetDepthDenoiseM()
{
	return ViveSR_DualCameraImageCapture::Instance()->DepthDenoiseMedianFilter;
}
int USRWorksFunctionLibrary::SetDepthDenoiseM(int value)
{
	return ViveSR_DualCameraImageCapture::Instance()->SetDepthDenoiseMedianFilter(value);
}
int USRWorksFunctionLibrary::GetCameraQualityInfo(CameraQuality item, FParamInfo &paramInfo)
{
	ParamInfo param;
	int result = ViveSR_DualCameraImageCapture::Instance()->GetCameraQualityInfo((ViveSR::SeeThrough::Param)item, &param);
	paramInfo.Status = param.nStatus;
	paramInfo.DefaultValue = param.nDefaultValue;
	paramInfo.Min = param.nMin;
	paramInfo.Max = param.nMax;
	paramInfo.Step = param.nStep;
	paramInfo.DefaultMode = param.nDefaultMode;
	paramInfo.Value = param.nValue;
	paramInfo.Mode = param.nMode;
	return result;
}
int USRWorksFunctionLibrary::SetCameraQualityInfo(CameraQuality item, FParamInfo paramInfo)
{
	ParamInfo param;
	param.nStatus = paramInfo.Status;
	param.nDefaultValue = paramInfo.DefaultValue;
	param.nMin = paramInfo.Min;
	param.nMax = paramInfo.Max;
	param.nStep = paramInfo.Step;
	param.nDefaultMode = paramInfo.DefaultMode;
	param.nValue = paramInfo.Value;
	param.nMode = paramInfo.Mode;
	return ViveSR_DualCameraImageCapture::Instance()->SetCameraQualityInfo((ViveSR::SeeThrough::Param)item, &param);
}
#pragma endregion

#pragma region RigidReconstruction
void USRWorksFunctionLibrary::StartScanning()
{
	ViveSR_RigidReconstruction::Instance()->StartScanning();
}
void USRWorksFunctionLibrary::StopScanning()
{
	ViveSR_RigidReconstruction::Instance()->StopScanning();
}
bool USRWorksFunctionLibrary::GetIsScanning()
{
	return ViveSR_RigidReconstruction::Instance()->IsScanning;
}
bool USRWorksFunctionLibrary::GetIsExporting()
{
	return ViveSR_RigidReconstruction::Instance()->IsExporting;
}
void USRWorksFunctionLibrary::ExportModel(FString filename)
{
	ViveSR_RigidReconstruction::Instance()->ExportModel(filename);
}
int USRWorksFunctionLibrary::GetExportProgress()
{
	return ViveSR_RigidReconstruction::Instance()->GetExportProgress();
}
void USRWorksFunctionLibrary::ExportSceneUnderstandingInfo(FString filename)
{
	ViveSR_RigidReconstruction::Instance()->ExportSceneUnderstandingInfo(filename);
}
#pragma endregion

bool USRWorksFunctionLibrary::FileExists(const FString &InPath)
{
	return FPaths::FileExists(InPath);
}
FString USRWorksFunctionLibrary::ProjectUserDir()
{
	return FPaths::ProjectUserDir();
}