#pragma once

#include "Engine/DeveloperSettings.h"
#include "MDGameDataConfig.generated.h"

class UMDGameDataContainer;
/**
 *
 */
UCLASS(DefaultConfig, Config=GameData)
class MDGAMEDATA_API UMDGameDataConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMDGameDataConfig();

	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	TSoftClassPtr<UMDGameDataContainer> GameDataContainerClass;

	// How many global game data entries to initially reserve memory for
	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	int32 InitialGlobalGameDataReserveSize = 128;

	// How many local player game data entries to initially reserve memory for
	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	int32 InitialLocalPlayerGameDataReserveSize = 64;

	// How many world game data entries to initially reserve memory for
	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	int32 InitialWorldGameDataReserveSize = 64;
};
