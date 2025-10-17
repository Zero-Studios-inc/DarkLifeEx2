using UnrealBuildTool;
using System.Collections.Generic;

public class DarkLifeExcaliburEditorTarget : TargetRules
{
    public DarkLifeExcaliburEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        // Alinea con los defaults de UE 5.6 para callar los warnings de upgrade
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
        CppStandard = CppStandardVersion.Cpp20;

        // IMPORTANTE: No sobreescribas WindowsPlatform.bStrictConformanceMode aquí.
        // Deja el valor por defecto (true en 5.6). Compila el código bajo /permissive-.

        ExtraModuleNames.AddRange(new string[] { "DarkLifeExcalibur" });
    }
}

