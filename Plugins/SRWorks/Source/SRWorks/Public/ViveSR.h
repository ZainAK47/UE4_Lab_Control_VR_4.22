// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ViveSR_DualCameraImageRenderer.h"
#include "ViveSR_DualCameraDepthCollider.h"
#include "ViveSR_RigidReconstructionRender.h"
#include "ViveSR_API.h"
#include "ViveSR_UEnums.h"
#include "ViveSR.generated.h"
DEFINE_LOG_CATEGORY_STATIC(LogViveSR, Log, All)

UCLASS()
class SRWORKS_API AViveSR : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViveSR();

	UPROPERTY(EditAnywhere, Category = "ViveSR") AActor* VROrigin = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, category = "ViveSR")
	void StartFramework();
	UFUNCTION(BlueprintCallable, category = "ViveSR")
	void StopFramework();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViveSR") FrameworkStatus frameworkStatus = FrameworkStatus::STOP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ViveSR") AViveSR_DualCameraImageRenderer *DualCameraImageRenderer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ViveSR") AViveSR_DualCameraDepthCollider *DualCameraDepthCollider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ViveSR") AViveSR_RigidReconstructionRender *RigidReconstructionRender;
};
