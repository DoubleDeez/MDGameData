#include "MDGameDataContainer.h"

#include "GameplayTagContainer.h"

#define LOCTEXT_NAMESPACE "MDGameDataContainer"

FMDGameDataEntry::FMDGameDataEntry(const FProperty* SourceProp)
{
	check(SourceProp != nullptr);
	
	EntryProperty = static_cast<FProperty*>(FField::Duplicate(SourceProp, {}, NAME_None, RF_Transient, EInternalObjectFlags::None));
	// TODO - Consider using pre-allocated blocks of memory for the data entries
	EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
	EntryProperty->InitializeValue(EntryValuePtr);
}

FMDGameDataEntry::FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept
{
	*this = MoveTemp(Other);
}

FMDGameDataEntry::~FMDGameDataEntry()
{
	if (EntryProperty != nullptr)
	{
		delete EntryProperty;
		EntryProperty = nullptr;
	}

	if (EntryValuePtr != nullptr)
	{
		FMemory::Free(EntryValuePtr);
		EntryValuePtr = nullptr;
	}
}

FMDGameDataEntry& FMDGameDataEntry::operator=(FMDGameDataEntry&& Other) noexcept
{
	EntryProperty = Other.EntryProperty;
	Other.EntryProperty = nullptr;
	
	EntryValuePtr = Other.EntryValuePtr;
	Other.EntryValuePtr = nullptr;
	
	return *this;
}

void UMDGameDataContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// TODO - Add any UObjects held in our map
	
	UObject::AddReferencedObjects(InThis, Collector);
}

void UMDGameDataContainer::InitializeGameDataContainer(const FMDGameDataContainerInitData& InitData)
{
	DataEntries.Reserve(InitData.InitialMapReserve);
}

void UMDGameDataContainer::ShutdownGameDataContainer()
{
	DataEntries.Reset();
}

TTuple<const FProperty*, const void*> UMDGameDataContainer::GetData(const FGameplayTag& DataKey) const
{
	const FMDGameDataEntry* Entry = DataEntries.Find(DataKey);
	if (Entry == nullptr)
	{
		return {};
	}

	return { Entry->EntryProperty, Entry->EntryValuePtr };
}

EMDGameDataContainerResult UMDGameDataContainer::SetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr)
{
	if (Prop == nullptr || ValuePtr == nullptr || !DataKey.IsValid())
	{
		return EMDGameDataContainerResult::Failure_InvalidInputs;
	}
	
	if (const FMDGameDataEntry* Entry = DataEntries.Find(DataKey))
	{
		if (!Entry->EntryProperty->SameType(Prop))
		{
			return EMDGameDataContainerResult::Failure_TypeMismatch;
		}

		Entry->EntryProperty->CopyCompleteValue(Entry->EntryValuePtr, ValuePtr);
		return EMDGameDataContainerResult::Success_ExistingEntry;
	}
	else
	{
		const FMDGameDataEntry& NewEntry = DataEntries.Emplace(DataKey, FMDGameDataEntry(Prop));
		NewEntry.EntryProperty->CopyCompleteValue(NewEntry.EntryValuePtr, ValuePtr);
		return EMDGameDataContainerResult::Success_NewEntry;
	}
}

EMDGameDataContainerResult UMDGameDataContainer::GetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr) const
{
	if (Prop == nullptr || ValuePtr == nullptr)
	{
		return EMDGameDataContainerResult::Failure_InvalidInputs;
	}
	
	const TTuple<const FProperty*, const void*> Entry = GetData(DataKey);
	if (Entry.Key == nullptr || Entry.Value == nullptr)
	{
		return EMDGameDataContainerResult::Failure_NoEntry;
	}

	if (!Entry.Key->SameType(Prop))
	{
		return EMDGameDataContainerResult::Failure_TypeMismatch;
	}

	Prop->CopyCompleteValue(ValuePtr, Entry.Value);
	return EMDGameDataContainerResult::Success_ExistingEntry;
}

#undef LOCTEXT_NAMESPACE
