#include "Core/Handheld/HandheldAutoConfigSubsystem.h"

#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformMisc.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"

static const TCHAR* kHandheldSection = TEXT("/Script/DarkLifeExcalibur.HandheldAutoConfig");
static const TCHAR* kAppliedKey = TEXT("bROGAllyApplied");

static void SetCVarInt(const TCHAR* Name, int32 Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, ECVF_SetByGameSetting);
	}
}

static void SetCVarFloat(const TCHAR* Name, float Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, ECVF_SetByGameSetting);
	}
}

FString UHandheldAutoConfigSubsystem::Lower(const FString& S)
{
	return S.ToLower();
}

bool UHandheldAutoConfigSubsystem::ContainsAny(const FString& HaystackLower, const TArray<FString>& NeedlesLower)
{
	for (const FString& N : NeedlesLower)
	{
		if (!N.IsEmpty() && HaystackLower.Contains(N))
		{
			return true;
		}
	}
	return false;
}

void UHandheldAutoConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if PLATFORM_WINDOWS
	if (WasAlreadyApplied())
	{
		return;
	}

	if (IsLikelyROGAlly())
	{
		ApplyROGAllyProfile();
		MarkApplied();
	}
#endif
}

bool UHandheldAutoConfigSubsystem::WasAlreadyApplied() const
{
	bool bApplied = false;
	GConfig->GetBool(kHandheldSection, kAppliedKey, bApplied, GGameUserSettingsIni);
	return bApplied;
}

void UHandheldAutoConfigSubsystem::MarkApplied() const
{
	GConfig->SetBool(kHandheldSection, kAppliedKey, true, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

bool UHandheldAutoConfigSubsystem::IsLikelyROGAlly() const
{
	const FString GPUB = Lower(FPlatformMisc::GetPrimaryGPUBrand());
	const FString CPUB = Lower(FPlatformMisc::GetCPUBrand());

	const TArray<FString> GPUNeedles = {
		TEXT("780m"),
		TEXT("radeon 780m"),
		TEXT("amd radeon"),
		TEXT("radeon(tm) graphics")
	};

	const TArray<FString> CPUNeedles = {
	TEXT("ryzen z1"),
	TEXT("z1 extreme"),
	TEXT("amd ryzen z1"),
	TEXT("ryzen z1") // <-- reemplaza el de ™ por esto
	};

	const bool bLooksLikeAllyCPU = ContainsAny(CPUB, CPUNeedles);
	const bool bLooksLikeAllyGPU = ContainsAny(GPUB, GPUNeedles);

	if (bLooksLikeAllyCPU)
	{
		return true;
	}

	// Fallback (menos estricto): si vemos 780M, lo tratamos como handheld preset
	if (bLooksLikeAllyGPU)
	{
		return true;
	}

	return false;
}

void UHandheldAutoConfigSubsystem::ApplyROGAllyProfile()
{
	// ====== Preset ROG Ally 60 FPS (CVar-based) ======

	// Iluminación/reflejos: sin Lumen
	SetCVarInt(TEXT("r.DynamicGlobalIlluminationMethod"), 0);
	SetCVarInt(TEXT("r.ReflectionMethod"), 0);
	SetCVarInt(TEXT("r.RayTracing"), 0);
	SetCVarInt(TEXT("r.Lumen.HardwareRayTracing"), 0);
	SetCVarInt(TEXT("r.GenerateMeshDistanceFields"), 0);

	// Sombras: sin VSM
	SetCVarInt(TEXT("r.Shadow.Virtual.Enable"), 0);
	SetCVarInt(TEXT("r.Shadow.MaxResolution"), 1536);
	SetCVarInt(TEXT("r.Shadow.CSM.MaxCascades"), 2);
	SetCVarInt(TEXT("r.Shadow.Quality"), 3);
	SetCVarInt(TEXT("r.ContactShadows"), 0);
	SetCVarFloat(TEXT("r.Shadow.RadiusThreshold"), 0.03f);

	// Nanite con límites
	SetCVarInt(TEXT("r.Nanite"), 1);
	SetCVarInt(TEXT("r.Nanite.MaxTrianglesPerFrame"), 1800000);
	SetCVarInt(TEXT("r.Nanite.MaxRasterizedPrimsPerFrame"), 1800000);

	// TSR agresivo
	SetCVarInt(TEXT("r.AntiAliasingMethod"), 3);
	SetCVarInt(TEXT("r.TSR.Quality"), 3);
	SetCVarInt(TEXT("r.PostProcessAAQuality"), 5);
	SetCVarFloat(TEXT("r.ScreenPercentage"), 75.0f);
	SetCVarFloat(TEXT("r.Tonemapper.Sharpen"), 0.05f);

	// Fog y post recortado
	SetCVarInt(TEXT("r.VolumetricFog"), 0);
	SetCVarInt(TEXT("r.SSR.Quality"), 2);
	SetCVarInt(TEXT("r.SSR.Temporal"), 1);
	SetCVarInt(TEXT("r.AmbientOcclusionLevels"), 2);
	SetCVarInt(TEXT("r.AmbientOcclusionMaxQuality"), 60);
	SetCVarInt(TEXT("r.BloomQuality"), 4);
	SetCVarInt(TEXT("r.DepthOfFieldQuality"), 2);
	SetCVarInt(TEXT("r.MotionBlurQuality"), 0);
	SetCVarInt(TEXT("r.SceneColorFringeQuality"), 0);

	// Streaming iGPU
	SetCVarInt(TEXT("r.Streaming.LimitPoolSizeToVRAM"), 1);
	SetCVarInt(TEXT("r.Streaming.PoolSize"), 1100);
	SetCVarInt(TEXT("r.Streaming.MaxTempMemoryAllowed"), 64);

	// Distancias / LOD
	SetCVarFloat(TEXT("r.ViewDistanceScale"), 0.9f);
	SetCVarFloat(TEXT("r.StaticMeshLODDistanceScale"), 1.15f);
	SetCVarInt(TEXT("r.SkeletalMeshLODBias"), 1);

	// 60 FPS
	SetCVarInt(TEXT("t.MaxFPS"), 60);

	// Scalability (opcional, pero ayuda)
	if (UGameUserSettings* GS = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		// 0 Low, 1 Medium, 2 High, 3 Epic, 4 Cinematic
		GS->SetOverallScalabilityLevel(2);
		GS->ApplySettings(false);
		GS->SaveSettings();
	}

	UE_LOG(LogTemp, Log, TEXT("[HandheldAutoConfig] Applied ROG Ally preset (CVar-based) @60 FPS."));
}
