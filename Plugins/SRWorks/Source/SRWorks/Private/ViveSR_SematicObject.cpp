// Fill out your copyright notice in the Description page of Project Settings.

#include "ViveSR_SematicObject.h"
#include "Paths.h"
UViveSR_SematicObject::UViveSR_SematicObject()
{
}
TArray<FElement> UViveSR_SematicObject::GetElements(const FString &_XmlPath)
{
	XmlPath = _XmlPath;
	Elements.Empty();
	if (!FPaths::FileExists(_XmlPath)) return Elements;

	TArray<FString> tags;
	TArray<int32> ids;
	TArray<FVector> positions;
	TArray<FVector> forwards;

	FXmlFile* _XmlFile = new FXmlFile(_XmlPath);
	FXmlNode* _RootNode = _XmlFile->GetRootNode();
	const TArray<FXmlNode*> nodes_0 = _RootNode->GetChildrenNodes();
	for (FXmlNode* node_0 : nodes_0)
	{
		FString tag_0 = node_0->GetTag();
		FString content_0 = node_0->GetContent();
		//UE_LOG(LogTemp, Warning, TEXT("Level0 %s %s"), *tag_0, *content_0);
		const TArray<FXmlNode*> nodes_1 = node_0->GetChildrenNodes();
		for (FXmlNode* node_1 : nodes_1)
		{
			FString tag_1 = node_1->GetTag();
			FString content_1 = node_1->GetContent();
			if (tag_1 == "tag") tags.Add(content_1);
			else if (tag_1 == "id") ids.Add(FCString::Atoi(*content_1));
			else if (tag_1 == "position" || tag_1 == "forward") {
				float x, y, z;
				const TArray<FXmlNode*> nodes_2 = node_1->GetChildrenNodes();
				for (FXmlNode* node_2 : nodes_2)
				{
					FString tag_2 = node_2->GetTag();
					FString content_2 = node_2->GetContent();
					if (tag_2 == "x") x = FCString::Atof(*content_2);
					else if (tag_2 == "y") y = FCString::Atof(*content_2);
					else if (tag_2 == "z") z = FCString::Atof(*content_2);
				}
				if (tag_1 == "position") positions.Add(FVector(x, y, z));
				else if (tag_1 == "forward") forwards.Add(FVector(x, y, z));
			}
		}
	}

	for (int i = 0; i < tags.Num(); i++) {
		FElement element;
		element.tag = tags[i];
		element.id = ids[i];
		element.position = positions[i];
		element.forward = forwards[i];
		Elements.Add(element);
	}
	return Elements;
}


