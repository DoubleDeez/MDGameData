#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "MDLocalPlayerGameDataSubsystem.generated.h"

class UMDGameDataContainer;

/**
 *
 */
UCLASS()
class MDGAMEDATA_API UMDLocalPlayerGameDataSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UMDLocalPlayerGameDataSubsystem* GetForFirstLocalPlayer(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "Game Data", meta = (WorldContext = "WorldContextObject"))
	static UMDGameDataContainer* GetFirstLocalPlayerGameDataContainer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Game Data")
	UMDGameDataContainer* GetGameDataContainer() const { return GameDataContainer; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UMDGameDataContainer> GameDataContainer = nullptr;
};
