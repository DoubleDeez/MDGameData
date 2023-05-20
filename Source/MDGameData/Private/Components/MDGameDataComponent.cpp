#include "Components/MDGameDataComponent.h"

#include "GameplayTagsManager.h"
#include "MDGameDataContainer.h"
#include "Net/UnrealNetwork.h"
#include "Util/MDGameDataConfig.h"

UMDGameDataComponent::UMDGameDataComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDGameDataComponent::OnRegister()
{
	Super::OnRegister();

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();
	check(!Config->GameDataContainerClass.IsNull());
	
	GameDataContainer = NewObject<UMDGameDataContainer>(this, Config->GameDataContainerClass.LoadSynchronous());
	const FMDGameDataContainerInitData InitData = {
		InitialGameDataReserveSize
	};

	GameDataContainer->InitializeGameDataContainer(InitData);
	ReplicatedGameData.GameDataComponent = this;

#if WITH_SERVER_CODE
	if (GetOwner()->HasAuthority() && GetNetMode() != NM_Standalone)
	{
		InitContainerForReplication();
	}
#endif
}

void UMDGameDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicatedGameData);
}

void UMDGameDataComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMDGameDataComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(GameDataContainer))
	{
		GameDataContainer->ClearEntries();
	}
	
	Super::EndPlay(EndPlayReason);
}

#if WITH_SERVER_CODE
void UMDGameDataComponent::InitContainerForReplication()
{
	check(IsValid(GameDataContainer));

	GameDataContainer->OnAnyEntryChanged.AddUObject(this, &UMDGameDataComponent::ServerOnEntryChanged);
}

void UMDGameDataComponent::ServerOnEntryChanged(const FGameplayTag& EntryKey)
{
	check(IsValid(GameDataContainer));
	check(EntryKey.IsValid());

	if (const FMDGameDataEntry* RawEntryPtr = GameDataContainer->GetRawEntryPtr(EntryKey))
	{
		ReplicatedGameData.AddOrUpdateEntry(EntryKey, *RawEntryPtr);
	}
	else
	{
		ReplicatedGameData.RemoveEntry(EntryKey);
	}
}
#endif

