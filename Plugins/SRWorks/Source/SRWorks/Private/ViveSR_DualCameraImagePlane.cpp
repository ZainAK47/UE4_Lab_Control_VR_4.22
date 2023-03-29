// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_DualCameraImagePlane.h"
#include "ViveSR_DualCameraImageCapture.h"
#include "ConstructorHelpers.h"


// Sets default values
AViveSR_DualCameraImagePlane::AViveSR_DualCameraImagePlane()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	FVector anchorLeft = FVector((10 - 0.071f), -0.0325f, 0.0026f);
	FVector anchorRight = FVector((10 - 0.071f), 0.0325f, 0.0026f);
	DistortedImagePlaneLeft = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMeshLeft"));
	SetupImagePlaneMesh(DistortedImagePlaneLeft, anchorLeft*100);
	DistortedImagePlaneRight = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMeshRight"));
	SetupImagePlaneMesh(DistortedImagePlaneRight, anchorRight*100);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (PlaneVisualAsset.Succeeded())
	{
		UndistortedImagePlaneLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImagePlane (Undistorted Left)"));
		UndistortedImagePlaneLeft->SetStaticMesh(PlaneVisualAsset.Object);
		SetupImagePlaneMesh(UndistortedImagePlaneLeft, anchorLeft * 100);
		UndistortedImagePlaneRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImagePlane (Undistorted Right)"));
		UndistortedImagePlaneRight->SetStaticMesh(PlaneVisualAsset.Object);
		SetupImagePlaneMesh(UndistortedImagePlaneRight, anchorRight * 100);

		DepthImagePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImagePlane (Depth)"));
		DepthImagePlane->SetStaticMesh(PlaneVisualAsset.Object);
		SetupImagePlaneMesh(DepthImagePlane);
	}
	else
	{
		UE_LOG(LogViveSR, Warning, TEXT("Image plane mesh not found."));
	}
}

// Called when the game starts or when spawned
void AViveSR_DualCameraImagePlane::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AViveSR_DualCameraImagePlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AViveSR_DualCameraImagePlane::Initial()
{
	distortedImageWidth = ViveSR_DualCameraImageCapture::Instance()->DistortedImageWidth;
	distortedImageHeight = ViveSR_DualCameraImageCapture::Instance()->DistortedImageHeight;
	undistortedImageWidth = ViveSR_DualCameraImageCapture::Instance()->UndistortedImageWidth;
	undistortedImageHeight = ViveSR_DualCameraImageCapture::Instance()->UndistortedImageHeight;

	cameraFocalLength = (float)(ViveSR_DualCameraImageCapture::Instance()->CameraParameters.FocalLength_L);
	undistortedCx[(int)DualCameraIndex::LEFT]    = ViveSR_DualCameraImageCapture::Instance()->UndistortedCx_L;
	undistortedCx[(int)DualCameraIndex::RIGHT]   = ViveSR_DualCameraImageCapture::Instance()->UndistortedCx_R;
	undistortedCy[(int)DualCameraIndex::LEFT]    = ViveSR_DualCameraImageCapture::Instance()->UndistortedCy_L;
	undistortedCy[(int)DualCameraIndex::RIGHT]   = ViveSR_DualCameraImageCapture::Instance()->UndistortedCy_R;
	undistortionMap[(int)DualCameraIndex::LEFT]  = ViveSR_DualCameraImageCapture::Instance()->UndistortionMap_L;
	undistortionMap[(int)DualCameraIndex::RIGHT] = ViveSR_DualCameraImageCapture::Instance()->UndistortionMap_R;

	InitDistortedImagePlane(DualCameraIndex::LEFT);
	InitDistortedImagePlane(DualCameraIndex::RIGHT);
	UndistortedImagePlaneLeft->SetRelativeScale3D(FVector(10 * (float)undistortedImageWidth / cameraFocalLength, 10 * (float)undistortedImageHeight / cameraFocalLength, 1.0f));
	UndistortedImagePlaneRight->SetRelativeScale3D(FVector(10 * (float)undistortedImageWidth / cameraFocalLength, 10 * (float)undistortedImageHeight / cameraFocalLength, 1.0f));
}

void AViveSR_DualCameraImagePlane::SetMode(DualCameraDisplayMode mode)
{
	switch (mode)
	{
	case DualCameraDisplayMode::MIX:
#ifdef USE_UNDISTORT_MESH
		DistortedImagePlaneLeft->SetHiddenInGame(false);
		DistortedImagePlaneRight->SetHiddenInGame(false);
#else
		UndistortedImagePlaneLeft->SetHiddenInGame(false);
		UndistortedImagePlaneRight->SetHiddenInGame(false);
#endif
		SetActorTickEnabled(true);
		break;
	case DualCameraDisplayMode::VIRTUAL:
	default:
#ifdef USE_UNDISTORT_MESH
		DistortedImagePlaneLeft->SetHiddenInGame(true);
		DistortedImagePlaneRight->SetHiddenInGame(true);
#else
		UndistortedImagePlaneLeft->SetHiddenInGame(true);
		UndistortedImagePlaneRight->SetHiddenInGame(true);
#endif
		SetActorTickEnabled(false);
		break;
	}
}
void AViveSR_DualCameraImagePlane::InitDistortedImagePlane(DualCameraIndex eye)
{
	TArray<FVector> vertices;
	TArray<FVector2D> uvs;
	TArray<int32> triangles;

	// Calculate the length of a pixel in the real space unit.
	UProceduralMeshComponent* plane = eye == DualCameraIndex::LEFT ? DistortedImagePlaneLeft : DistortedImagePlaneRight;
	float imagePlaneDistance = plane->RelativeLocation.X;
	float pixelLength = imagePlaneDistance / cameraFocalLength;

	//int totalPixels = distortedImageWidth * distortedImageHeight;
	for (int y = 0; y < distortedImageHeight; ++y) {
		for (int x = 0; x < distortedImageWidth; ++x) {
			int idx = y*distortedImageWidth + x;
			float undistortedX = undistortionMap[(int)eye][idx * 4 + 2] - undistortedCx[(int)eye];
			float undistortedY = undistortionMap[(int)eye][idx * 4 + 3] - undistortedCy[(int)eye];
			vertices.Add(FVector(undistortedX * pixelLength, undistortedY * pixelLength, 0.0f));
			uvs.Add(FVector2D(((float)x + 0.5f) / distortedImageWidth, ((float)y + 0.5f) / distortedImageHeight));
		}
	}

	for (int y = 0; y < distortedImageHeight - 1; ++y) {
		for (int x = 0; x < distortedImageWidth - 1; ++x) {
			int vert00ID = y * distortedImageWidth + x;
			int vert01ID = vert00ID + distortedImageWidth;
			int vert11ID = vert01ID + 1;
			int vert10ID = vert00ID + 1;
			// tri 1
			triangles.Add(vert00ID);
			triangles.Add(vert01ID);
			triangles.Add(vert11ID);
			// tri 2
			triangles.Add(vert00ID);
			triangles.Add(vert11ID);
			triangles.Add(vert10ID);
		}
	}
	plane->CreateMeshSection(0, vertices, triangles, TArray<FVector>(), uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}