// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class NifImporter : ModuleRules
{
	public NifImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		bUseRTTI = true;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"ThirdParty/nifly/include",
				"ThirdParty/nifly/external"
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"MeshDescription",
				"RawMesh",
				"StaticMeshDescription",
				"AssetTools",
				"Projects",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);

		// Add Nifly
		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
		{
			string niflibPath = Path.Combine(ModuleDirectory, "..\\ThirdParty\\nifly\\build\\src\\release\\nifly.lib");
			PublicAdditionalLibraries.Add(niflibPath);
		}
		else
		{
			throw new BuildException("Platform not yet supported by NifImporter.");
		}
	}
}
