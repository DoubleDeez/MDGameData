#include "Subsystems/MDGlobalGameDataSubsystem.h"

#include "MDGameDataContainer.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/MDGameDataConfig.h"

UMDGlobalGameDataSubsystem* UMDGlobalGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine != nullptr)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return UGameInstance::GetSubsystem<UMDGlobalGameDataSubsystem>(World->GetGameInstance());
		}
	}
	
	return nullptr;
}

void UMDGlobalGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();
	check(!Config->GameDataContainerClass.IsNull());
	
	GameDataContainer = NewObject<UMDGameDataContainer>(this, Config->GameDataContainerClass.LoadSynchronous());
	const FMDGameDataContainerInitData InitData = {
		Config->InitialGlobalGameDataReserveSize
	};

	GameDataContainer->InitializeGameDataContainer(InitData);
}

void UMDGlobalGameDataSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (GameDataContainer != nullptr)
	{
		GameDataContainer->ShutdownGameDataContainer();
		GameDataContainer = nullptr;
	}
}

UMDGameDataContainer* UMDGlobalGameDataSubsystem::GetGlobalGameDataContainer(const UObject* WorldContextObject)
{
	if (const UMDGlobalGameDataSubsystem* Subsystem = Get(WorldContextObject))
	{
		return Subsystem->GetGameDataContainer();
	}

	return nullptr;
}
