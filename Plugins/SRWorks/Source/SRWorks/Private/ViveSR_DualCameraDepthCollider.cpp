// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_DualCameraDepthCollider.h"
#include "ViveSR_DualCameraDepthExtra.h"
#include "ViveSR_API.h"
#include "SRWorks.h"
#include "ViveSR_Enums.h"
// Sets default values
AViveSR_DualCameraDepthCollider::AViveSR_DualCameraDepthCollider()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("DepthCollider"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
}

// Called when the game starts or when spawned
void AViveSR_DualCameraDepthCollider::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AViveSR_DualCameraDepthCollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UpdateDepthCollider)
	{
		ViveSR_DualCameraDepthExtra::Instance()->GetDepthColliderFrameInfo();
		int currentDepthColliderUpdateTime = ViveSR_DualCameraDepthExtra::Instance()->DepthColliderTimeIndex;
		if (currentDepthColliderUpdateTime != LastDepthColliderUpdateTime)
		{
			ExtractCurrentColliders();
			LastDepthColliderUpdateTime = currentDepthColliderUpdateTime;
		}
	}
}

void AViveSR_DualCameraDepthCollider::Release() {
	UpdateDepthCollider = false;
	ColliderEnable = true;
	ViveSR_DualCameraDepthExtra::Instance()->Release();
}

void AViveSR_DualCameraDepthCollider::ExtractCurrentColliders()
{
	ViveSR_DualCameraDepthExtra::Instance()->GetDepthColliderData(NumCldVertData, VertexData, NumCldIdxData, CldIdxData);
	if (NumCldVertData > 0 && NumCldIdxData > 0)
	{
		GenerateMeshColliders();
	}
}

void AViveSR_DualCameraDepthCollider::GenerateMeshColliders()
{
	TArray<FVector> validVertexData;
	validVertexData.Init(FVector::ZeroVector, NumCldVertData);
	for (int i = 0; i < NumCldVertData; ++i)
	{
		float x = VertexData[i * 3 + 2];
		float y = VertexData[i * 3 + 0];
		float z = VertexData[i * 3 + 1];
		validVertexData[i].Set(x * 100, y * 100, z * 100);
	}

	TArray<int> validCldIdxData;
	validCldIdxData.Init(0, NumCldIdxData);
	for (int i = 0; i < NumCldIdxData; i += 3)
	{
		validCldIdxData[i + 0] = CldIdxData[i + 0];
		validCldIdxData[i + 1] = CldIdxData[i + 2];
		validCldIdxData[i + 2] = CldIdxData[i + 1];
	}

	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	mesh->CreateMeshSection_LinearColor(0, validVertexData, validCldIdxData, normals, UV0, vertexColors, tangents, ColliderEnable);

	// Enable collision data
	mesh->ContainsPhysicsTriMeshData(true);
}

int AViveSR_DualCameraDepthCollider::SetColliderProcessEnable(bool enable)
{
	int result = ViveSR_SetCommandBool(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Cmd::EXTRACT_DEPTH_MESH, enable);
	UpdateDepthCollider = enable;
	return result;
}
void AViveSR_DualCameraDepthCollider::SetLiveMeshVisibility(bool visible)
{
	mesh->SetVisibility(visible);
	ColliderMeshVisibility = visible;
}
bool AViveSR_DualCameraDepthCollider::SetColliderEnable(bool value)
{
	if (UpdateDepthCollider) {
		ColliderEnable = value;
		return true;
	}
	return false;
}

bool AViveSR_DualCameraDepthCollider::GetQualityScale(int &value)
{
	int result = ViveSR_GetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::COLLIDER_QUALITY, &value);
	return result == ViveSR::SR_Error::WORK;
}

bool AViveSR_DualCameraDepthCollider::SetQualityScale(int value)
{
	int result = ViveSR_SetParameterInt(FSRWorksModule::Get().MODULE_ID_DEPTH, ViveSR::Depth::Param::COLLIDER_QUALITY, value);
	if (result == ViveSR::SR_Error::WORK)
	{
		QualityScale = value;
		return true;
	}
	else
		return false;
}