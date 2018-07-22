// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "AnimNode_StepVrGlove.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "StepVrPlugin/Public/StepVrBPLibrary.h"
#include "StepVrPlugin/Public/StepVrGlobal.h"


void FAnimNode_StepVrGlove::UpdateComponentPose_AnyThread(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateComponentPose_AnyThread(Context);

	if (!StepVrSDKIsValid())
	{
		return;
	}

	//TODO:查看手套是否开启
	StepVR::Frame tmp = STEPVR_FRAME->GetFrame();
	StepVR::SpringData TempData = tmp.GetSpringData();

	FQuat TempQuat;
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Thumb_Up, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Thumb_Up)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Thumb_Down, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Thumb_Down)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Index, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Index)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Middle, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Middle)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Ring, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Ring)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Left_Pinky, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Left_Pinky)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Thumb_Up, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Thumb_Up)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Thumb_Down, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Thumb_Down)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Index, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Index)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Middle, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Middle)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Ring, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Ring)) = TempQuat;
	}
	{
		UStepVrBPLibrary::SVGetGloveState(&TempData, EStepVRGloveType::Right_Pinky, TempQuat);
		GloveData.FindOrAdd(int32(EStepVRGloveType::Right_Pinky)) = TempQuat;
	}
}

void FAnimNode_StepVrGlove::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	if (Output.Pose.GetPose().GetNumBones() <= 0)
	{
		return;
	}

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	int32 LeftIndex = int32(EStepVRGloveType::Left_Thumb_Up);

	for (FBoneReference& Temp : LeftBoneToModifys)
	{
		//手套数据
		FQuat* TempQuat = GloveData.Find(LeftIndex);
		if (TempQuat == nullptr)
		{
			continue;
		}

		FCompactPoseBoneIndex CompactPoseBoneToModify = Temp.GetCompactPoseIndex(BoneContainer);
		FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
		FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);

		//重置手套姿态
		NewBoneTM.SetRotation(*TempQuat);
	
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);
		OutBoneTransforms.Add(FBoneTransform(CompactPoseBoneToModify, NewBoneTM));

		++LeftIndex;
	}

	int32 RightIndex = int32(EStepVRGloveType::Right_Thumb_Up);
	for (FBoneReference& Temp : RightBoneToModifys)
	{
		//手套数据
		FQuat* TempQuat = GloveData.Find(RightIndex);
		if (TempQuat == nullptr)
		{
			continue;
		}

		FCompactPoseBoneIndex CompactPoseBoneToModify = Temp.GetCompactPoseIndex(BoneContainer);
		FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
		FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);

		//重置手套姿态
		NewBoneTM.SetRotation(*TempQuat);

		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);
		OutBoneTransforms.Add(FBoneTransform(CompactPoseBoneToModify, NewBoneTM));

		++RightIndex;
	}


	OutBoneTransforms.Sort([](const FBoneTransform& A, const FBoneTransform& B)
	{
		return A.BoneIndex < B.BoneIndex;
	});

	//if (RotationMode != BMM_Ignore)
	//{
	//	// Convert to Bone Space.
	//	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, RotationSpace);

	//	const FQuat BoneQuat(Rotation);
	//	if (RotationMode == BMM_Additive)
	//	{
	//		NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());
	//	}
	//	else
	//	{
	//		NewBoneTM.SetRotation(BoneQuat);
	//	}	
}

bool FAnimNode_StepVrGlove::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	//未绑定骨架
	if (Skeleton == nullptr)
	{
		return false;
	}
	//SDK未启动
	if (!StepVrSDKIsValid())
	{
		return false;
	}
	//数据不全
	if (GloveData.Num() != int32(EStepVRGloveType::MAX))
	{
		return false;
	}
	//绑定骨架不足
	return (LeftBoneToModifys.Num() == 6) &&(RightBoneToModifys.Num() == 6);
}

void FAnimNode_StepVrGlove::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	for (FBoneReference& Temp : LeftBoneToModifys)
	{
		Temp.Initialize(RequiredBones);
	}
	for (FBoneReference& Temp : RightBoneToModifys)
	{
		Temp.Initialize(RequiredBones);
	}
}

bool FAnimNode_StepVrGlove::StepVrSDKIsValid()
{
	StepVrGlobal* Temp = StepVrGlobal::Get();

	if (Temp == nullptr)
	{
		return false;
	}

	return Temp->SDKIsValid();
}

