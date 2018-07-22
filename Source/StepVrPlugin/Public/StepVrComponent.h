// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "StepVr.h"
#include "StepVrServerModule.h"
#include "Components/ActorComponent.h"
#include "StepVrComponent.generated.h"

#define StepvrLibrary
#define StepvrClassGroup


/**
*   ��׼��
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

	//�ֶ�����Oculus�Ƕ�
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMDForStepVr();

	//�����Ƿ�����
	UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	bool GetGloveIsConnect();

	//��׼����λ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRNode CurrentNodeState;

	////���׶�λ����
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	//TMap<int32, FRotator> StepVRGloveNode;
	//
	////��ȡ��ָ��λ����
	//UFUNCTION(BlueprintPure, Category = StepvrLibrary)
	//void GetFingerRotator(EStepVRGloveType InType,FRotator& OutRotator);

protected:
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool IsInitOwner();

	//����Oculus�ĽǶ�
	bool ResetControllPawnRotation();
	bool ResetOculusRif();

	//ģ��Զ��
	void TickSimulate();

	//����ģ��
	void TickLocal();

	//��������
	//void UpdateGlove(StepVR::Frame* InFrame);

private:
	int32	PlayerID;
	bool	bIsReset = false;
	bool	bIsLocalControll = false;
	bool	bIsInitOwner = false;

	//�����Ƿ�����
	bool	GloveIsConnect = false;

	static bool s_bIsResetOculus;
};