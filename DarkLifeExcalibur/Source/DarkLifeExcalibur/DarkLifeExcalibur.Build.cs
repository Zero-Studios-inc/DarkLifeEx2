// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class DarkLifeExcalibur : ModuleRules
{
    public DarkLifeExcalibur(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "AIModule",
            "InputCore",
            "LevelSequence",
            "MovieScene",
            "Niagara",
            "GameplayTags",
            "GameplayAbilities",
            "GameplayTasks",
            // Online Subsystem clásico en UE 5.2
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });

        // Si TIENES el plugin "OnlineSubsystemEOS" activado, descomenta UNA de las dos líneas:
        // (A) Como dependencia privada (si usas sus headers directamente)
        //PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystemEOS" });

        // (B) O cargarlo dinámicamente (si no incluyes headers de EOS)
        //DynamicallyLoadedModuleNames.AddRange(new string[] { "OnlineSubsystemEOS" });
    }
}
