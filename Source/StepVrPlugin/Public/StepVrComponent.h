// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "StepVrDataInterface.h"
#include "StepVrComponent.generated.h"

#define StepvrLibrary
#define StepvrClassGroup

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



/**
 * 1 必须为Pawn，才可使用
 * 2 添加Server插件，DeviceTransform即为同步后数据
 * 3 启动时自动校准HMD
 */
UCLASS(ClassGroup = StepvrClassGroup, editinlinenew, meta = (BlueprintSpawnableComponent))
class STEPVRPLUGIN_API UStepVrComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * HMD校准方式
	 * Cave  无需校准
	 */
	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FResetHMDType	ResetHMDType = FResetHMDType::ResetHMD_BeginPlay;

	UPROPERTY(EditAnywhere, Category = StepvrLibrary)
	FGameUseType	GameUseType = FGameUseType::UseType_VR;

	//手动校准HMD
	UFUNCTION(BlueprintCallable, Category = StepvrLibrary)
	void ResetHMD();

	/**
	* 本机/远端 定位信息
	* 支持联网，PS：StepVrServer
	*/
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	void DeviceTransform(int32 DeviceID, FTransform& Trans);

	UPROPERTY(Replicated)
	uint32 PlayerGUID;

	UFUNCTION(Server,Reliable)
	void SetPlayerGUID(uint32 NewPlayerGUID);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	void ResetHMDFinal();
	void ResetOculusRealTime();
	void ResetHMDAuto();

	bool CheckControllState();

	//初始化
	void AfterinitializeLocalControlled();

private:
	bool    bAlreadyInitializeLocal = false;
	bool	bLocalControlled = false;

	//所有定位
	SinglePlayer SinglePlayerData;

	//头显校准角度
	float ResetYaw;

	/************************************************************************/
	/* Auto Reset 															   */
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