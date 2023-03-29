// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

UENUM(BlueprintType)
enum class FrameworkStatus : uint8
{
	WORKING,
	STOP,
	ERROR
};

UENUM(BlueprintType)
enum class DualCameraMode : uint8
{
	REAL,
	MIX
};

UENUM(BlueprintType)
enum class DualCameraIndex : uint8
{
	LEFT,
	RIGHT
};

UENUM(BlueprintType)
enum class CalibrationType : uint8
{
	RELATIVE,
	ABSOLUTE
};

UENUM(BlueprintType)
enum class CalibrationAxis : uint8
{
	X, Y, Z
};

UENUM(BlueprintType)
enum class DualCameraDisplayMode : uint8
{
	VIRTUAL,
	MIX
};

UENUM(BlueprintType)
enum class DualCameraStatus : uint8
{
	NOT_FOUND,
	IDLE,
	WORKING,
	ERROR
};

UENUM(BlueprintType)
enum class SRLogLevel : uint8
{
	LEVEL_0 = 3,				// turn-off any log except error & warning
	LEVEL_1 = 4,				// the lower level, the less log, default value
	LEVEL_2 = 5,
	LEVEL_3 = 6,
	LEVEL_MAX = 10,			// turn-on all log msg
};

UENUM(BlueprintType)
enum class ReconstructionLiveMeshExtractMode : uint8
{
	VERTEX_WITHOUT_NORMAL = 0,
	VERTEX_WITH_NORMAL = 1,
	FACE_NORMAL = 2,
};

UENUM(BlueprintType)
enum class ReconstructionDisplayMode : uint8
{
	FULL_SCENE = 0,
	FIELD_OF_VIEW = 1,
	ADAPTIVE_MESH = 2,
};

//UENUM(BlueprintType)
//enum class ReconstructionLiveMeshExtractMode : uint8
//{
//	VERTEX_WITHOUT_NORMAL = 0,
//	VERTEX_WITH_NORMAL = 1,
//	FACE_NORMAL = 2,
//};

UENUM(BlueprintType)
enum class CameraQuality : uint8
{
	BRIGHTNESS				= 100,
	CONTRAST				= 101,
	HUE						= 102,
	SATURATION				= 103,
	SHARPNESS				= 104,
	GAMMA					= 105,
	//COLOR_ENABLE          = 106,
	WHITE_BALANCE			= 107,
	BACKLIGHT_COMPENSATION	= 108,
	GAIN					= 109,
	//PAN                   = 110,
	//TILT                  = 111,
	//ROLL                  = 112,
	//ZOOM                  = 113,
	//EXPOSURE              = 114,
	//IRIS                  = 115,
	//FOCUS                 = 116,
};

UENUM(BlueprintType)
enum class UDepthCase : uint8
{
	DEFAULT,
	CLOSE_RANGE,
};