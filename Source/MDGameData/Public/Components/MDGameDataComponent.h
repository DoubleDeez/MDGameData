#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Replication/MDReplicatedGameData.h"
#include "MDGameDataComponent.generated.h"

struct FMDGameDataEntry;
struct FGameplayTag;
class UMDGameDataContainer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDGAMEDATA_API UMDGameDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDGameDataComponent();

	virtual void OnRegister() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Game Data")
	UMDGameDataContainer* GetGameDataContainer() const { return GameDataContainer; }

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Game Data")
	int32 InitialGameDataReserveSize = 8;

private:
#if WITH_SERVER_CODE
	void InitContainerForReplication();

	void ServerOnEntryChanged(const FGameplayTag& EntryKey);
#endif

	UPROPERTY(Replicated)
	FMDReplicatedGameData ReplicatedGameData;

	UPROPERTY(Transient)
	TObjectPtr<UMDGameDataContainer> GameDataContainer = nullptr;
};
