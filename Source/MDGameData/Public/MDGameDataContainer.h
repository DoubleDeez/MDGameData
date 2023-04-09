#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MDGameDataContainer.generated.h"

struct FMDGameDataEntry
{
	explicit FMDGameDataEntry(const FProperty* SourceProp);
	
	FMDGameDataEntry(const FMDGameDataEntry& Other) = delete;
	FMDGameDataEntry& operator=(const FMDGameDataEntry& Other) = delete;
	
	FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept;
	FMDGameDataEntry& operator=(FMDGameDataEntry&& Other) noexcept;
	
	~FMDGameDataEntry();

	const FProperty* EntryProperty = nullptr;

	void* EntryValuePtr = nullptr;
};

struct FMDGameDataContainerInitData
{
	int32 InitialMapReserve = 128;
};

UENUM()
enum class EMDGameDataContainerResult : uint8
{
	Success_NewEntry,
	Success_ExistingEntry,
	Failure_InvalidInputs,
	Failure_NoEntry,
	Failure_TypeMismatch
};

/**
 * Core object to hold Game Data Key-Value pairs
 */
UCLASS()
class MDGAMEDATA_API UMDGameDataContainer : public UObject
{
	GENERATED_BODY()

public:
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	void InitializeGameDataContainer(const FMDGameDataContainerInitData& InitData);
	void ShutdownGameDataContainer();
	
	TTuple<const FProperty*, const void*> GetData(const FGameplayTag& DataKey) const;

	EMDGameDataContainerResult SetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr);
	EMDGameDataContainerResult GetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr) const;

protected:
	TMap<FGameplayTag, FMDGameDataEntry> DataEntries;
};
