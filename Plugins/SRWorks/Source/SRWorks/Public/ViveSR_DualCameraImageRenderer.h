// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ViveSR_UEnums.h"
#include "ViveSR_DualCameraImageRenderer.generated.h"

UCLASS()
class SRWORKS_API AViveSR_DualCameraImageRenderer : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AViveSR_DualCameraImageRenderer();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVROrigin(AActor* vrOrigin) { VROrigin = vrOrigin; }
	void Initial();
	void Release();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") DualCameraDisplayMode dualCameraDisplayMode = DualCameraDisplayMode::MIX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViveSR|DualCamera|ImageRenderer") bool UpdateDistortedMaterial = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViveSR|DualCamera|ImageRenderer") bool UpdateUndistortedMaterial = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ViveSR|DualCamera|ImageRenderer") bool UpdateDepthMaterial = false;
	UPROPERTY(EditAnywhere, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInterface* DistortedMaterialInstanceLeft;
	UPROPERTY(EditAnywhere, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInterface* DistortedMaterialInstanceRight;
	UPROPERTY(EditAnywhere, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInterface* UndistortedMaterialInstanceLeft;
	UPROPERTY(EditAnywhere, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInterface* UndistortedMaterialInstanceRight;
	UPROPERTY(EditAnywhere, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInterface* DepthMaterialInstance;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") TArray<UMaterialInstanceDynamic*> DistortedDynamicMaterialsLeft;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") TArray<UMaterialInstanceDynamic*> DistortedDynamicMaterialsRight;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") TArray<UMaterialInstanceDynamic*> UndistortedDynamicMaterialsLeft;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") TArray<UMaterialInstanceDynamic*> UndistortedDynamicMaterialsRight;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") TArray<UMaterialInstanceDynamic*> DepthDynamicMaterials;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInstanceDynamic *DistortedDynamicMaterialSelfLeft;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInstanceDynamic *DistortedDynamicMaterialSelfRight;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInstanceDynamic *UndistortedDynamicMaterialSelfLeft;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInstanceDynamic *UndistortedDynamicMaterialSelfRight;
	UPROPERTY(BlueprintReadOnly, Category = "ViveSR|DualCamera|ImageRenderer") UMaterialInstanceDynamic *DepthDynamicMaterialSelf;

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|ImageRenderer") UProceduralMeshComponent* GetDistortedImagePlane(DualCameraIndex eye);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|ImageRenderer") UStaticMeshComponent* GetUndistortedImagePlane(DualCameraIndex eye);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|ImageRenderer") UStaticMeshComponent* GetDepthImagePlane();
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|ImageRenderer") void SetMode(DualCameraDisplayMode mode);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UArrowComponent* rootArrow;
	AActor* VROrigin = nullptr;
	AActor* ImagePlane = nullptr;

	int distortedImageWidth;
	int distortedImageHeight;
	int undistortedImageWidth;
	int undistortedImageHeight;

	int LastDistortedTextureUpdateTime = 0;
	int LastUndistortedTextureUpdateTime = 0;
	int LastDepthTextureUpdateTime = 0;

	float RealDistortedFPS = 0.0f;
	float RealUndistortedFPS = 0.0f;
	float RealDepthFPS = 0.0f;
	
	DualCameraStatus dualCameraStatus = DualCameraStatus::IDLE;

private:
	void UpdateUndistortedPose();
	void UpdateDistortedPose();

	UMaterialInterface* DefaultDistortedMaterialInstanceLeft;
	UMaterialInterface* DefaultDistortedMaterialInstanceRight;
	UMaterialInterface* DefaultUndistortedMaterialInstanceLeft;
	UMaterialInterface* DefaultUndistortedMaterialInstanceRight;
	UMaterialInterface* DefaultDepthMaterialInstance;
};
