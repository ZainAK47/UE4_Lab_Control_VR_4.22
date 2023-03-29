// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViveSR_API.h"
#include "ViveSR_UEnums.h"
#include "Runnable.h"
#include "ThreadSafeCounter.h"

/**
*
*/
class ViveSR_RigidReconstruction : public FRunnable
{
public:
	ViveSR_RigidReconstruction();
	~ViveSR_RigidReconstruction();
	static ViveSR_RigidReconstruction* Instance();

	void Initial();
	void Release();

	virtual bool Init(); // FRunnable interface
	virtual uint32 Run();// FRunnable interface
	virtual void Stop(); // FRunnable interface

	int GetRigidReconstructionIntParameter(int type);
	void AllocOutputDataMemory();
	void FreeOutputDataMemory();
	bool GetRigidReconstructionFrame(int &frame);
	bool GetRigidReconstructionData(int &frame, TArray<float> &pose, int &verticesNum, TArray<float> &verticesBuff, int &vertStrideInFloat, int &indicesNum, TArray<int> &indicesBuff);
	void ExportModel(FString filename);
	void GetExportProgress(int &stage, int &percentage);
	int GetExportProgress();
	void EnableLiveMeshExtraction(bool enable);
	void SetLiveMeshExtractionMode(ReconstructionLiveMeshExtractMode mode);
	void StartScanning();
	void StopScanning();
	void ExportSceneUnderstandingInfo(FString filename);
	//bool LoadSceneUnderstandingInfo(FString folderPath, FString filename);
	int EnableSceneUnderstandingSceneUnderstanding(bool isSceneUnderstandingEnabled);
	//int EnableSceneUnderstandingVisualization(bool enable);
	//void ResetSceneUnderstandingProgress();
	void SetSceneUnderstandingPreviewTarget(int type);
	void GetSceneUnderstandingProgress(int &stage, int &percentage);
	int GetSceneUnderstandingProgress();
	int GetSceneUnderstandingConfig(SceneUnderstandingConfig *config);
	int SetSceneUnderstandingConfig(SceneUnderstandingConfig *config);

	bool ExportAdaptiveMesh;
	float AdaptiveMaxGridSize;
	float AdaptiveMinGridSize;
	float AdaptiveErrorThres;
	bool IsScanning = false;
	bool HasScanned = true;
	bool IsExporting = false;
	int ExportStage;
	int ExportPercentage;

	bool IsEnabledSceneUnderstandingSceneUnderstanding;
	bool IsEnabledSceneUnderstandingVisualization;
	int SceneUnderstandingPreviewTargetType;
	int SceneUnderstandingProcessingStage;
	int SceneUnderstandingProcessingProgressBar;
private:
	static ViveSR_RigidReconstruction *Runnable;
	bool IsInitial = false;

	const int NUM_DATA_POINTCLOUD = 7;
	DataInfo *DataInfoPointCloud = new DataInfo[NUM_DATA_POINTCLOUD];;
	bool InitialPointCloudPtrSize = false;

	TArray<float> OutVertex;
	TArray<int> OutIndex;
	TArray<float> RawTrackedPose;

	int FrameSeq;
	int VertNum;
	int IdxNum;
	int VertStrideInByte;
	int LastFrameSeq = -1;

	void UpdateFrame();
	void UpdateData();
	//void UpdateExportProgress(int stage, int percentage);

	//Thread to run the worker FRunnable on
	FRunnableThread* Thread;
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
	FThreadSafeCounter WorkerCounter;
	FCriticalSection m_mutex;
};
