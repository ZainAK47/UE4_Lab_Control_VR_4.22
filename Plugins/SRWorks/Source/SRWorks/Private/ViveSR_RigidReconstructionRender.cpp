// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_RigidReconstructionRender.h"
#include "ViveSR_RigidReconstruction.h"
#include "ViveSR_API.h"
#include "ViveSR_Enums.h"
#include "SRWorks.h"

// Sets default values
AViveSR_RigidReconstructionRender::AViveSR_RigidReconstructionRender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true;
}

// Called when the game starts or when spawned
void AViveSR_RigidReconstructionRender::BeginPlay()
{
	Super::BeginPlay();
}

void AViveSR_RigidReconstructionRender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (ViveSR_RigidReconstruction::Instance()->IsScanning)
		ViveSR_RigidReconstruction::Instance()->StopScanning();
}

// Called every frame
void AViveSR_RigidReconstructionRender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ViveSR_RigidReconstruction::Instance()->IsScanning)
	{
		int ProcessedFrame;
		ViveSR_RigidReconstruction::Instance()->GetRigidReconstructionFrame(ProcessedFrame);
		if (ProcessedFrame != LastProcessedFrame)
		{
			ViveSR_RigidReconstruction::Instance()->GetRigidReconstructionData(
				ProcessedFrame,
				Pose,
				VertexNum,
				VertexData,
				VertStrideInFloat,
				IndexNum,
				IndexData);
			if (VertexNum > 0 && IndexNum > 0)
			{
				TArray<FVector> validVertexData;
				validVertexData.Init(FVector::ZeroVector, VertexNum);
				for (int i = 0; i < VertexNum; i++)
				{
					float x = VertexData[i * 4 + 2];
					float y = VertexData[i * 4 + 0];
					float z = VertexData[i * 4 + 1];
					validVertexData[i].Set(x * 100, y * 100, z * 100);
				}
				TArray<int> validIndexData;
				validIndexData.Init(0, IndexNum);
				for (int i = 0; i < IndexNum; i += 3)
				{
					validIndexData[i + 0] = IndexData[i + 0];
					validIndexData[i + 1] = IndexData[i + 2];
					validIndexData[i + 2] = IndexData[i + 1];
				}
				TArray<FVector> normals;
				TArray<FVector2D> UV0;
				TArray<FProcMeshTangent> tangents;
				TArray<FLinearColor> vertexColors;
				mesh->CreateMeshSection_LinearColor(0, validVertexData, validIndexData, normals, UV0, vertexColors, tangents, true);
			}
			LastProcessedFrame = ProcessedFrame;
		}
	}
}

void AViveSR_RigidReconstructionRender::Initial()
{
	ViveSR_RigidReconstruction::Instance()->Initial();
	ViveSR_RigidReconstruction::Instance()->AllocOutputDataMemory();
	SetMeshDisplayMode(ReconstructionDisplayMode::ADAPTIVE_MESH);
	if (MeshMaterial != nullptr) SetMeshMaterial(MeshMaterial);
}
void AViveSR_RigidReconstructionRender::Release() {
	ViveSR_RigidReconstruction::Instance()->Release();
	ViveSR_RigidReconstruction::Instance()->FreeOutputDataMemory();
}

bool AViveSR_RigidReconstructionRender::SetMeshDisplayMode(ReconstructionDisplayMode displayMode)
{
	int result = ViveSR::SR_Error::FAILED;
	if (displayMode == ReconstructionDisplayMode::ADAPTIVE_MESH)
	{
		result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LITE_POINT_CLOUD_MODE, false);
		result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::FULL_POINT_CLOUD_MODE, false);
		result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LIVE_ADAPTIVE_MODE, true);
	}
	//else if (displayMode == ReconstructionDisplayMode::FIELD_OF_VIEW)
	//{
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LITE_POINT_CLOUD_MODE, true);
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::FULL_POINT_CLOUD_MODE, false);
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LIVE_ADAPTIVE_MODE, false);
	//}
	//else if (displayMode == ReconstructionDisplayMode::FULL_SCENE)
	//{
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LITE_POINT_CLOUD_MODE, false);
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::FULL_POINT_CLOUD_MODE, true);
	//	result = ViveSR_SetParameterBool(FSRWorksModule::Get().MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::RigidReconstruction::Param::LIVE_ADAPTIVE_MODE, false);
	//}
	return (result == ViveSR::SR_Error::WORK);
}

void AViveSR_RigidReconstructionRender::SetMeshMaterial(UMaterialInterface *material)
{
	mesh->SetMaterial(0, material);
}