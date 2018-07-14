#include "StepVrComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"
#include "StepVrBPLibrary.h"
#include "StepVrInput.h"
#include "StepVrGlobal.h"




bool UStepVrComponent::s_bIsResetOculus = false;

UStepVrComponent::UStepVrComponent():
bIsReset(false),
bIsLocalControll(false),
bIsInitOwner(false),
PlayerID(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UStepVrComponent::ResetHMDForStepVr()
{
	bIsReset = false;
	s_bIsResetOculus = false;
}

bool UStepVrComponent::ResetControllPawnRotation()
{
	APawn* _pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(_pawn)) 
	{ 
		return false; 
	}

	_pawn->SetActorRotation(FRotator::ZeroRotator);
	return ResetOculusRif();
}

void UStepVrComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (StepVrGlobal::Get()->ServerIsValid()) 
	{ 
		STEPVR_SERVER->UnRegistDelegate(PlayerID, this, bIsLocalControll);

		UE_LOG(LogStepVrPlugin, Warning, TEXT("Stop Replicate Player : %d"), PlayerID);
	}
}

void UStepVrComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsInitOwner())
	{
		return;
	}

	if (bIsLocalControll)
	{
		//获取本地
		TickLocal();
	}
	else
	{
		//获取远端数据
		TickSimulate();
	}
}

bool UStepVrComponent::ResetOculusRif()
{
#if WITH_EDITOR
	s_bIsResetOculus = false;
#endif

	if (!s_bIsResetOculus)
	{
		float Yaw = CurrentNodeState.FHead.Rotator().Yaw;

		float NewYaw = Yaw + 90.0;
		NewYaw = NewYaw > 180 ? NewYaw - 360 : NewYaw;

		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(NewYaw);
		UE_LOG(LogStepVrPlugin, Warning, TEXT("Stepvr ResetHMD Yaw:%f"), Yaw);

		s_bIsResetOculus = true;	
	}

	return s_bIsResetOculus;
}

void UStepVrComponent::TickSimulate()
{
	if (!StepVrGlobal::Get()->ServerIsValid()) 
	{ 
		return; 
	}

	for (auto deviceID : StepVrGlobal::Get()->GetReplicatedDevices())
	{
		switch ((StepVrDeviceID::Type)deviceID)
		{
			case StepVrDeviceID::DHead:
				GetRemoteData(deviceID, CurrentNodeState.FHead);
				break;
			case StepVrDeviceID::DGun:
				GetRemoteData(deviceID, CurrentNodeState.FGun);
				break;
			case StepVrDeviceID::DLeftController:
				GetRemoteData(deviceID, CurrentNodeState.FDLeftController);
				break;
			case StepVrDeviceID::DRightController:
				GetRemoteData(deviceID, CurrentNodeState.FRightController);
				break;
		}
	}
}

void UStepVrComponent::TickLocal()
{
	if (!StepVrGlobal::Get()->SDKIsValid()) 
	{ 
		return; 
	}

	/** Update Device Data */
	StepVR::Frame tmp = STEPVR_FRAME->GetFrame();
	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DHead, CurrentNodeState.FHead);
	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DGun, CurrentNodeState.FGun);
	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DLeftController, CurrentNodeState.FDLeftController);
	UStepVrBPLibrary::SVGetDeviceStateWithID(&tmp, StepVrDeviceID::DRightController, CurrentNodeState.FRightController);

	/** Auto Reset HMD */
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
	{
		FRotator	S_QTemp;
		FVector		S_VTemp;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(S_QTemp, S_VTemp);

		CurrentNodeState.FHeadForOculus.SetLocation(CurrentNodeState.FHead.GetLocation() - S_VTemp);
		CurrentNodeState.FHeadForOculus.SetRotation(S_QTemp.Quaternion());
		S_mStepVrDeviceState.FindOrAdd(StepVrDeviceID::DOculusHead) = CurrentNodeState.FHeadForOculus;


		/** Reset HMD */
		if (!bIsReset)
		{
			bIsReset = ResetControllPawnRotation();
		}
	}
}

bool UStepVrComponent::IsInitOwner()
{
	if (bIsInitOwner)
	{
		return bIsInitOwner;
	}

	do
	{
		auto _pawn = Cast<APawn>(GetOwner());
		if (!IsValid(_pawn))
		{
			break;
		}

		auto _playerstate = Cast<APlayerState>(_pawn->PlayerState);
		if (!IsValid(_playerstate))
		{
			break;
		}

		bIsInitOwner = true;
		PlayerID = _playerstate->PlayerId;
		bIsLocalControll = _pawn->IsLocallyControlled();

		if (!StepVrGlobal::Get()->ServerIsValid())
		{
			break;
		}
		STEPVR_SERVER->RegistDelegate(PlayerID, this, bIsLocalControll);

		UE_LOG(LogStepVrPlugin, Warning, TEXT("Start Replicate Player : %d"), PlayerID);
	} while (0);

	return bIsInitOwner;
}

