#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MDGlobalGameDataSubsystem.generated.h"

class UMDGameDataContainer;

/**
 * 
 */
UCLASS()
class MDGAMEDATA_API UMDGlobalGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UMDGlobalGameDataSubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "Game Data", meta = (WorldContext = "WorldContextObject"))
	static UMDGameDataContainer* GetGlobalGameDataContainer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Game Data")
	UMDGameDataContainer* GetGameDataContainer() const { return GameDataContainer; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UMDGameDataContainer> GameDataContainer = nullptr;
};
