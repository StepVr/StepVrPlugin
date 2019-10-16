// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "StepVrComponent.generated.h"

#define StepvrLibrary
#define StepvrClassGroup


USTRUCT(BlueprintType)
struct FStepVRNode
{
	GENERATED_USTRUCT_BODY()

	//6号键Location + HMD的Rotator
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHeadForHMD;

	//ID ：6
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHead;
	 
	//ID ：4
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FGun;

	//ID ：1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FDLeftController;

	//ID ：2
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRightController;
};

enum class FHMDType : uint8
{
	HMD_Oculus,
	HMD_Windows,
	HMD_InValid,
};

UENUM()
enum class FResetHMDType : uint8
{
	ResetHMD_RealTime,
	ResetHMD_BeginPlay
};


UENUM()
enum class FGameUseType : uint8
{
	UseType_VR,
	UseType_Mocap,
	UseType_Cave
};


UCLASS(ClassGroup = StepvrClassGroup, editinlinenew, meta = (BlueprintSpawnableComponent))
class STEPVRPLUGIN_API UStepVrComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * 手动校准HMD
	 */
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMD();

	/**
	* 本机/远端 定位信息
	* 支持联网，PS：StepVrServer
	*/
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	void DeviceTransform(int32 DeviceID, FTransform& Trans);

	//ip是否有效
	bool	IsValidPlayerAddr();
	//获取同步IP
	uint32	GetPlayerAddr();

	//是否初始化
	bool	IsInitialization();
	//是否本地控制玩家
	bool	IsLocalControlled();

	/**
	 * HMD校准方式
	 * Cave 模式无需校准，当前字段无效
	 */
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FResetHMDType	ResetHMDType = FResetHMDType::ResetHMD_BeginPlay;

	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FGameUseType	GameUseType = FGameUseType::UseType_VR;

	//动捕联网
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	bool bMocapReplicate = false;

	/**
	 * 本机/远端 定位信息
	 * 支持联网，PS：StepVrServer
	 * 后续版本删掉，请使用DeviceTransform
	 */
	UPROPERTY(AdvancedDisplay,BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRNode CurrentNodeState;

protected:
	/**
	 * Delegate
	 */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//R
	void RegistInputComponent();

	//Reset HMD
	void ResetHMDDirection();
	void ResetOculusRif();
	void ResetOculusRealTime();
	void ResetHMDAuto();

	void TickLocal();
	FTransform& GetDeviceDataPtr(int32 DeviceID);

	//联网同步地址
	UPROPERTY(Replicated)
	uint32  PlayerAddr = 0;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = StepvrLibrary)
	FString  PlayerIP;
	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerAddrOnServer(const FString& LocalIP);

	/**
	 *	@return 是否初始化
	 */
	bool InitializeLocalControlled();
	void AfterinitializeLocalControlled();

private:
	bool	bIsLocalControll = false;
	bool    bInitializeLocal = false;

	//插值
	float ResetYaw;

	/**
	 * 客户端
	 */
	FString ServerIP;

	//需要更新的ID
	TArray<int32>	NeedUpdateDevices;
	
	//需要同步的ID
	TArray<int32>   ReplicateID;

	FHMDType HMDType = FHMDType::HMD_InValid;

	/************************************************************************/
	/* 时时校准															   */
	/************************************************************************/
	#define Yawn  45
	float HMDYaw[Yawn];
	float IMUYaw[Yawn];
	float HMDrate[Yawn - 1];
	float HMDPitch;
	int Tnum;
	float Cnum;

	bool bIsReset = true;
	bool bIsStart = false;
	bool bIsCorrect = false;
	bool s_bIsResetOculus = true;

	float NewYaw;
	/************************************************************************/
	/* Auto Reset                                                           */
	/************************************************************************/
};