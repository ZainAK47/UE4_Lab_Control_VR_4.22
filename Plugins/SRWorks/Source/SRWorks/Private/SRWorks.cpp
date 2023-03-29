// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SRWorks.h"
#include "Core.h"
#include "Paths.h"
#include "IPluginManager.h"
#include "ViveSR_API.h"
#include "ViveSR_Enums.h"

#define LOCTEXT_NAMESPACE "FSRWorksModule"

//FSRWorksModule* FSRWorksModule::SRWorkModule = nullptr;

void FSRWorksModule::StartupModule()
{
	//SRWorkModule = this;
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	BaseDir = FPaths::ProjectPluginsDir() + "SRWorks";

	// Add on the relative location of the third party dll and load it
	//FString LibraryPath, Dual_Camera_Device_Intrinsic_LibPath, opencvLibPath;
#if PLATFORM_WINDOWS
	const FString SRWorksAPILibName = "ViveSR_API.dll";
#elif PLATFORM_MAC
    //LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SRWorksLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS

	LoadLibraryDLL(SRWorksAPILibName, SRWorksAPIDllHandle);

	if (SRWorksAPIDllHandle)
	{
		SRWorksVersion = ViveSR_GetVersion();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load SRWorks library"));
	}
}

void FSRWorksModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//SRWorksImageCapture::DestroyImageCapture();
	//ViveSR_Stop();

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(SRWorksAPIDllHandle);
	SRWorksAPIDllHandle = nullptr;
}

//FSRWorksModule* FSRWorksModule::GetSRWorkModule()
//{
//	return SRWorkModule;
//}

int FSRWorksModule::InitialFramework()
{
	SRWorksResult = ViveSR_Initial();
	ViveSR_EnableLog(true);
	ViveSR_SetLogLevel((int)ViveSR::SR_LogLevel::SRLOG_LEVEL_2);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Error, TEXT("[ViveSR] ViveSR_Initial failed. %d"), SRWorksResult); return SRWorksResult; }
	
	SRWorksResult = ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_SEETHROUGH, &MODULE_ID_SEETHROUGH);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] CreateModule MODULE_ID_SEETHROUGH Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_DEPTH, &MODULE_ID_DEPTH);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] CreateModule DEPTH Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_AI_VISION, &MODULE_ID_AI_VISION);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] CreateModule MODULE_ID_AI_SCENE Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_RIGID_RECONSTRUCTION, &MODULE_ID_RIGID_RECONSTRUCTION);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] CreateModule RIGID_RECONSTRUCTION Error %d"), SRWorksResult); return SRWorksResult; }
	return SRWorksResult;
}

int FSRWorksModule::StartFramework()
{
	IsFrameworkStarted = true;
	SRWorksResult = ViveSR_StartModule(MODULE_ID_SEETHROUGH);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] StartModule DISTORTED Error %d %d"), MODULE_ID_SEETHROUGH, SRWorksResult); return SRWorksResult; }
	
	SRWorksResult = ViveSR_StartModule(MODULE_ID_DEPTH);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] StartModule DEPTH Error %d %d"), MODULE_ID_DEPTH, SRWorksResult); return SRWorksResult; }

	AI_VisionModuleInfo *AI_VisionInfo = new AI_VisionModuleInfo();
	ViveSR_GetParameterStruct(MODULE_ID_AI_VISION, ViveSR::AI_Vision::MODULEINFO, (void**)&AI_VisionInfo);
	strcpy(AI_VisionInfo->Model_Path, TCHAR_TO_ANSI(*(FPaths::ProjectPluginsDir() + "SRWorks/Binaries/Win64")));
	ViveSR_SetParameterStruct(MODULE_ID_AI_VISION, ViveSR::AI_Vision::MODULEINFO, (void*)AI_VisionInfo);
	SRWorksResult = ViveSR_StartModule(MODULE_ID_AI_VISION);
	if (SRWorksResult == 1) UE_LOG(LogTemp, Warning, TEXT("[ViveSR] Please put the model folder in the assigned path"));
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] StartModule AI_VISION Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_StartModule(MODULE_ID_RIGID_RECONSTRUCTION);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] StartModule RIGID_RECONSTRUCTION Error %d"), SRWorksResult); return SRWorksResult; }

	//SRWorksResult = ViveSR_ModuleLink(MODULE_ID_SEETHROUGH, MODULE_ID_DEPTH, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
	//if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] ModuleLink UNDISTORTED to DDEPTH Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_ModuleLink(MODULE_ID_DEPTH, MODULE_ID_RIGID_RECONSTRUCTION, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] ModuleLink DEPTH to RIGID_RECONSTRUCTION Error %d"), SRWorksResult); return SRWorksResult; }

	SRWorksResult = ViveSR_ModuleLink(MODULE_ID_RIGID_RECONSTRUCTION, MODULE_ID_AI_VISION, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
	if (SRWorksResult != ViveSR::SR_Error::WORK) { UE_LOG(LogTemp, Warning, TEXT("[ViveSR] ModuleLink RIGID_RECONSTRUCTION to AI_SCENE Error %d"), SRWorksResult); return SRWorksResult; }
	return SRWorksResult;
}

int FSRWorksModule::StopFramework()
{
	IsFrameworkStarted = false;
	return ViveSR_Stop();
}

bool FSRWorksModule::LoadLibraryDLL(const FString& libName, void*& handle)
{
	FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64"), *libName);
	handle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	return handle != nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSRWorksModule, SRWorks)