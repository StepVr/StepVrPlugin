#include "StepVrInput.h"
#include "Features/IModularFeatures.h"
#include "StepVrBPLibrary.h"
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
	{
		static bool IsInitialize = false;
		if (IsInitialize == false)
		{
			Initialize();
			IsInitialize = true;
		}
	}
	

	/**
	 * 更新定位数据
	 */
	StepVR::SingleNode Node = STEPVR_FRAME->GetFrame().GetSingleNode();
	for (auto DevID : GNeedUpdateDevices)
	{
		FTransform TempData;
		UStepVrBPLibrary::SVGetDeviceStateWithID(&Node, DevID, TempData);
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

void FStepVrInput::Initialize()
{
	/**
	* 清除其他Controller，以防冲突
	*/
	{
		TArray<IMotionController*> _Arrays = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
		for (int32 i = 0; i < _Arrays.Num(); i++)
		{
			if (_Arrays[i] != this)
			{
				IModularFeatures::Get().UnregisterModularFeature(IMotionController::GetModularFeatureName(), _Arrays[i]);
			}
		}
	}

	/**
	 * 
	 */
}

void FStepVrInput::SendControllerEvents()
{
	if (!STEPVR_FRAME_IsValid)
	{
		return;
	}

	uint8 flag = 0x0;
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

		switch (ButtonState.ActionState)
		{
		case EStepActionState::State_Button:
		{
			flag = 0x0;
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
			if (ButtonState.PressedState == SButton_Repeat && ButtonState.NextRepeatTime <= CurrentTime)
			{
				UE_LOG(LogStepVrPlugin, Warning, TEXT("EquipID:%d,%s Repeat!"), DeviceID, *ButtonState.key.ToString());
				MessageHandler->OnControllerButtonPressed(ButtonState.key, 0, false);
				ButtonState.NextRepeatTime = m_fBtnRepeatTime + CurrentTime;
			}
		}
		break;
		case EStepActionState::State_ValueX:
		{
			MessageHandler->OnControllerAnalog(ButtonState.key, 0, Node.GetJoyStickPosX(DeviceID));
		}
		break;
		case EStepActionState::State_ValueY:
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
	
	FTransform Transform;
	UStepVrBPLibrary::SVGetDeviceStateWithID(*_deviceID, Transform);

	OutOrientation = Transform.Rotator();
	OutPosition = Transform.GetLocation();

	return true;
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

void FStepVrInput::RegisterDeviceKey()
{
	/**
	* add key binding
	*/
	EKeys::AddMenuCategoryDisplayInfo(StepVRCategoryName,
		LOCTEXT(StepVRCategory, StepVRCategoryFriendlyName),
		TEXT("GraphEditor.PadEvent_16x"));

	/**
	 * 枪按键
	 */
	FStepVRCapacitiveKey CapacitiveKey;
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_GunBtn_A_Trigger, 
		LOCTEXT("StepVR_1", "StepVR_GunBtn_A_Trigger"),
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_GunBtn_B_Trigger, 
		LOCTEXT("StepVR_2", "StepVR_GunBtn_B_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_GunBtn_C_Trigger, 
		LOCTEXT("StepVR_3", "StepVR_GunBtn_C_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_GunBtn_D_Trigger, 
		LOCTEXT("StepVR_4", "StepVR_GunBtn_D_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_Gun_ValueX, 
		LOCTEXT("StepVR_5", "StepVR_Gun_ValueX"), 
		FKeyDetails::FloatAxis,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_Gun_ValueY, 
		LOCTEXT("StepVR_6", "StepVR_Gun_ValueY"), 
		FKeyDetails::FloatAxis,
		StepVRCategoryName));

	/**
	 * 左手按键
	 */
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_LeftBtn_A_Trigger, 
		LOCTEXT("StepVR_10", "StepVR_LeftBtn_A_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_LeftBtn_B_Trigger, 
		LOCTEXT("StepVR_11", "StepVR_LeftBtn_B_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_LeftBtn_C_Trigger, 
		LOCTEXT("StepVR_12", "StepVR_LeftBtn_C_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));

	/**
	 * 右手按键
	 */
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_RightBtn_A_Trigger, 
		LOCTEXT("StepVR_20", "StepVR_RightBtn_A_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_RightBtn_B_Trigger, 
		LOCTEXT("StepVR_21", "StepVR_RightBtn_B_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_RightBtn_C_Trigger, 
		LOCTEXT("StepVR_22", "StepVR_RightBtn_C_Trigger"), 
		FKeyDetails::GamepadKey,
		StepVRCategoryName));

	/**
	 * 导演监视器
	 */
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_DirMon_A_Trigger,
		LOCTEXT("StepVR_31", "StepVR_DirMon_A_Trigger"),
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_DirMon_B_Trigger,
		LOCTEXT("StepVR_32", "StepVR_DirMon_B_Trigger"),
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_DirMon_C_Trigger,
		LOCTEXT("StepVR_33", "StepVR_DirMon_C_Trigger"),
		FKeyDetails::GamepadKey,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_DirMon_ValueX,
		LOCTEXT("StepVR_34", "StepVR_DirMon_ValueX"),
		FKeyDetails::FloatAxis,
		StepVRCategoryName));
	EKeys::AddKey(FKeyDetails(CapacitiveKey.StepVR_DirMon_ValueY,
		LOCTEXT("StepVR_35", "StepVR_DirMon_ValueY"),
		FKeyDetails::FloatAxis,
		StepVRCategoryName));
}

#undef LOCTEXT_NAMESPACE

