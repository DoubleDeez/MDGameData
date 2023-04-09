﻿using UnrealBuildTool;

public class MDGameData : ModuleRules
{
	public MDGameData(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"BlueprintGraph",
				"Core",
				"DeveloperSettings",
				"GameplayTags",
				"StructUtils"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
		);
	}
}