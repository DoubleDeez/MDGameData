#pragma once

#include "CoreMinimal.h"
#include <typeinfo>
#include "GameplayTagContainer.h"
#include "MDGameDataEntry.h"
#include "UObject/Object.h"
#include "Util/MDGameDataAllocator.h"
#include "Util/MDGameDataUtils.h"
#include "Util/MDGameDataTypeUtils.h"
#include "MDGameDataContainer.generated.h"

DECLARE_DYNAMIC_DELEGATE(FMDOnGameDataEntryChanged);

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

	UFUNCTION(BlueprintCallable, Category = "Game Data", meta = (AutoCreateRefTerm = "DataKey"))
	void RemoveEntry(const FGameplayTag& DataKey);

	UFUNCTION(BlueprintCallable, Category = "Game Data", meta = (AutoCreateRefTerm = "DataKey"))
	void ClearEntries();

	UFUNCTION(BlueprintCallable, Category = "Game Data", meta = (AutoCreateRefTerm = "DataKey"))
	void BindOnEntryChanged(const FGameplayTag& DataKey, FMDOnGameDataEntryChanged Delegate);

	UFUNCTION(BlueprintCallable, Category = "Game Data", meta = (AutoCreateRefTerm = "DataKey"))
	void UnbindOnEntryChanged(const FGameplayTag& DataKey, FMDOnGameDataEntryChanged Delegate);

	template<typename T>
	FDelegateHandle BindOnEntryChangedDelegate(const FGameplayTag& DataKey, TDelegate<void(T)>&& Delegate);

	FDelegateHandle BindOnEntryChangedDelegate(const FGameplayTag& DataKey, FSimpleDelegate&& Delegate);
	void UnbindOnEntryChangedDelegate(const FGameplayTag& DataKey, const FDelegateHandle& DelegateHandle);

	const FMDGameDataEntry* GetRawEntryPtr(const FGameplayTag& DataKey) const; 

	FString GetEntryTypeString(const FGameplayTag& DataKey) const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntryEvent, const FGameplayTag&);
	FOnEntryEvent OnAnyEntryChanged;

protected:
	const FMDGameDataEntry& AddEntry(const FGameplayTag& DataKey, FMDGameDataEntry&& Entry);
	void BroadcastEntryChanged(const FGameplayTag& DataKey) const;

	template<typename T>
	void NotifyNewEntryValue(FGameplayTag DataKey, TDelegate<void(T)> Delegate);

	FMDGameDataAllocator Allocator;
	TMap<FGameplayTag, FMDGameDataEntry> DataEntries;
	TMap<FGameplayTag, FSimpleMulticastDelegate> DataEntryDelegates;
	TMap<FGameplayTag, TArray<FMDOnGameDataEntryChanged>> DataEntryDynamicDelegates;
	TSet<FGameplayTag> EntriesWithUObjects;
};

template <typename T>
FORCEINLINE EMDGameDataContainerResult UMDGameDataContainer::SetData(const FGameplayTag& DataKey, const T& Value)
{
	const FProperty* Prop = MDGameDataUtils::GetPropertyForType<T>();
	if (!DataKey.IsValid() || Prop == nullptr)
	{
		return EMDGameDataContainerResult::Failure_InvalidInputs;
	}

	const EMDGameDataContainerResult Result = SetDataFromProperty(DataKey, Prop, &Value);
	ensureAlwaysMsgf(Result != EMDGameDataContainerResult::Failure_TypeMismatch, TEXT("Type [%s] does not match existing type [%s] with key [%s]"),
		*MDGameDataUtils::GetPropertyTypeAsString(Prop),
		*GetEntryTypeString(DataKey),
		*DataKey.ToString()
	);

	return Result;
}

template <typename T>
FORCEINLINE EMDGameDataContainerResult UMDGameDataContainer::GetData(const FGameplayTag& DataKey, T& OutResult) const
{
	const TTuple<const FProperty*, const void*> Entry = GetData(DataKey);
	if (Entry.Key == nullptr || Entry.Value == nullptr)
	{
		return EMDGameDataContainerResult::Failure_NoEntry;
	}

	const FProperty* Prop = MDGameDataUtils::GetPropertyForType<T>();
	if (Prop == nullptr)
	{
		return EMDGameDataContainerResult::Failure_InvalidInputs;
	}

	const EMDGameDataContainerResult Result = GetDataFromProperty(DataKey, Prop, &OutResult);
	ensureAlwaysMsgf(Result != EMDGameDataContainerResult::Failure_TypeMismatch, TEXT("Type [%s] does not match existing type [%s] with key [%s]"),
		*MDGameDataUtils::GetPropertyTypeAsString(Prop),
		*GetEntryTypeString(DataKey),
		*DataKey.ToString()
	);

	return Result;
}

template <typename T>
FDelegateHandle UMDGameDataContainer::BindOnEntryChangedDelegate(const FGameplayTag& DataKey, TDelegate<void(T)>&& Delegate)
{
	check(Delegate.IsBound());

	FSimpleDelegate InternalDelegate = FSimpleDelegate::CreateUObject(this, &UMDGameDataContainer::NotifyNewEntryValue<T>, DataKey, MoveTemp(Delegate));
	return BindOnEntryChangedDelegate(DataKey, MoveTemp(InternalDelegate));
}

template <typename T>
void UMDGameDataContainer::NotifyNewEntryValue(FGameplayTag DataKey, TDelegate<void(T)> Delegate)
{
	using ValueType = typename TRemoveCV<typename TRemoveReference<T>::Type>::Type;

	ValueType Value;
	GetData(DataKey, Value);

	Delegate.ExecuteIfBound(Value);
}
