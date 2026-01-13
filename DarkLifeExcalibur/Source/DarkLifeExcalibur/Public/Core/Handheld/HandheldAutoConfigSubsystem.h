#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HandheldAutoConfigSubsystem.generated.h"

UCLASS()
class DARKLIFEEXCALIBUR_API UHandheldAutoConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	bool IsLikelyROGAlly() const;
	void ApplyROGAllyProfile();

	bool WasAlreadyApplied() const;
	void MarkApplied() const;

	static FString Lower(const FString& S);
	static bool ContainsAny(const FString& HaystackLower, const TArray<FString>& NeedlesLower);
};
