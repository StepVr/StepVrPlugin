// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "StepVr.h"
#include "StepVrServerModule.h"
#include "Components/ActorComponent.h"
#include "StepVrComponent.generated.h"

#define StepvrLibrary
#define StepvrClassGroup


/**
*   标准件
*/
USTRUCT(BlueprintType)
struct FStepVRNode
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FTransform FHeadForOculus;

	UPROPERTY(BlueprintReadOnly)
	FTransform FHead;
	 
	UPROPERTY(BlueprintReadOnly)
	FTransform FGun;

	UPROPERTY(BlueprintReadOnly)
	FTransform FDLeftController;

	UPROPERTY(BlueprintReadOnly)
	FTransform FRightController;

	UPROPERTY(BlueprintReadOnly)
	FTransform FLeftHand;

	UPROPERTY(BlueprintReadOnly)
	FTransform FRightHand;

	UPROPERTY(BlueprintReadOnly)
	FTransform FLeftFoot;

	UPROPERTY(BlueprintReadOnly)
	FTransform FRighFoot;

	UPROPERTY(BlueprintReadOnly)
	FTransform FBack;
};


UCLASS(BlueprintType,meta = (BlueprintSpawnableComponent), ClassGroup = StepvrClassGroup)
class STEPVRPLUGIN_API UStepVrComponent : public UActorComponent ,public ReplciateComponment
{
	GENERATED_BODY()
public:
	UStepVrComponent();

	//手动重置Oculus角度
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMDForStepVr();

	//手套是否链接
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	bool GetGloveIsConnect();

	//标准件定位数据
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRNode CurrentNodeState;

	////手套定位数据
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	//TMap<int32, FRotator> StepVRGloveNode;
	//
	////获取手指定位数据
	//UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	//void GetFingerRotator(EStepVRGloveType InType,FRotator& OutRotator);

protected:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool IsInitOwner();

	//重置Oculus的角度
	bool ResetControllPawnRotation();
	bool ResetOculusRif();

	//模拟远端
	void TickSimulate();

	//本地模拟
	void TickLocal();

	//更新手套
	//void UpdateGlove(StepVR::Frame* InFrame);

private:
	int32	PlayerID;
	bool	bIsReset = false;
	bool	bIsLocalControll = false;
	bool	bIsInitOwner = false;

	//手套是否连接
	bool	GloveIsConnect = false;

	static bool s_bIsResetOculus;
};