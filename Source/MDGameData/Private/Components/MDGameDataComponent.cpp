#include "Components/MDGameDataComponent.h"

#include "MDGameDataContainer.h"
#include "Util/MDGameDataConfig.h"


void UMDGameDataComponent::BeginPlay()
{
	Super::BeginPlay();

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();
	check(!Config->GameDataContainerClass.IsNull());
	
	GameDataContainer = NewObject<UMDGameDataContainer>(this, Config->GameDataContainerClass.LoadSynchronous());
	const FMDGameDataContainerInitData InitData = {
		InitialGameDataReserveSize
	};

	GameDataContainer->InitializeGameDataContainer(InitData);
}

