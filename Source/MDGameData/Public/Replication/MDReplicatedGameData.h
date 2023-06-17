#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MDReplicatedGameData.generated.h"

struct FMDGameDataEntry;
class UMDGameDataComponent;
class UMDGameDataContainer;

USTRUCT()
struct FMDReplicatedGameDataEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FMDReplicatedGameDataEntry() = default;

	FMDReplicatedGameDataEntry(const FMDReplicatedGameDataEntry& Other);
	FMDReplicatedGameDataEntry& operator=(const FMDReplicatedGameDataEntry& Other);

	FMDReplicatedGameDataEntry(FMDReplicatedGameDataEntry&& Other) noexcept;
	FMDReplicatedGameDataEntry& operator=(FMDReplicatedGameDataEntry&& Other) noexcept;

	~FMDReplicatedGameDataEntry();

	void PopulateFromEntry(const FMDGameDataEntry& Entry);

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	static bool NetSerializeProperty(const FProperty* Property, void* ValuePtr, FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	void CleanUpEntry();

	void PreReplicatedRemove(const FMDReplicatedGameData& Serializer);
	void PostReplicatedAdd(const FMDReplicatedGameData& Serializer);
	void PostReplicatedChange(const FMDReplicatedGameData& Serializer);

	FString GetDebugString() const;

	UPROPERTY()
	FGameplayTag EntryKey;

	FProperty* EntryProperty = nullptr;

	void* EntryValuePtr = nullptr;
};

template<>
struct TStructOpsTypeTraits<FMDReplicatedGameDataEntry> : public TStructOpsTypeTraitsBase2<FMDReplicatedGameDataEntry>
{
	enum
	{
		WithNetSerializer = true
	};
};

// We leverage FFastArraySerializer's Pre/Post functions to modify the local game data container on the client
USTRUCT()
struct FMDReplicatedGameData : public FFastArraySerializer
{
	GENERATED_BODY()

	friend class UMDGameDataComponent;

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);

#if WITH_SERVER_CODE
	void AddOrUpdateEntry(const FGameplayTag& EntryKey, const FMDGameDataEntry& RawEntry);
	void RemoveEntry(const FGameplayTag& EntryKey);
#endif

	UMDGameDataContainer* GetGameDataContainer() const;

protected:
	UPROPERTY()
	TArray<FMDReplicatedGameDataEntry> GameDataEntries;

private:
	TWeakObjectPtr<UMDGameDataComponent> GameDataComponent;
};

template<>
struct TStructOpsTypeTraits<FMDReplicatedGameData> : public TStructOpsTypeTraitsBase2<FMDReplicatedGameData>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
