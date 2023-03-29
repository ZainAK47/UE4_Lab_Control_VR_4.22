// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ViveSR_UEnums.h"
#include "ViveSR_RigidReconstructionRender.generated.h"

UCLASS(Blueprintable)
class SRWORKS_API AViveSR_RigidReconstructionRender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViveSR_RigidReconstructionRender();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction|Render") void Initial();
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction|Render") void Release();
	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction|Render") void SetMeshMaterial(UMaterialInterface *material);

	UPROPERTY(EditAnywhere, category = "ViveSR|RigidReconstruction|Render") UMaterialInterface *MeshMaterial;
	UPROPERTY(EditAnywhere, category = "ViveSR|RigidReconstruction|Render") UProceduralMeshComponent *mesh;
private:
	int VertexNum;
	int IndexNum;
	int ProcessedFrame;
	int VertStrideInFloat;
	int ColliderNum;
	int LastProcessedFrame;
	TArray<float> VertexData;
	TArray<int> IndexData;
	int NumSubMeshes;
	int LastMeshes;
	int NumLastMeshVert;
	bool IsMeshUpdate;
	TArray<float> Pose;

	bool SetMeshDisplayMode(ReconstructionDisplayMode displayMode);
};
