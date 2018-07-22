using UnrealBuildTool;

public class StepVrEditor : ModuleRules
{
	public StepVrEditor(ReadOnlyTargetRules Target) : base(Target)
    {

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "AnimGraph",
            "BlueprintGraph",
            "StepVrGlove"
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            "Slate",
            "MessageLog"
        });   
    }
}