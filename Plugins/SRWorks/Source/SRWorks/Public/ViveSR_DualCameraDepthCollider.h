// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ViveSR_DualCameraDepthCollider.generated.h"

UCLASS(Blueprintable)
class SRWORKS_API AViveSR_DualCameraDepthCollider : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViveSR_DualCameraDepthCollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Release();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "ViveSR|DualCamera|DepthCollider") bool ColliderMeshVisibility = true;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "ViveSR|DualCamera|DepthCollider") bool UpdateDepthCollider = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "ViveSR|DualCamera|DepthCollider") bool ColliderEnable = true;

	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|DepthCollider") int SetColliderProcessEnable(bool enable);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|DepthCollider") void SetLiveMeshVisibility(bool visible);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|DepthCollider") bool SetColliderEnable(bool value);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|DepthCollider") bool GetQualityScale(int &value);
	UFUNCTION(BlueprintCallable, category = "ViveSR|DualCamera|DepthCollider") bool SetQualityScale(int value);
	UPROPERTY(BlueprintReadOnly, category = "ViveSR|DualCamera|DepthCollider") UProceduralMeshComponent *mesh;

private:
	int QualityScale = 8;
	int LastDepthColliderUpdateTime;
	
	int NumCldVertData;
	TArray<float> VertexData;
	int NumCldIdxData;
	TArray<int> CldIdxData;	

	void ExtractCurrentColliders();
	void GenerateMeshColliders();
};
