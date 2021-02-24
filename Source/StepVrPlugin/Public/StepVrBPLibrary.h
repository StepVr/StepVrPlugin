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



UCLASS()
class STEPVRPLUGIN_API UStepVrBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 *	Client模式需要设置ServerIP
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|Game")
	static void SetGameType(EStepGameType type,FString ServerIP);

	/**
	 * 设置缩放，所有定位数据得Location将进行缩放
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|Game")
	static void SetScaleTransform(bool NewState, FVector Scales);

	/**
	 * 设置偏移，所有定位进行偏移
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|Game")
	static void SetOffsetTransform(bool NewState, FVector Offset);

	UFUNCTION(BlueprintPure, Category = "StepVr|Game")
	static bool GetDeviceTransform(int32 DeviceID,FTransform& OutData);

	/**
	 * 是否使用Step的MotionController
	 * 默认使用
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|Game")
	static void SetUseStepMotionController(bool UseStep);


	/************************************************************************/
	/*                            卡丁车                                     */
	/************************************************************************/
	/*
	* Percent speed:[0,100],设置当前速度
	* -1：服务未开
	* 0：设置成功
	* 1：响应超时
	* 100：数据未发送（找不到串口导致）
	*/
	UFUNCTION(BlueprintCallable, Category = "StepVr|KaDing")
	static bool StepSetKartMaxSpeed(int32 Percent);
	
	/**
	 * 开启或解除急刹
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|KaDing")
	static bool StepSetKartBrake(bool bSet);
	
	/**
	 * 是否禁用物理倒挡
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|KaDing")
	static bool StepSetKartEnableReverse(bool bset);

	/**
	 * 设置前进或后退(禁用物理倒挡后才起作用)
	 */
	UFUNCTION(BlueprintCallable, Category = "StepVr|KaDing")
	static bool StepSetKartForward(bool bForward);
};
