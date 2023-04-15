using UnrealBuildTool;

public class MDGameDataGraph : ModuleRules
{
    public MDGameDataGraph(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
	            "BlueprintGraph",
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "MDGameData",
                "Slate",
                "SlateCore"
            }
        );
    }
}