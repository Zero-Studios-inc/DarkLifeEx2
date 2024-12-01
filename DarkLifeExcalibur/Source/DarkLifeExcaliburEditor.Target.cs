// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class DarkLifeExcaliburEditorTarget : TargetRules
{
	public DarkLifeExcaliburEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		//DefaultBuildSettings = BuildSettingsVersion.V2;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		CppStandard = CppStandardVersion.Cpp20;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		//BuildEnvironment = TargetBuildEnvironment.Unique;
		//bOverrideBuildEnvironment = true;
        ExtraModuleNames.AddRange( new string[] { "DarkLifeExcalibur" } );
	}
}
