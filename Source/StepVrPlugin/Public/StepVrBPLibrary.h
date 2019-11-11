// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "StepVr.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StepVrBPLibrary.generated.h"


/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/


class Frame;
/**
* 
* received Date class Need inherit StepVrServerInterface
*/
UENUM()
enum class FGameType : uint8
{
	GameStandAlone,
	GameServer,
	GameClient
};

UCLASS()
class STEPVRPLUGIN_API UStepVrBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	*	先确保PlayerController有效
	*	@param: GameId和自定义的ID匹配	
	*/
	UFUNCTION(BlueprintCallable, Category = StepvrLibrary)
	static bool SVCheckGameLic(FString gameId);

	
	static void SVGetDeviceStateWithID(StepVR::SingleNode* InSingleNode,int32 DeviceID, FTransform& Transform);
	
	/**
	 * 获取标准件数据
	 */
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	static void SVGetDeviceStateWithID(int32 DeviceID, FTransform& Transform);

	/**
	 * 激光系定位转UE系
	 */
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	static FTransform Convert2UETransform(float Vx, float Vy, float Vz, float Qw, float Qx, float Qy, float Qz);

	/**
	 * 	type
	 * 	0 EStandAlone,
	 *	1 EClient,
	 *	2 EServer,
	 *	@ServerIP ：1 EClient 设置 ServerIP,
	 */
	UFUNCTION(BlueprintCallable, Category = StepvrLibrary)
	static void SetGameType(FGameType type,FString ServerIP);
};
