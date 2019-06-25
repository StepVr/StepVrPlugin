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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHeadForOculus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHead;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FGun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FDLeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRightController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FDLeftFoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRightFoot;
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
	UseType_Normal,
	UseType_Mocap
};

UCLASS(ClassGroup = StepvrClassGroup, editinlinenew, meta = (BlueprintSpawnableComponent))
class STEPVRPLUGIN_API UStepVrComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	//手动校准HMD
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMD();
	UFUNCTION(BlueprintCallable, Category = StepvrLibrary)
	void ToggleResetType();

	//HMD校准方式
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FResetHMDType	ResetHMDType = FResetHMDType::ResetHMD_BeginPlay;

	/**
	 * UseType_Normal 单独HMD
	 * UseType_Mocap  动捕链接HMD
	 */
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FGameUseType	GameUseType = FGameUseType::UseType_Normal;

	/**
	 * 需要更新定位的设备ID
	 */
	UPROPERTY(EditDefaultsOnly, Category = StepvrLibrary)
	TArray<int32>	NeedUpdateDevices;

	UPROPERTY(AdvancedDisplay, EditAnywhere,BlueprintReadOnly, Category = StepvrLibrary)
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
	void ResetHMDRealTime();
	void ResetHMDAuto();

	//void UpdateTimer();

	void TickSimulate();
	void TickLocal();
	FTransform& GetDeviceDataPtr(int32 DeviceID);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerAddrOnServer(const uint32 InAddr);

	UPROPERTY(Replicated)
	uint32  PlayerAddr = 0;

	bool IsValidPlayerAddr();

	/**
	 *	@return 是否初始化
	 */
	bool InitializeLocalControlled();
	void AfterinitializeLocalControlled();

private:
	bool	bIsLocalControll = false;
	bool    bInitializeLocal = false;

	//差值
	float ResetYaw;

	/**
	 * 服务器
	 */
	TArray<FString> RemotAddrIP;

	/**
	 * 客户端
	 */
	FString ServerIP;


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