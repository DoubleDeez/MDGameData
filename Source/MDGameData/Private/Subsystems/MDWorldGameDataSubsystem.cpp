#include "Subsystems/MDWorldGameDataSubsystem.h"

#include "MDGameDataContainer.h"
#include "Engine/Engine.h"
#include "Util/MDGameDataConfig.h"

UMDWorldGameDataSubsystem* UMDWorldGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine != nullptr)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return UWorld::GetSubsystem<UMDWorldGameDataSubsystem>(World);
		}
	}
	
	return nullptr;
}

void UMDWorldGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();
	check(!Config->GameDataContainerClass.IsNull());
	
	GameDataContainer = NewObject<UMDGameDataContainer>(this, Config->GameDataContainerClass.LoadSynchronous());
	const FMDGameDataContainerInitData InitData = {
		Config->InitialWorldGameDataReserveSize
	};

	GameDataContainer->InitializeGameDataContainer(InitData);
}

void UMDWorldGameDataSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (GameDataContainer != nullptr)
	{
		GameDataContainer->ShutdownGameDataContainer();
		GameDataContainer = nullptr;
	}
}

UMDGameDataContainer* UMDWorldGameDataSubsystem::GetWorldGameDataContainer(const UObject* WorldContextObject)
{
	if (const UMDWorldGameDataSubsystem* Subsystem = Get(WorldContextObject))
	{
		return Subsystem->GetGameDataContainer();
	}

	return nullptr;
}
