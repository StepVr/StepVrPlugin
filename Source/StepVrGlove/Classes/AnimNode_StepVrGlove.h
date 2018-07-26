// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_StepVrGlove.generated.h"


USTRUCT(BlueprintInternalUseOnly)
struct STEPVRGLOVE_API FAnimNode_StepVrGlove : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** 左手六个关节
	0 Left_Thumb_Up ,
	1 Left_Thumb_Down,
	2 Left_Index,
	3 Left_Middle,
	4 Left_Ring,
	5 Left_Pinky,
	**/
	UPROPERTY(EditAnywhere, Category = "SkeletalControl")
	TArray<FBoneReference> LeftBoneToModifys;
	
	/** 右手六个关节
	0 Right_Thumb_Up,
	1 Right_Thumb_Down,
	2 Right_Index,
	3 Right_Middle,
	4 Right_Ring,
	5 Right_Pinky,
	**/
	UPROPERTY(EditAnywhere, Category = "SkeletalControl")
	TArray<FBoneReference> RightBoneToModifys;
	
protected:

	// FAnimNode_SkeletalControlBase interface
	virtual void UpdateComponentPose_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// return true if it is valid to Evaluate
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// initialize any bone references you have
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	bool StepVrSDKIsValid();
private:
	TMap<int32, FQuat> GloveData;
};