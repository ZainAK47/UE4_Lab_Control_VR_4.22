// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_RigidReconstruction.h"
#include "ViveSR_Enums.h"
#include "SRWorks.h"
#include "Engine.h"
#include "ModuleManager.h"

#pragma region singleton 
ViveSR_RigidReconstruction* ViveSR_RigidReconstruction::Runnable = nullptr;
ViveSR_RigidReconstruction::ViveSR_RigidReconstruction()
{
}
ViveSR_RigidReconstruction::~ViveSR_RigidReconstruction()
{
}
ViveSR_RigidReconstruction* ViveSR_RigidReconstruction::Instance()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new ViveSR_RigidReconstruction();
	}
	return Runnable;
}
void ViveSR_RigidReconstruction::Initial()
{
	FString ThreadName(FString::Printf(TEXT("ViveSR_RigidReconstruction%i"), Runnable->WorkerCounter.Increment()));
	Runnable->Thread = FRunnableThread::Create(Runnable, *ThreadName, 0, TPri_BelowNormal);
	Runnable->Thread->Suspend(true);
	IsInitial = true;
	InitialPointCloudPtrSize = false;
	IsScanning = false;
	HasScanned = true;
	IsExporting = false;
}
void ViveSR_RigidReconstruction::Release()
{
	if (!IsInitial) return;
	if (Runnable)
	{
		Runnable->Stop();
		Runnable->Thread->Suspend(false);
		Runnable->Thread->WaitForCompletion();
		Runnable->Thread->Kill();
		delete Runnable;
		Runnable = nullptr;
	}
	IsInitial = false;
}
#pragma endregion

void ViveSR_RigidReconstruction::UpdateFrame()
{
	int sr_module_id = FSRWorksModule::GetModuleChecked().MODULE_ID_RIGID_RECONSTRUCTION;
	int result = ViveSR::SR_Error::FAILED;

	if (!InitialPointCloudPtrSize) {
		result = ViveSR_GetMultiDataSize(sr_module_id, DataInfoPointCloud, NUM_DATA_POINTCLOUD);
		InitialPointCloudPtrSize = result == ViveSR::SR_Error::WORK;
	}
	else {
		result = ViveSR_GetMultiData(sr_module_id, DataInfoPointCloud, 1); // Only FrameSeq
		if (result != ViveSR::SR_Error::WORK) {
			UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Update POINTCLOUD FrameSeq Error %d"), result);
		}
	}
}
void ViveSR_RigidReconstruction::UpdateData()
{
	int sr_module_id = FSRWorksModule::GetModuleChecked().MODULE_ID_RIGID_RECONSTRUCTION;
	int result = ViveSR::SR_Error::FAILED;

	result = ViveSR_GetMultiData(sr_module_id, DataInfoPointCloud, NUM_DATA_POINTCLOUD);
	if (result != ViveSR::SR_Error::WORK)
	{
		VertNum = 0;
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] GetRigidReconstructionData %d"), result);
	}
}

#pragma region Public APIs
int ViveSR_RigidReconstruction::GetRigidReconstructionIntParameter(int type)
{
	int ret = -1;
	if (ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, type, &ret) != (int)ViveSR::SR_Error::WORK)
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] [RigidReconstruction] GetRigidReconstructionIntParameter Failed"));
	return ret;
}
void ViveSR_RigidReconstruction::AllocOutputDataMemory()
{
	OutVertex.Init(0, 8 * 2500000);
	OutIndex.Init(0, 2500000);
	RawTrackedPose.Init(0, 16);
	ExportAdaptiveMesh = true;
	AdaptiveMaxGridSize = 64;
	AdaptiveMinGridSize = 4;
	AdaptiveErrorThres = 0.4f;

	DataInfoPointCloud = new DataInfo[NUM_DATA_POINTCLOUD];
	for (int i = 0; i < NUM_DATA_POINTCLOUD; i++) DataInfoPointCloud[i].mask = i;
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::FRAME_SEQ].ptr	= &FrameSeq;
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::POSEMTX44].ptr	= RawTrackedPose.GetData();
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::NUM_VERTICES].ptr = &VertNum;
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::BYTEPERVERT].ptr	= &VertStrideInByte;
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::VERTICES].ptr		= OutVertex.GetData();
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::NUM_INDICES].ptr	= &IdxNum;
	DataInfoPointCloud[ViveSR::RigidReconstruction::DataMask::INDICES].ptr		= OutIndex.GetData();
}
void  ViveSR_RigidReconstruction::FreeOutputDataMemory() {
	if (DataInfoPointCloud) delete[] DataInfoPointCloud;
}
void UpdateExportProgress(int stage, int percentage)
{
	if (stage == ViveSR::RigidReconstruction::ExportStage::EXTRACT_MODEL) ViveSR_RigidReconstruction::Instance()->ExportStage = 0;
	else if (stage == ViveSR::RigidReconstruction::ExportStage::COMPACT_TEXTURE) ViveSR_RigidReconstruction::Instance()->ExportStage = 1;
	else if (stage == ViveSR::RigidReconstruction::ExportStage::SAVE_MODEL) ViveSR_RigidReconstruction::Instance()->ExportStage = 2;
	else if (stage == ViveSR::RigidReconstruction::ExportStage::EXTRACT_COLLIDER) ViveSR_RigidReconstruction::Instance()->ExportStage = 3;
	ViveSR_RigidReconstruction::Instance()->ExportPercentage = percentage;
	//if (stage == ViveSR::RigidReconstruction::ExportStage::EXTRACT_MODEL) {
	//	UE_LOG(LogTemp, Warning, TEXT("Extracting Model: %d"), percentage);
	//}
	//else if (stage == ViveSR::RigidReconstruction::ExportStage::COMPACT_TEXTURE) {
	//	UE_LOG(LogTemp, Warning, TEXT("Compacting Textures: %d"), percentage);
	//}
	//else if (stage == ViveSR::RigidReconstruction::ExportStage::EXTRACT_COLLIDER) {
	//	UE_LOG(LogTemp, Warning, TEXT("Extracting Collider: %d"), percentage);
	//}
	//else if (stage == ViveSR::RigidReconstruction::ExportStage::SAVE_MODEL)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Saving Model: %d"), percentage);
	//}
	if (ViveSR_RigidReconstruction::Instance()->ExportStage == 3 && ViveSR_RigidReconstruction::Instance()->ExportPercentage == 100)
	{
		ViveSR_RigidReconstruction::Instance()->StopScanning();
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] [RigidReconstruction] Finish Exporting"));
	}
}
void ViveSR_RigidReconstruction::ExportModel(FString filename)
{
	ExportStage = 0;
	ExportPercentage = 0;
	IsExporting = true;

	ViveSR_SetParameterString(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::FILE_EXPORT_ROOT_PATH, (char*)StringCast<ANSICHAR>(*FPaths::ProjectUserDir()).Get());

	ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::EXPORT_ADAPTIVE_MODEL, ExportAdaptiveMesh);
	if (ExportAdaptiveMesh)
	{
		ViveSR_SetParameterFloat(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::ADAPTIVE_MAX_GRID, AdaptiveMaxGridSize * 0.05f);   // cm to m
		ViveSR_SetParameterFloat(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::ADAPTIVE_MIN_GRID, AdaptiveMinGridSize * 0.01f);
		ViveSR_SetParameterFloat(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::ADAPTIVE_ERROR_THRES, AdaptiveErrorThres);
	}
	ViveSR_RegisterCallback(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Callback::EXPORT_PROGRESS, UpdateExportProgress);
	ViveSR_SetCommandString(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::EXPORT_MODEL_FOR_UNITY, (char*)StringCast<ANSICHAR>(*filename).Get());
}
void ViveSR_RigidReconstruction::GetExportProgress(int &stage, int &percentage)
{
	stage = ExportStage;
	percentage = ExportPercentage;
}
int ViveSR_RigidReconstruction::GetExportProgress()
{
	return ExportStage * 25 + (int)((float)ExportPercentage * 0.25f);
}
void ViveSR_RigidReconstruction::EnableLiveMeshExtraction(bool enable)
{
	ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::EXTRACT_POINT_CLOUD, enable);
}
void ViveSR_RigidReconstruction::SetLiveMeshExtractionMode(ReconstructionLiveMeshExtractMode mode)
{
	ViveSR_SetCommandInt(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::EXTRACT_VERTEX_NORMAL, (int)mode);
}
void ViveSR_RigidReconstruction::StartScanning()
{
	if (IsScanning) return;
	ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::START, true);
	ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, 53927676, true);
	IsScanning = true;
	Runnable->Thread->Suspend(false);
}
void ViveSR_RigidReconstruction::StopScanning()
{
	if (!IsScanning) return;
	IsScanning = false;
	IsExporting = false;
	ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::STOP, true);
	Runnable->Thread->Suspend(true);
}
bool ViveSR_RigidReconstruction::GetRigidReconstructionFrame(int &frame)
{
	if (Runnable) {
		Runnable->m_mutex.Lock();
		frame = Runnable->FrameSeq;
		Runnable->m_mutex.Unlock();
	}
	return true;
}
bool ViveSR_RigidReconstruction::GetRigidReconstructionData(int &frame, TArray<float> &pose, int &verticesNum, TArray<float> &verticesBuff, int &vertStrideInFloat, int &indicesNum, TArray<int> &indicesBuff)
{
	if (Runnable) {
		if (Runnable->VertNum > 0)
		{
			Runnable->m_mutex.Lock();
			frame = Runnable->FrameSeq;
			pose = Runnable->RawTrackedPose;
			verticesNum = Runnable->VertNum;
			verticesBuff = Runnable->OutVertex;
			vertStrideInFloat = Runnable->VertStrideInByte;
			indicesNum = Runnable->IdxNum;
			indicesBuff = Runnable->OutIndex;
			Runnable->m_mutex.Unlock();
			return true;
		}
	}
	return false;
}
void UpdateSceneUnderstandingProgress(int stage, int percentage)
{
	switch (stage)
	{
	case ViveSR::RigidReconstruction::ExportStage::SCENE_UNDERSTANDING_PASS_1:
		ViveSR_RigidReconstruction::Instance()->SceneUnderstandingProcessingStage = 0;
		break;
	case ViveSR::RigidReconstruction::ExportStage::SCENE_UNDERSTANDING_PASS_2:
		ViveSR_RigidReconstruction::Instance()->SceneUnderstandingProcessingStage = 1;
		break;
	default:
		break;
	}
	ViveSR_RigidReconstruction::Instance()->SceneUnderstandingProcessingProgressBar = percentage;
	UE_LOG(LogTemp, Warning, TEXT("[ViveSR] [SceneUnderstanding Scene Understanding] Stage : %d  Progress : %d"), ViveSR_RigidReconstruction::Instance()->SceneUnderstandingProcessingStage, percentage);
	if (stage == 1 && percentage == 100)
		UE_LOG(LogTemp, Warning, TEXT("[ViveSR] [SceneUnderstanding Scene Understanding] Finished"));
}
void ViveSR_RigidReconstruction::ExportSceneUnderstandingInfo(FString filename)
{
	SceneUnderstandingProcessingProgressBar = 0;
	SceneUnderstandingProcessingStage = 0;
	ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::SCENE_UNDERSTANDING_ENABLE, true);
	// scene understanding is incompatible with these following functionalities
	ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::ENABLE_SECTOR_GROUPING, false);
	ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::ENABLE_FRUSTUM_CULLING, false);

	ViveSR_RegisterCallback(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Callback::SCENE_UNDERSTANDING_PROGRESS, UpdateSceneUnderstandingProgress);
	ViveSR_SetCommandString(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Cmd::EXPORT_SCENE_UNDERSTANDING_FOR_UNITY, (char*)StringCast<ANSICHAR>(*filename).Get());
}
int ViveSR_RigidReconstruction::EnableSceneUnderstandingSceneUnderstanding(bool isSceneUnderstandingEnabled)
{
	IsEnabledSceneUnderstandingSceneUnderstanding = isSceneUnderstandingEnabled;
	return ViveSR_RegisterCallback(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION,
		ViveSR::RigidReconstruction::Callback::SCENE_UNDERSTANDING_PROGRESS,
		UpdateSceneUnderstandingProgress);
}
void ViveSR_RigidReconstruction::SetSceneUnderstandingPreviewTarget(int type)
{
	int result = ViveSR_SetParameterInt(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::SCENE_UNDERSTANDING_MACHINE_VISION, type);
	if (result != ViveSR::SR_Error::WORK) UE_LOG(LogTemp, Warning, TEXT("[ViveSR] [SceneUnderstanding Scene Understanding] Set preview target failed"));
	SceneUnderstandingPreviewTargetType = type;
}
void ViveSR_RigidReconstruction::GetSceneUnderstandingProgress(int &stage, int &percentage)
{
	stage = SceneUnderstandingProcessingStage;
	percentage = SceneUnderstandingProcessingProgressBar;
}
int ViveSR_RigidReconstruction::GetSceneUnderstandingProgress()
{
	const float ProportionStage1 = 0.8f; // hardcoded
	const float ProportionStage2 = 0.2f;
	int percentage = 0;
	if (SceneUnderstandingProcessingStage == 0) percentage = (int)(ProportionStage1 * SceneUnderstandingProcessingProgressBar);
	if (SceneUnderstandingProcessingStage == 1) percentage = (int)(ProportionStage1 * 100 + ProportionStage2 * SceneUnderstandingProcessingProgressBar);
	return percentage;
}
int ViveSR_RigidReconstruction::GetSceneUnderstandingConfig(SceneUnderstandingConfig *config)
{
	return ViveSR_GetParameterStruct(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION,
		ViveSR::RigidReconstruction::Param::SCENE_UNDERSTANDING_CONFIG,
		(void**)(&config));
}
int ViveSR_RigidReconstruction::SetSceneUnderstandingConfig(SceneUnderstandingConfig *config)
{
	return ViveSR_SetParameterStruct(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION,
		ViveSR::RigidReconstruction::Param::SCENE_UNDERSTANDING_CONFIG,
		(void*)(config));
}
#pragma endregion

#pragma region Thread Lifecycle
bool ViveSR_RigidReconstruction::Init()
{
	StopTaskCounter.Reset();
	return true;
}
uint32 ViveSR_RigidReconstruction::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

	while (StopTaskCounter.GetValue() == 0)
	{
		m_mutex.Lock();
		if (IsScanning)
		{
			UpdateFrame();
			if (LastFrameSeq != FrameSeq) UpdateData();
			LastFrameSeq = FrameSeq;
		}
		m_mutex.Unlock();
		FPlatformProcess::Sleep(0.01667);
	}
	return 0;
}
void ViveSR_RigidReconstruction::Stop()
{
	StopTaskCounter.Increment();
}
#pragma endregion