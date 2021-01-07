#include "StepVrInput.h"
#include "Features/IModularFeatures.h"
#include "StepVrGlobal.h"



#define LOCTEXT_NAMESPACE "StepVrInput"

#define StepVRCategory				"StepVRSubCategory"
#define StepVRCategoryName			"StepVR"
#define StepVRCategoryFriendlyName	"StepVR"



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
}

FStepVrInput::~FStepVrInput()
{
	/** Unregister Module */
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

void FStepVrInput::Tick(float DeltaTime)
{
	/**
	 * 初始化
	 */
	//{
	//	static bool IsInitialize = false;
	//	if (IsInitialize == false)
	//	{
	//		Initialize();
	//		IsInitialize = true;
	//	}
	//}
}

bool FStepVrInput::IsGamepadAttached() const
{
	return true;
}

ETrackingStatus FStepVrInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
	return ETrackingStatus::Tracked;
}

void FStepVrInput::RegisterMotionPair()
{
	m_MotionPair.Add((uint8)EControllerHand::Left, (int32)StepVrDeviceID::DLeftController);
	m_MotionPair.Add((uint8)EControllerHand::Right, (int32)StepVrDeviceID::DRightController);
	m_MotionPair.Add((uint8)EControllerHand::Gun, (int32)StepVrDeviceID::DGun);
}

void FStepVrInput::EventMocapHand(StepVR::SingleNode& Node, FStepVrButtonState& ButtonState, int DeviceID)
{
	do 
	{
		if (Node.GetKeyUp1(DeviceID, SDKKEYID(ButtonState.KeyID)))
		{
			UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Relese!"), DeviceID, *ButtonState.key.ToString());
			MessageHandler->OnControllerButtonReleased(ButtonState.key, 0, false);
			break;
		}
		if (Node.GetKeyDown1(DeviceID, SDKKEYID(ButtonState.KeyID)))
		{
			UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Press!"), DeviceID, *ButtonState.key.ToString());
			MessageHandler->OnControllerButtonPressed(ButtonState.key, 0, false);
			break;
		}
		//if (Node.GetKey1(DeviceID, SDKKEYID(ButtonState.KeyID)))
		//{
		//	UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Repeat!"), DeviceID, *ButtonState.Repeat.ToString());
		//	MessageHandler->OnControllerButtonPressed(ButtonState.Repeat, 0, false);
		//	break;
		//}
	} while (0);
}

void FStepVrInput::Initialize()
{
	/**
	* 清除其他Controller，以防冲突
	*/
	{
		TArray<IMotionController*> _Arrays = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
		for (int32 i = 0; i < _Arrays.Num(); i++)
		{
			//if (_Arrays[i] != this)
			//{
			//	IModularFeatures::Get().UnregisterModularFeature(IMotionController::GetModularFeatureName(), _Arrays[i]);
			//}
		}
	}

	/**
	 * 
	 */
}

void FStepVrInput::SendControllerEvents()
{
#if SHOW_STATE
	SCOPE_CYCLE_COUNTER(Stat_StepVrInput_ControllerEvent);
#endif

	if (!STEPVR_FRAME_ENABLE)
	{
		return;
	}

	StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();

	const double CurrentTime = FPlatformTime::Seconds();

	int32 PreDeviceID = 0;
	bool  IsLink = false;
	for (int32 i = 0; i < StateController.Devices.Num(); i++)
	{
		FStepVrButtonState& ButtonState = StateController.Devices[i];

		int32 DeviceID = ButtonState.DeviceID;
		if (DeviceID != PreDeviceID)
		{
			PreDeviceID = DeviceID;
			IsLink = Node.IsHardWareLink(SDKKEYID(DeviceID));
		}

		if (!IsLink)
		{
			continue;
		}

		//手套那件单独处理
		if (DeviceID == 49 || DeviceID == 56)
		{
			EventMocapHand(Node,ButtonState,DeviceID);
			continue;
		}


		switch (ButtonState.ActionState)
		{
		case EStepDeviceKeyType::State_Button:
		{
			uint8 flag = 0;
			flag = (Node.GetKeyUp(DeviceID, SDKKEYID(ButtonState.KeyID)) ? SButton_Release : 0x0) | flag;
			flag = (Node.GetKeyDown(DeviceID, SDKKEYID(ButtonState.KeyID)) ? SButton_Press : 0x0) | flag;
			flag = (Node.GetKey(DeviceID, SDKKEYID(ButtonState.KeyID)) ? SButton_Repeat : 0x0) | flag;

			if (flag != ButtonState.PressedState)
			{
				ButtonState.PressedState = flag;
				if (ButtonState.PressedState == SButton_Release)
				{
					UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Relese!"), DeviceID, *ButtonState.key.ToString());
					MessageHandler->OnControllerButtonReleased(ButtonState.key, 0, false);
				}
				if (ButtonState.PressedState == SButton_Press)
				{
					UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Press!"), DeviceID, *ButtonState.key.ToString());
					MessageHandler->OnControllerButtonPressed(ButtonState.key, 0, false);
					ButtonState.NextRepeatTime = m_fBtnRepeatTime + CurrentTime;
				}
			}
			//if (ButtonState.PressedState == SButton_Repeat && ButtonState.NextRepeatTime <= CurrentTime)
			//{
			//	UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Repeat!"), DeviceID, *ButtonState.key.ToString());
			//	MessageHandler->OnControllerButtonPressed(ButtonState.key, 0, false);
			//	ButtonState.NextRepeatTime = m_fBtnRepeatTime + CurrentTime;
			//}
		}
		break;
		case EStepDeviceKeyType::State_ValueX:
		{
			MessageHandler->OnControllerAnalog(ButtonState.key, 0, Node.GetJoyStickPosX(DeviceID));
		}
		break;
		case EStepDeviceKeyType::State_ValueY:
		{
			MessageHandler->OnControllerAnalog(ButtonState.key, 0, Node.GetJoyStickPosY(DeviceID));
		}
		break;
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
	auto _deviceID = m_MotionPair.Find((uint8)DeviceHand);
	if (_deviceID == nullptr)
	{
		return false;
	}
	
	//FTransform Transform;
	//UStepVrBPLibrary::SVGetDeviceStateWithID(*_deviceID, Transform);

	//OutOrientation = Transform.Rotator();
	//OutPosition = Transform.GetLocation();

	return true;
}

FName FStepVrInput::GetMotionControllerDeviceTypeName() const
{
	static FName GDeviceTypeName(TEXT("StepVrController"));
	return GDeviceTypeName;
}


bool FStepVrInput::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	GetOrientationAndPosition(ControllerIndex, DeviceHand, OutOrientation, OutPosition);

	return STEPVR_GLOBAL->UseStepMotionController;
}

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

void FStepVrInput::RegisterDeviceKey()
{
	EKeys::AddMenuCategoryDisplayInfo(StepVRCategoryName,
		LOCTEXT(StepVRCategory, StepVRCategoryFriendlyName),
		TEXT("GraphEditor.PadEvent_16x"));

	for (int32 Index = 0; Index < StateController.Devices.Num(); Index++)
	{
		FStepVrButtonState& ButtonState = StateController.Devices[Index];
		uint8 KeyFlag = ButtonState.ActionState == EStepDeviceKeyType::State_Button ?
			FKeyDetails::GamepadKey :
			FKeyDetails::FloatAxis;
		FText KeyText = FText::FromName(ButtonState.key);
		FKey TargetKey(ButtonState.key);
		EKeys::AddKey(FKeyDetails(TargetKey, KeyText, KeyFlag, StepVRCategoryName));
		//if (ButtonState.DeviceID == 56)
		//{
		//	ButtonState.Repeat = FName("StepVR_RightMocapHand_Repeat");
		//	FText KeyText1 = FText::FromName(ButtonState.Repeat);
		//	FKey  TargetKey1(ButtonState.Repeat);
		//	EKeys::AddKey(FKeyDetails(TargetKey1, KeyText1, KeyFlag, StepVRCategoryName));
		//}		
		//if (ButtonState.DeviceID == 49)
		//{
		//	ButtonState.Repeat = FName("StepVR_LeftMocapHand_Repeat");
		//	FText KeyText1 = FText::FromName(ButtonState.Repeat);
		//	FKey  TargetKey1(ButtonState.Repeat);
		//	EKeys::AddKey(FKeyDetails(TargetKey1, KeyText1, KeyFlag, StepVRCategoryName));
		//}
	}
}

#undef LOCTEXT_NAMESPACE

