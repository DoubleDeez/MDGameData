#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "MDWorldGameDataSubsystem.generated.h"

class UMDGameDataContainer;

/**
 *
 */
UCLASS()
class MDGAMEDATA_API UMDWorldGameDataSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UMDWorldGameDataSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "Game Data", meta = (WorldContext = "WorldContextObject"))
	static UMDGameDataContainer* GetWorldGameDataContainer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Game Data")
	UMDGameDataContainer* GetGameDataContainer() const { return GameDataContainer; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UMDGameDataContainer> GameDataContainer = nullptr;
};
