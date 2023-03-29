// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ViveSR_UEnums.h"
#include "Runtime/XmlParser/Public/XmlParser.h"
#include "ViveSR_SematicObject.generated.h"

USTRUCT(BlueprintType)
struct FElement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Element Struct") FString tag;
	UPROPERTY(BlueprintReadWrite, Category = "Element Struct") int32 id;
	UPROPERTY(BlueprintReadWrite, Category = "Element Struct") FVector position;
	UPROPERTY(BlueprintReadWrite, Category = "Element Struct") FVector forward;
	FElement() {
	}
};

/**
 *
 */
UCLASS(BlueprintType)
class SRWORKS_API UViveSR_SematicObject : public UObject
{
	GENERATED_BODY()
public:
	UViveSR_SematicObject();

	UFUNCTION(BlueprintCallable, category = "ViveSR|RigidReconstruction")
	TArray<FElement> GetElements(const FString &_XmlPath);

	UPROPERTY(BlueprintReadOnly, category = "ViveSR|RigidReconstruction")
	TArray<FElement> Elements;

	UPROPERTY(BlueprintReadOnly, category = "ViveSR|RigidReconstruction")
	FString XmlPath;
};
