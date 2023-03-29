// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViveSR_UEnums.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SRWorksFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FParamInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Status;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 DefaultValue;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Min;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Max;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Step;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 DefaultMode;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Value;
	UPROPERTY(BlueprintReadWrite, Category = "ParamInfo Struct") int32 Mode;

	FParamInfo() {
	}
};

UCLASS()
class SRWORKS_API USRWorksFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Get the version of  SRWorks. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|Framework")
	static FString GetSRWorksVersion();
	
	/** Enable the log system of SRWorks and return the error code of result. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|Framework")
	static int EnableLog(bool active);

	/** Set the level of log system of SRWorks and return the error code of result. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|Framework")
	static int SetLogLevel(SRLogLevel level);

	/** Return true if the depth process is active. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static bool GetDepthProcessing();

	/** Eanble or disable the depth process and return the error code of result. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int EnableDepthProcess(bool active);

	/** Return true if the depth refinement is active. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static bool GetDepthRefinement();

	/** Eanble or disable the depth refinement and return the error code of result. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int EnableDepthRefinement(bool active);

	/** Return true if the depth edge enhance is active. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static bool GetDepthEdgeEnhance();

	/** Eanble or disable the depth edge enhance and return the error code of result. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int EnableDepthEdgeEnhance(bool active);

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static UDepthCase GetDepthCase();

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int SetDepthCase(UDepthCase depthCase);

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static float GetDepthConfidenceThreshold();

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int SetDepthConfidenceThreshold(float value);

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int GetDepthDenoiseG();

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int SetDepthDenoiseG(int value);

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int GetDepthDenoiseM();

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|Depth")
	static int SetDepthDenoiseM(int value);

	/** Get camera quality information and return the error code of result */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera")
	static int GetCameraQualityInfo(CameraQuality item, FParamInfo &paramInfo);

	/** Set camera quality information and return the error code of result */
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera")
	static int SetCameraQualityInfo(CameraQuality item, FParamInfo paramInfo);

	/** Start to scan the real scene. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static void StartScanning();

	/** Stop scaning the real scene. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static void StopScanning();

	/** Return true if the rigid reconstruction module is scaning. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static bool GetIsScanning();

	/** Return true if there are models are being exported. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static bool GetIsExporting();

	/** Start to export current scene and create an obj file */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static void ExportModel(FString filename);

	/** Get the percentage of current export progress. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static int GetExportProgress();

	/** Start to export current scene and create an obj file */
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	static void ExportSceneUnderstandingInfo(FString filename);

	/** Call the FPaths::FileExists and return true if this file was found, false otherwise. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|Utility|File")
	static bool FileExists(const FString &InPath);

	/** Call FPaths::ProjectUserDir and returns the root directory for user-specific game files. */
	UFUNCTION(BlueprintCallable, category = "ViveSR|Utility|File")
	static FString ProjectUserDir();
};