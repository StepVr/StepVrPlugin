#include "Auxiliary/StepVrAuxiliaryCollection.h"
#include "Auxiliary/StepVrAuxiliaryUDP.h"
#include "StepVrGlobal.h"
#include "LocalDefine.h"
#include "Kismet/GameplayStatics.h"
#include "StepVrComponent.h"
#include "StepVrCameraComponent.h"



FStepVrAuxiliaryCollection::~FStepVrAuxiliaryCollection()
{
	
}

void FStepVrAuxiliaryCollection::StartCollection()
{
	if (!StepVrAuxiliaryUDP.IsValid())
	{
		StepVrAuxiliaryUDP = MakeShared<FStepVrAuxiliaryUDP>();
		StepVrAuxiliaryUDP->StartServer();
	}
}

void FStepVrAuxiliaryCollection::StopCollection()
{

}


void FStepVrAuxiliaryCollection::EngineBeginFrame()
{
	if (NeedCollection())
	{
		float DelayTime = GWorld->GetDeltaSeconds();;
		//定位信息
		{
			FDeviceFrame& DeviceFrame = STEPVR_GLOBAL->GetDeviceFrame();

			FAuxiliaryDevice AuxiliaryDevice;

			auto HeadFrame = DeviceFrame.GetDevice(StepVrDeviceID::DHead);
			AuxiliaryDevice.Head = HeadFrame.GetTransform().GetLocation();
			AuxiliaryDevice.bHead = HeadFrame.ISLink();
			auto GunFrame = DeviceFrame.GetDevice(StepVrDeviceID::DGun);
			AuxiliaryDevice.Gun = GunFrame.GetTransform().GetLocation();
			AuxiliaryDevice.bGun = GunFrame.ISLink();

			AuxiliaryDevice.TotalFrames = GFrameCounter;
			if (GWorld)
			{
				AuxiliaryDevice.FrameTimes = DelayTime;
			}

			StepVrAuxiliaryUDP->SendDeviceData(AuxiliaryDevice);
		}

		//ControllPawn信息
		{
			static float Interval = 0;
			if ((Interval += DelayTime) > 1)
			{
				Interval = 0;

				FAuxiliaryControll AuxiliaryControll;

				APawn* UsePawn = nullptr;
				if (GIsEditor)
				{
					const TIndirectArray<FWorldContext>& AllWorlds = GEngine->GetWorldContexts();

					for (int32 i = AllWorlds.Num() - 1; i >= 0; i--)
					{
						UsePawn = UGameplayStatics::GetPlayerPawn(AllWorlds[i].World(), 0);
						if (UsePawn)
						{
							break;
						}
					}
				}else if (GWorld)
				{
					UsePawn = UGameplayStatics::GetPlayerPawn(GWorld, 0);
				}
		
				if (UsePawn)
				{
					AuxiliaryControll.PawnName = UsePawn->GetName();

					AuxiliaryControll.PawnLocation = UsePawn->GetActorLocation();
					AuxiliaryControll.PawnRotator = UsePawn->GetActorRotation().Euler();

					AuxiliaryControll.bHaveComponent = UsePawn->GetComponentByClass(UStepVrComponent::StaticClass()) != nullptr;

					auto Camera = Cast<UStepVrCameraComponent>(UsePawn->GetComponentByClass(UStepVrCameraComponent::StaticClass()));
					AuxiliaryControll.bHaveCamera = Camera != nullptr;
					if (Camera)
					{
						FTransform CameraTransform = Camera->GetComponentToWorld();
						AuxiliaryControll.CameraLocation = CameraTransform.GetLocation();
						AuxiliaryControll.CameraRotator = CameraTransform.GetRotation().Euler();

						AuxiliaryControll.bHaveMocap = false;
					}
				}
				else
				{
					AuxiliaryControll.PawnName = "None";
					AuxiliaryControll.bHaveComponent = false;
					AuxiliaryControll.bHaveCamera = false;
					AuxiliaryControll.bHaveMocap = false;
				}

				StepVrAuxiliaryUDP->SendControllData(AuxiliaryControll);
			}
		}
	}
}

bool FStepVrAuxiliaryCollection::NeedCollection()
{
	return StepVrAuxiliaryUDP.IsValid() && (StepVrAuxiliaryUDP->GetSendType() == EType_Connect);
}
