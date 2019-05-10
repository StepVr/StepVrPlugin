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
	//手动重置头显
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMD();
	UFUNCTION(BlueprintCallable, Category = StepvrLibrary)
	void ToggleResetType();

	//头显重置类型
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FResetHMDType	ResetHMDType = FResetHMDType::ResetHMD_BeginPlay;

	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FGameUseType	GameUseType = FGameUseType::UseType_Normal;


	UPROPERTY(AdvancedDisplay, EditAnywhere,BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRNode CurrentNodeState;

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void InitializeComponent() override;

	void OwnerBeginPlay();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void RegistInputComponent();

	//重置Oculus的角度
	void ResetControllPawnRotation();
	void ResetOculusRif();
	void ResetHMDRealTime();
	void ResetHMDAuto();

	//void UpdateTimer();

	void TickSimulate();
	void TickLocal();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerAddrOnServer(const uint32 InAddr);

	UPROPERTY(Replicated)
	uint32  PlayerAddr = 0;

	bool IsValidPlayerAddr();

	//return bool是否有效
	bool IsLocalControlled();

private:
	static  bool bGlobleIsReset;
	
	//是不是本地角色
	bool	bIsLocalControll = false;
	//检测是否为本地角色
	bool    bIsCheckLoal = false;

	float ResetYaw;

	//Auto Reset
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
};