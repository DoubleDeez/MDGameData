#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MDGameDataConfig.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, Config=Game)
class MDGAMEDATA_API UMDGameDataConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// How many game data entries to initially reserve memory for
	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	int32 InitialGameDataReserveSize = 128;
};
