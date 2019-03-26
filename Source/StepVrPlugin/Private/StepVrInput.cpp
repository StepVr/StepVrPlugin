#include "StepVrInput.h"
#include "Features/IModularFeatures.h"
#include "StepVrBPLibrary.h"
#include "StepVrGlobal.h"



#define LOCTEXT_NAMESPACE "StepVrPlugin"

FStepVrInput::FStepVrInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler):
	MessageHandler(InMessageHandler),
	m_fBtnRepeatTime(0.15f),
	m_HandleDLL(nullptr)
{	
	/** Register Input Info */
	RegisterDeviceKey();

	/** Register MotionController Pair */
	RegisterMotionPair();

	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);

#if (!AFTER_ENGINEVERSION_411)
	GEngine->MotionControllerDevices.AddUnique(this);
#endif	
}

FStepVrInput::~FStepVrInput()
{
	/** Unregister Module */
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

void FStepVrInput::Tick(float DeltaTime)
{
	/** Clear other MotionController */
	static bool IsUnregist = false;
	static float DelayTime = 0.0f;

	if (!IsUnregist)
	{
		TArray<IMotionController*> _Arrays = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
		if (_Arrays.Num()<=1)
		{
			DelayTime += DeltaTime;
			if (DelayTime > 10) { IsUnregist = true; }
			return;
		}

		for (int32 i = 0; i < _Arrays.Num(); i++)
		{
			if (_Arrays[i] != this)
			{
				IModularFeatures::Get().UnregisterModularFeature(IMotionController::GetModularFeatureName(), _Arrays[i]);
			}
		}

		IsUnregist = true;
	}
}

void FStepVrInput::RegisterMotionPair()
{
	m_MotionPair.Add((uint8)EControllerHand::Left, (int32)StepVrDeviceID::DLeftController);
	m_MotionPair.Add((uint8)EControllerHand::Right, (int32)StepVrDeviceID::DRightController);
#if AFTER_ENGINEVERSION_414
	m_MotionPair.Add((uint8)EControllerHand::Gun, (int32)StepVrDeviceID::DGun);
#endif
	
}

void FStepVrInput::SendControllerEvents()
{
	if (!STEPVR_FRAME_IsValid)
	{ 
		return; 
	}

	uint8 flag = 0x0;
	FString ButtonName = TEXT("ABCDEF");
	StepVR::Frame tmp = STEPVR_FRAME->GetFrame();

	const double CurrentTime = FPlatformTime::Seconds();

	for (int32 i = 0; i < (int32)EStepVrDeviceId::DTotalCount; i++)
	{
		FStepVrDeviceState& device = ButtonState.Devices[i];
		for (int32 j = 0; j < device.TBtnKey.Num(); j++)
		{
			FStepVrButtonState& btnState = device.TBtnKey[j];
			flag = 0x0;
			flag = (tmp.GetSingleNode().GetKeyUp(device.EquipId, SDKKEYID(j + 1)) ? SButton_Release : 0x0) | flag;
			flag = (tmp.GetSingleNode().GetKeyDown(device.EquipId, SDKKEYID(j + 1)) ? SButton_Press : 0x0) | flag;
			flag = (tmp.GetSingleNode().GetKey(device.EquipId, SDKKEYID(j + 1)) ? SButton_Repeat : 0x0) | flag;
	
			if (flag != btnState.PressedState)
			{
				btnState.PressedState = flag;
				if (btnState.PressedState == SButton_Release)
				{
					UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,Button%c Relese!"),(int32)device.EquipId,ButtonName.GetCharArray()[j]);
					MessageHandler->OnControllerButtonReleased(btnState.key, 0, false);
				}
				if (btnState.PressedState == SButton_Press)
				{
					UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,Button%c Press!"), (int32)device.EquipId, ButtonName.GetCharArray()[j]);
					MessageHandler->OnControllerButtonPressed(btnState.key, 0, false);
					btnState.NextRepeatTime = m_fBtnRepeatTime + CurrentTime;
				}
			}
			if (btnState.PressedState == SButton_Repeat && btnState.NextRepeatTime <= CurrentTime)
			{
				UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,Button%c Repeat!"), (int32)device.EquipId, ButtonName.GetCharArray()[j]);
				MessageHandler->OnControllerButtonPressed(btnState.key, 0, false);
				btnState.NextRepeatTime = m_fBtnRepeatTime + CurrentTime;
			}
		}
	}	
}

void FStepVrInput::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FStepVrInput::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FStepVrInput::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	UE_LOG(LogStepVrPlugin,Warning,TEXT("SetChannelValue---%f"), Value);
}

void FStepVrInput::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
// 	if (!StepManagerIsEnable())
// 	{
// 		return;
// 	}

	//LeftHand IsVaild
	//UpdateVibration(values.LeftLarge, ButtonState.Devices[(int32)EStepVrDeviceId::DLeft].EquipId);

	//RightHand IsVaild
	//UpdateVibration(values.RightLarge, ButtonState.Devices[(int32)EStepVrDeviceId::Dright].EquipId);
}


bool FStepVrInput::GetOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
{
	bool _flag = false;

	if (!STEPVR_FRAME_IsValid) 
	{ 
		return _flag;
	}

	auto _deviceID = m_MotionPair.Find((uint8)DeviceHand);
	if (_deviceID != nullptr)
	{
		StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();

		FTransform Transform;
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, *_deviceID, Transform);

		OutOrientation = Transform.Rotator();
		OutPosition = Transform.GetLocation();

		_flag = true;
	}
	
	return _flag;
}

#if AFTER_ENGINEVERSION_416
FName FStepVrInput::S_DeviceTypeName(TEXT("StepVrController"));
FName FStepVrInput::GetMotionControllerDeviceTypeName() const
{
	return S_DeviceTypeName;
}
#endif

#if AFTER_ENGINEVERSION_415
bool FStepVrInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	return GetOrientationAndPosition(ControllerIndex,DeviceHand,OutOrientation,OutPosition);
}
#else
bool FStepVrInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
{
	return GetOrientationAndPosition(ControllerIndex, DeviceHand, OutOrientation, OutPosition);
}
#endif

//void FStepVrInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values)
//{
//
//}
//
//void FStepVrInput::GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const
//{
//	MinFrequency = 0.0f;
//	MaxFrequency = 1.0f;
//}
//
//float FStepVrInput::GetHapticAmplitudeScale() const
//{
//	return 1.0f;
//}

bool FStepVrInput::RegisterDeviceKey()
{
	/**
	* add key binding
	*/
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_GunBtn_A_Trigger, LOCTEXT("StepVR_GunBtn_A_Trigger", "StepVR_GunBtn_A_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_GunBtn_B_Trigger, LOCTEXT("StepVR_GunBtn_B_Trigger", "StepVR_GunBtn_B_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_GunBtn_C_Trigger, LOCTEXT("StepVR_GunBtn_C_Trigger", "StepVR_GunBtn_C_Trigger"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_LeftBtn_A_Trigger, LOCTEXT("StepVR_LeftBtn_A_Trigger", "StepVR_LeftBtn_A_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_LeftBtn_B_Trigger, LOCTEXT("StepVR_LeftBtn_B_Trigger", "StepVR_LeftBtn_B_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_LeftBtn_C_Trigger, LOCTEXT("StepVR_LeftBtn_C_Trigger", "StepVR_LeftBtn_C_Trigger"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_RightBtn_A_Trigger, LOCTEXT("StepVR_RightBtn_A_Trigger", "StepVR_RightBtn_A_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_RightBtn_B_Trigger, LOCTEXT("StepVR_RightBtn_B_Trigger", "StepVR_RightBtn_B_Trigger"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(ButtonState.MyKey.StepVR_RightBtn_C_Trigger, LOCTEXT("StepVR_RightBtn_C_Trigger", "StepVR_RightBtn_C_Trigger"), FKeyDetails::GamepadKey));
	return true;
}

#undef LOCTEXT_NAMESPACE

