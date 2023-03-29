// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_DualCameraImageRenderer.h"
#include "ViveSR.h"
#include "ViveSR_DualCameraImageCapture.h"
#include "ViveSR_DualCameraImagePlane.h"
#include "ConstructorHelpers.h"
#include "Engine.h"


AViveSR_DualCameraImageRenderer::AViveSR_DualCameraImageRenderer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	rootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	RootComponent = rootArrow;

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> distortedMatInstAssetLeft(TEXT("MaterialInstanceConstant'/SRWorks/Materials/DistortedMaterialLeft_Inst.DistortedMaterialLeft_Inst'"));
	if (distortedMatInstAssetLeft.Succeeded())
	{
		DistortedMaterialInstanceLeft = distortedMatInstAssetLeft.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> distortedMatInstAssetRight(TEXT("MaterialInstanceConstant'/SRWorks/Materials/DistortedMaterialRight_Inst.DistortedMaterialRight_Inst'"));
	if (distortedMatInstAssetRight.Succeeded())
	{
		DistortedMaterialInstanceRight = distortedMatInstAssetRight.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> undistortedMatInstAssetLeft(TEXT("MaterialInstanceConstant'/SRWorks/Materials/UndistortedMaterialLeft_Inst.UndistortedMaterialLeft_Inst'"));
	if (undistortedMatInstAssetLeft.Succeeded())
	{
		UndistortedMaterialInstanceLeft = undistortedMatInstAssetLeft.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> undistortedMatInstAssetRight(TEXT("MaterialInstanceConstant'/SRWorks/Materials/UndistortedMaterialRight_Inst.UndistortedMaterialRight_Inst'"));
	if (undistortedMatInstAssetRight.Succeeded())
	{
		UndistortedMaterialInstanceRight = undistortedMatInstAssetRight.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> depthMatInstAsset(TEXT("MaterialInstanceConstant'/SRWorks/Materials/DepthMaterial_Inst.DepthMaterial_Inst'"));
	if (depthMatInstAsset.Succeeded())
	{
		DepthMaterialInstance = depthMatInstAsset.Object;
	}
	else
	{
		UE_LOG(LogViveSR, Warning, TEXT("Default Depth Material Instance not found."));
	}


#ifdef USE_UNDISTORT_MESH
	UpdateDistortedMaterial = true;
	UpdateUndistortedMaterial = false;
#else
	UpdateDistortedMaterial = false;
	UpdateUndistortedMaterial = true;
#endif
}

void AViveSR_DualCameraImageRenderer::SetMode(DualCameraDisplayMode mode)
{
	dualCameraDisplayMode = mode;
	static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->SetMode(mode);

	switch (mode)
	{
	case DualCameraDisplayMode::MIX:
		SetActorTickEnabled(true);
		break;
	case DualCameraDisplayMode::VIRTUAL:
	default:
		SetActorTickEnabled(false);
		break;
	}
}
void AViveSR_DualCameraImageRenderer::Initial()
{
	// Initial ViveSR_DualCameraImageCapture
	ViveSR_DualCameraImageCapture::Instance()->Initial();
	distortedImageWidth = ViveSR_DualCameraImageCapture::Instance()->DistortedImageWidth;
	distortedImageHeight = ViveSR_DualCameraImageCapture::Instance()->DistortedImageHeight;
	undistortedImageWidth = ViveSR_DualCameraImageCapture::Instance()->UndistortedImageWidth;
	undistortedImageHeight = ViveSR_DualCameraImageCapture::Instance()->UndistortedImageHeight;
	SetActorTickEnabled(true);
	dualCameraStatus = DualCameraStatus::WORKING;
}
void AViveSR_DualCameraImageRenderer::Release() {
	SetActorTickEnabled(false);
	dualCameraStatus = DualCameraStatus::IDLE;
	ViveSR_DualCameraImageCapture::Instance()->Release();
}
// Called when the game starts or when spawned
void AViveSR_DualCameraImageRenderer::BeginPlay()
{
	Super::BeginPlay();
	if (dualCameraStatus != DualCameraStatus::WORKING) return;
	DistortedDynamicMaterialSelfLeft = UMaterialInstanceDynamic::Create(DistortedMaterialInstanceLeft, this);
	DistortedDynamicMaterialSelfRight = UMaterialInstanceDynamic::Create(DistortedMaterialInstanceRight, this);
	UndistortedDynamicMaterialSelfLeft = UMaterialInstanceDynamic::Create(UndistortedMaterialInstanceLeft, this);
	UndistortedDynamicMaterialSelfRight = UMaterialInstanceDynamic::Create(UndistortedMaterialInstanceRight, this);
	DepthDynamicMaterialSelf = UMaterialInstanceDynamic::Create(DepthMaterialInstance, this);
	DistortedDynamicMaterialsLeft.Add(DistortedDynamicMaterialSelfLeft);
	DistortedDynamicMaterialsRight.Add(DistortedDynamicMaterialSelfRight);
	UndistortedDynamicMaterialsLeft.Add(UndistortedDynamicMaterialSelfLeft);
	UndistortedDynamicMaterialsRight.Add(UndistortedDynamicMaterialSelfRight);
	DepthDynamicMaterials.Add(DepthDynamicMaterialSelf);

	AViveSR_DualCameraImagePlane* planeActor = GetWorld()->SpawnActor<AViveSR_DualCameraImagePlane>();
	planeActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	planeActor->Initial();
	planeActor->DistortedImagePlaneLeft->SetMaterial(0, DistortedDynamicMaterialSelfLeft);
	planeActor->DistortedImagePlaneRight->SetMaterial(0, DistortedDynamicMaterialSelfRight);
	planeActor->UndistortedImagePlaneLeft->SetMaterial(0, UndistortedDynamicMaterialSelfLeft);
	planeActor->UndistortedImagePlaneRight->SetMaterial(0, UndistortedDynamicMaterialSelfRight);
	planeActor->DepthImagePlane->SetMaterial(0, DepthDynamicMaterialSelf);
	ImagePlane = planeActor;

	SetMode(dualCameraDisplayMode);
}

// Called every frame
void AViveSR_DualCameraImageRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (dualCameraStatus != DualCameraStatus::WORKING) return;

	if (UpdateDistortedMaterial)
	{
		UTexture2D *imgLeft, *imgRight;
		int frameIdx, timeIdx;
		ViveSR_DualCameraImageCapture::Instance()->UpdateDistortedImage();
		ViveSR_DualCameraImageCapture::Instance()->GetDistortedTexture(imgLeft, imgRight, frameIdx, timeIdx);
		if (LastDistortedTextureUpdateTime != timeIdx)
		{
			LastDistortedTextureUpdateTime = timeIdx;
			RealDistortedFPS = LastDistortedTextureUpdateTime / 1000.0f;
			for (int i = 0; i < DistortedDynamicMaterialsLeft.Num(); i++) {
				if (IsValid(DistortedDynamicMaterialsLeft[i]))
				{
					DistortedDynamicMaterialsLeft[i]->SetTextureParameterValue(TEXT("Left_Eye"), imgLeft);
				}
			}
			for (int i = 0; i < DistortedDynamicMaterialsRight.Num(); i++) {
				if (IsValid(DistortedDynamicMaterialsRight[i]))
				{
					DistortedDynamicMaterialsRight[i]->SetTextureParameterValue(TEXT("Right_Eye"), imgRight);
				}
			}
			UpdateDistortedPose();
		}
	}

	if (UpdateUndistortedMaterial)
	{
		UTexture2D *imgLeft, *imgRight;
		int frameIdx, timeIdx;
		ViveSR_DualCameraImageCapture::Instance()->UpdateUndistortedImage();
		ViveSR_DualCameraImageCapture::Instance()->GetUndistortedTexture(imgLeft, imgRight, frameIdx, timeIdx);
		if (LastUndistortedTextureUpdateTime != timeIdx)
		{
			LastUndistortedTextureUpdateTime = timeIdx;
			RealUndistortedFPS = LastUndistortedTextureUpdateTime / 1000.0f;
			for (int i = 0; i < UndistortedDynamicMaterialsLeft.Num(); i++) {
				if (IsValid(UndistortedDynamicMaterialsLeft[i]))
				{
					UndistortedDynamicMaterialsLeft[i]->SetTextureParameterValue(TEXT("Left_Eye"), imgLeft);
				}
			}
			for (int i = 0; i < UndistortedDynamicMaterialsRight.Num(); i++) {
				if (IsValid(UndistortedDynamicMaterialsRight[i]))
				{
					UndistortedDynamicMaterialsRight[i]->SetTextureParameterValue(TEXT("Right_Eye"), imgRight);
				}
			}
			UpdateUndistortedPose();
		}
	}

	if (UpdateDepthMaterial)
	{
		UTexture2D *img;
		int frameIdx, timeIdx;
		ViveSR_DualCameraImageCapture::Instance()->UpdateDepthImage();
		ViveSR_DualCameraImageCapture::Instance()->GetDepthTexture(img, frameIdx, timeIdx);
		if (LastDepthTextureUpdateTime != timeIdx)
		{
			LastDepthTextureUpdateTime = timeIdx;
			RealDepthFPS = LastDepthTextureUpdateTime / 1000.0f;
			for (int i = 0; i < DepthDynamicMaterials.Num(); i++) {
				if (IsValid(DepthDynamicMaterials[i]))
				{
					DepthDynamicMaterials[i]->SetTextureParameterValue(TEXT("Depth"), img);
				}
			}
		}
	}
}

void AViveSR_DualCameraImageRenderer::UpdateUndistortedPose()
{
	auto posePosition = ViveSR_DualCameraImageCapture::Instance()->GetUndistortedLocalPosition(DualCameraIndex::LEFT);
	auto poseRotation = ViveSR_DualCameraImageCapture::Instance()->GetUndistortedLocalRotation(DualCameraIndex::LEFT);
	//FVector localLocation(posePosition.X - 0.071, posePosition.Y + 0.0325, posePosition.Z + 0.0026);

	if (IsValid(VROrigin))
	{
		SetActorTransform(VROrigin->GetActorTransform());
		RootComponent->AddLocalTransform(FTransform(poseRotation, posePosition * 100));
	}
	else
	{
		RootComponent->SetWorldLocationAndRotation(posePosition * 100, poseRotation);
	}
}

void AViveSR_DualCameraImageRenderer::UpdateDistortedPose()
{
	auto posePosition = ViveSR_DualCameraImageCapture::Instance()->GetDistortedLocalPosition(DualCameraIndex::LEFT);
	auto poseRotation = ViveSR_DualCameraImageCapture::Instance()->GetDistortedLocalRotation(DualCameraIndex::LEFT);

	FVector localLocation(posePosition.X - 0.071, posePosition.Y + 0.0325, posePosition.Z + 0.0026);
	localLocation = localLocation * 100;

	if (IsValid(VROrigin))
	{
		SetActorTransform(VROrigin->GetActorTransform());
		RootComponent->AddLocalTransform(FTransform(poseRotation, localLocation));
	}
	else
	{
		RootComponent->SetWorldLocationAndRotation(localLocation, poseRotation);
	}
}

UProceduralMeshComponent* AViveSR_DualCameraImageRenderer::GetDistortedImagePlane(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->DistortedImagePlaneLeft;
	else
		return static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->DistortedImagePlaneRight;
}
UStaticMeshComponent* AViveSR_DualCameraImageRenderer::GetUndistortedImagePlane(DualCameraIndex eye)
{
	if (eye == DualCameraIndex::LEFT)
		return static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->UndistortedImagePlaneLeft;
	else
		return static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->UndistortedImagePlaneRight;
}
UStaticMeshComponent* AViveSR_DualCameraImageRenderer::GetDepthImagePlane()
{
	return static_cast<AViveSR_DualCameraImagePlane*>(ImagePlane)->DepthImagePlane;
}