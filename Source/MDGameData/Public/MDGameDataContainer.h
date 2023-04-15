#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDGameDataEntry.h"
#include "UObject/Object.h"
#include "Util/MDGameDataUtils.h"
#include "MDGameDataContainer.generated.h"

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

	template<typename T>
	EMDGameDataContainerResult SetData(const FGameplayTag& DataKey, const T& Value);
	template<typename T>
	EMDGameDataContainerResult GetData(const FGameplayTag& DataKey, T& OutResult) const;

protected:
	TMap<FGameplayTag, FMDGameDataEntry> DataEntries;
};

template <typename T>
EMDGameDataContainerResult UMDGameDataContainer::SetData(const FGameplayTag& DataKey, const T& Value)
{
	if (!DataKey.IsValid())
	{
		return EMDGameDataContainerResult::Failure_InvalidInputs;
	}
	
	const FProperty* Prop = MDGameDataUtils::GetPropertyForType<T>();
	if (const FMDGameDataEntry* Entry = DataEntries.Find(DataKey))
	{
		const bool bDoesTypeMatch = ensureMsgf(Entry->EntryProperty->SameType(Prop), TEXT("Type [%s] does not match existing type [%s] with key [%s]"),
			ANSI_TO_TCHAR(typeid(T).name()),
			*Entry->EntryProperty->GetCPPType(),
			*DataKey.ToString()
		);
		if (!bDoesTypeMatch)
		{
			return EMDGameDataContainerResult::Failure_TypeMismatch;
		}

		Entry->EntryProperty->CopyCompleteValue(Entry->EntryValuePtr, &Value);
		return EMDGameDataContainerResult::Success_ExistingEntry;
	}
	else
	{
		const FMDGameDataEntry& NewEntry = DataEntries.Emplace(DataKey, FMDGameDataEntry(DataKey.GetTagName(), Prop));
		NewEntry.EntryProperty->CopyCompleteValue(NewEntry.EntryValuePtr, &Value);
		return EMDGameDataContainerResult::Success_NewEntry;
	}
}

template <typename T>
EMDGameDataContainerResult UMDGameDataContainer::GetData(const FGameplayTag& DataKey, T& OutResult) const
{
	const TTuple<const FProperty*, const void*> Entry = GetData(DataKey);
	if (Entry.Key == nullptr || Entry.Value == nullptr)
	{
		return EMDGameDataContainerResult::Failure_NoEntry;
	}

	const FProperty* Prop = MDGameDataUtils::GetPropertyForType<T>();
	const bool bDoesTypeMatch = ensureMsgf(Entry.Key->SameType(Prop), TEXT("Type [%s] does not match type [%s] with key [%s]"),
		ANSI_TO_TCHAR(typeid(T).name()),
		*Entry.Key->GetCPPType(),
		*DataKey.ToString()
	);
	if (!bDoesTypeMatch)
	{
		return EMDGameDataContainerResult::Failure_TypeMismatch;
	}

	Entry.Key->CopyCompleteValue(&OutResult, Entry.Value);
	return EMDGameDataContainerResult::Success_ExistingEntry;
}
