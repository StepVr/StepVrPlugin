using System;
using System.IO;
using UnrealBuildTool;




public class StepVrPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string LibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty")); }
    }

    public string GetLibFullPath()
    {
        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x32";
        string LibrariesPath = Path.Combine(LibPath, "lib", PlatformString);
        return LibrariesPath;
    }

    public void ReferenceDlls()
    {
        string Path = GetLibFullPath();
        var DllFiles = Directory.GetFiles(Path, "*.dll");
        foreach (var file in DllFiles)
        {
            RuntimeDependencies.Add(file);
            Console.WriteLine(file);
        }
    }
    public StepVrPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //外部链接头文件目录
        PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModulePath, "Classes"),
                Path.Combine(ModulePath, "Public"),
                Path.Combine(LibPath,"include"),
				// ... add public include paths required here ...
			}
			);
				
		
        //本模块连接头文件路径
		PrivateIncludePaths.AddRange(
			new string[] {
                "StepVrPlugin/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Projects",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "Projects",
                "InputCore",
                "InputDevice",
                "HeadMountedDisplay",
                "Networking",
                "Sockets",
                "Slate",
				// ... add private dependencies that you statically link with here ...	
			}
			);

        //配置DLL
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateIncludePaths.Add(Path.Combine(LibPath, "include"));
            PublicIncludePaths.Add(Path.Combine(LibPath, "Include"));

            //DLL路径
            string LibrariesPath = GetLibFullPath();

            PublicDelayLoadDLLs.Add("StepVR.dll");
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "StepVr.lib"));

            PublicDelayLoadDLLs.Add("license.dll");
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "license.lib"));

            //依赖DLL
            ReferenceDlls();
        }
    }


}
