
using UnrealBuildTool;

public class Velu_Case_Study_V1 : ModuleRules
{
	public Velu_Case_Study_V1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PrivateDependencyModuleNames.AddRange(new string[] {  });
        // ? Required for Windows socket support
        PublicAdditionalLibraries.Add("ws2_32.lib");
        
    }
}
