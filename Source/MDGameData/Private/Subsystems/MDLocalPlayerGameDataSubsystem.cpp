#include "Subsystems/MDLocalPlayerGameDataSubsystem.h"

#include "MDGameDataContainer.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Util/MDGameDataConfig.h"

UMDLocalPlayerGameDataSubsystem* UMDLocalPlayerGameDataSubsystem::GetForFirstLocalPlayer(const UObject* WorldContextObject)
{
	if (GEngine != nullptr)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (const UGameInstance* GameInstance = World->GetGameInstance())
			{
				for (const ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
				{
					if (LocalPlayer != nullptr)
					{
						return ULocalPlayer::GetSubsystem<UMDLocalPlayerGameDataSubsystem>(LocalPlayer);
					}
				}
			}
		}
	}
	
	return nullptr;
}

void UMDLocalPlayerGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();
	check(!Config->GameDataContainerClass.IsNull());
	
	GameDataContainer = NewObject<UMDGameDataContainer>(this, Config->GameDataContainerClass.LoadSynchronous());
	const FMDGameDataContainerInitData InitData = {
		Config->InitialLocalPlayerGameDataReserveSize
	};

	GameDataContainer->InitializeGameDataContainer(InitData);
}

void UMDLocalPlayerGameDataSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (GameDataContainer != nullptr)
	{
		GameDataContainer->ShutdownGameDataContainer();
		GameDataContainer = nullptr;
	}
}

UMDGameDataContainer* UMDLocalPlayerGameDataSubsystem::GetFirstLocalPlayerGameDataContainer(const UObject* WorldContextObject)
{
	if (const UMDLocalPlayerGameDataSubsystem* Subsystem = GetForFirstLocalPlayer(WorldContextObject))
	{
		return Subsystem->GetGameDataContainer();
	}
	
	return nullptr;
}
