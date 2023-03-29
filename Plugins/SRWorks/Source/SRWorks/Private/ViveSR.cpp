// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "ViveSR_DualCameraImageCapture.h"
#include "ViveSR_DualCameraDepthExtra.h"
#include "SRWorks.h"
#include "ViveSR_RigidReconstruction.h"
// Sets default values
AViveSR::AViveSR()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AViveSR::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AViveSR::StaticClass(), FoundActors);
	if (FoundActors.Num() > 1)
	{
		UE_LOG(LogViveSR, Log, TEXT("[ViveSR] Please make only one ViveSR in your content"));
	}

	FString version = ViveSR_GetVersion();
	UE_LOG(LogViveSR, Log, TEXT("Version %s"), *version);

	StartFramework();
}

void AViveSR::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopFramework();
}

void AViveSR::StartFramework()
{
	if (frameworkStatus == FrameworkStatus::WORKING) return;
	int result = FSRWorksModule::Get().InitialFramework();
	if (result != ViveSR::SR_Error::WORK)
	{
		UE_LOG(LogViveSR, Warning, TEXT("Initial Framework: %d"), result);
		frameworkStatus = FrameworkStatus::ERROR;
		return;
	}

	result = FSRWorksModule::Get().StartFramework();
	if (result != ViveSR::SR_Error::WORK)
	{
		UE_LOG(LogViveSR, Warning, TEXT("Start Framework: %d"), result);
		frameworkStatus = FrameworkStatus::ERROR;
		return;
	}
	frameworkStatus = FrameworkStatus::WORKING;

	// Intial DualCameraImageRenderer
	if (!IsValid(DualCameraImageRenderer))	// Get the reference if it exists.
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AViveSR_DualCameraImageRenderer::StaticClass(), foundActors);
		if (foundActors.Num() > 0)
			DualCameraImageRenderer = (AViveSR_DualCameraImageRenderer*)foundActors[0];
	}
	if (IsValid(DualCameraImageRenderer)) {
		DualCameraImageRenderer->Initial();
		if (IsValid(VROrigin))
		{
			DualCameraImageRenderer->SetVROrigin(VROrigin);
		}
		ViveSR_DualCameraDepthExtra::Instance()->InitialDepthCollider(ViveSR_DualCameraImageCapture::Instance()->DepthImageWidth, ViveSR_DualCameraImageCapture::Instance()->DepthImageHeight);
	}

	// Intial DualCameraDepthCollider
	if (!IsValid(DualCameraDepthCollider)) // Get the reference if it exists.
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AViveSR_DualCameraDepthCollider::StaticClass(), foundActors);
		if (foundActors.Num() > 0)
			DualCameraDepthCollider = (AViveSR_DualCameraDepthCollider*)foundActors[0];
	}

	// Intial RigidReconstructionRender
	if (!IsValid(RigidReconstructionRender)) // Get the reference if it exists.
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AViveSR_RigidReconstructionRender::StaticClass(), foundActors);
		if (foundActors.Num() > 0)
			RigidReconstructionRender = (AViveSR_RigidReconstructionRender*)foundActors[0];
	}
	if (IsValid(RigidReconstructionRender))
	{
		RigidReconstructionRender->Initial();
	}
}

void AViveSR::StopFramework()
{
	if (frameworkStatus != FrameworkStatus::STOP)
	{
		frameworkStatus = FrameworkStatus::STOP;
		if (IsValid(DualCameraImageRenderer)) DualCameraImageRenderer->Release();
		if (IsValid(DualCameraDepthCollider)) DualCameraDepthCollider->Release();
		if (IsValid(RigidReconstructionRender)) RigidReconstructionRender->Release();

		int result = FSRWorksModule::Get().StopFramework();
		UE_LOG(LogViveSR, Warning, TEXT("Stop Framework: %d"), result);
	}
	else
	{
		UE_LOG(LogViveSR, Warning, TEXT("Stop Framework: not working"));
	}
}



