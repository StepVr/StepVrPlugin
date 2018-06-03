// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "StepVrServerModule.h"
#include "Components/ActorComponent.h"
#include "StepVrComponent.generated.h"

#define StepvrLibrary
#define StepvrClassGroup


/**
*   EquipID
*	FHeadForOculus To deal with OculusHMD Offset,Not original Data
*	FHead The original Data
*/
USTRUCT(BlueprintType)
struct FStepVRNode
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHeadForOculus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FHead;
	 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FGun;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FDLeftController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRightController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FLeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRightHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FLeftFoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FRighFoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FTransform FBack;
};


/**
*   MotionCapture Data
*	The upper part of the joint
*/
USTRUCT(BlueprintType)
struct FStepVRMCapNode
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FVector HipLoc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator HipRot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator Head;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator Neck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator LHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator LUpperarm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator LLowerarm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator RHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator RUpperarm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FRotator RLowerarm;
};


UCLASS(BlueprintType,meta = (BlueprintSpawnableComponent), ClassGroup = StepvrClassGroup)
class STEPVRPLUGIN_API UStepVrComponent : public UActorComponent ,public ReplciateComponment
{
	GENERATED_BODY()
public:
	UStepVrComponent();

	/**
	* Reset OculusHMD Offset  
	* When Auto Reset Something Wrong,You Can Run Function Reset Again
	*/
	UFUNCTION(BlueprintCallable,Category = StepvrLibrary)
	void ResetHMDForStepVr();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRNode CurrentNodeState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StepvrLibrary)
	FStepVRMCapNode MotionCaptuerState;

private:
	bool ResetControllPawnRotation();

	bool ResetOculusRif();

	void TickSimulate();
	void TickLocal();

	bool IsInitOwner();
private:
	int32	PlayerID;
	uint32	bIsReset : 1;

	uint32	bIsLocalControll : 1;
	uint32	bIsInitOwner : 1;

	static bool s_bIsResetOculus;
};