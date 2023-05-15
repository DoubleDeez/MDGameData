#include "MDGameDataContainer.h"
#include "Launch/Resources/Version.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "MDGameDataContainer"

namespace MDGameDataContainer_Private
{
	bool DoesPropertyReferenceUObjects(const FProperty* Property, TSet<const FProperty*>& SeenList)
	{
		if (SeenList.Contains(Property))
		{
			return false;
		}
		SeenList.Add(Property);

		if (Property->IsA<FObjectProperty>())
		{
			return true;
		}

		if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			for (TFieldIterator<const FProperty> It(StructProperty->Struct); It; ++It)
			{
				if (DoesPropertyReferenceUObjects(*It, SeenList))
				{
					return true;
				}
			}

			return false;
		}

		if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			return DoesPropertyReferenceUObjects(ArrayProperty->Inner, SeenList);
		}

		if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
		{
			return DoesPropertyReferenceUObjects(SetProperty->ElementProp, SeenList);
		}

		if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
		{
			if (DoesPropertyReferenceUObjects(MapProperty->KeyProp, SeenList))
			{
				return true;
			}

			return DoesPropertyReferenceUObjects(MapProperty->ValueProp, SeenList);
		}

		return false;
	}

	void AddPropertyReferencedUObjects(FReferenceCollector& Collector, const FProperty* Property, const void* ValuePtr, TSet<TPair<const FProperty*, const void*>>& SeenList)
	{
		TPair<const FProperty*, const void*> Pair { Property, ValuePtr };
		if (SeenList.Contains(Pair))
		{
			return;
		}

		SeenList.Add(MoveTemp(Pair));

		if (Property->IsA<FObjectProperty>())
		{
			UObject** ObjectPtr = static_cast<UObject**>(const_cast<void*>(ValuePtr));
			if (ObjectPtr != nullptr && *ObjectPtr != nullptr)
			{
				Collector.AddReferencedObject(*ObjectPtr);
			}
		}
		else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2)
			Collector.AddPropertyReferences(StructProperty->Struct, const_cast<void*>(ValuePtr));
#else
			for (TFieldIterator<const FProperty> It(StructProperty->Struct); It; ++It)
			{
				AddPropertyReferencedUObjects(Collector, *It, It->ContainerPtrToValuePtr<void>(ValuePtr), SeenList);
			}
#endif
		}
		else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			TSet<const FProperty*> PropSeenList;
			if (DoesPropertyReferenceUObjects(ArrayProperty->Inner, PropSeenList))
			{
				FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
				for (int32 i = 0; i < Helper.Num(); ++i)
				{
					const void* ElementPtr = Helper.GetRawPtr(i);
					AddPropertyReferencedUObjects(Collector, ArrayProperty->Inner, ElementPtr, SeenList);
				}
			}
		}
		else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
		{
			TSet<const FProperty*> PropSeenList;
			if (DoesPropertyReferenceUObjects(SetProperty->ElementProp, PropSeenList))
			{
				FScriptSetHelper Helper(SetProperty, ValuePtr);
				for (int32 i = 0; i < Helper.Num(); ++i)
				{
					const void* ElementPtr = Helper.GetElementPtr(i);
					AddPropertyReferencedUObjects(Collector, SetProperty->ElementProp, ElementPtr, SeenList);
				}
			}
		}
		else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
		{
			TSet<const FProperty*> PropSeenList;
			const bool bDoesKeyReferenceUObjects = DoesPropertyReferenceUObjects(MapProperty->KeyProp, PropSeenList);
			PropSeenList.Reset();
			const bool bDoesValueReferenceUObjects = DoesPropertyReferenceUObjects(MapProperty->ValueProp, PropSeenList);
			if (bDoesKeyReferenceUObjects || bDoesValueReferenceUObjects)
			{
				FScriptMapHelper Helper(MapProperty, ValuePtr);
				for (int32 i = 0; i < Helper.Num(); ++i)
				{
					if (bDoesKeyReferenceUObjects)
					{
						const void* KeyPtr = Helper.GetKeyPtr(i);
						AddPropertyReferencedUObjects(Collector, SetProperty->ElementProp, KeyPtr, SeenList);
					}

					if (bDoesValueReferenceUObjects)
					{
						const void* MapValuePtr = Helper.GetValuePtr(i);
						AddPropertyReferencedUObjects(Collector, SetProperty->ElementProp, MapValuePtr, SeenList);
					}
				}
			}
		}
	}
}

void UMDGameDataContainer::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	if (UMDGameDataContainer* This = Cast<UMDGameDataContainer>(InThis))
	{
		TSet<TPair<const FProperty*, const void*>> SeenList;
		for (const FGameplayTag& EntryKey : This->EntriesWithUObjects)
		{
			SeenList.Reset();
			MDGameDataContainer_Private::AddPropertyReferencedUObjects(Collector, This->DataEntries[EntryKey].EntryProperty, This->DataEntries[EntryKey].EntryValuePtr, SeenList);
		}
	}

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
		BroadcastEntryChanged(DataKey);
		return EMDGameDataContainerResult::Success_ExistingEntry;
	}
	else
	{
		const FMDGameDataEntry& NewEntry = AddEntry(DataKey, FMDGameDataEntry(Allocator, DataKey.GetTagName(), Prop));
		NewEntry.EntryProperty->CopyCompleteValue(NewEntry.EntryValuePtr, ValuePtr);
		BroadcastEntryChanged(DataKey);
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

void UMDGameDataContainer::BindOnEntryChanged(const FGameplayTag& DataKey, FMDOnGameDataEntryChanged Delegate)
{
	DataEntryDynamicDelegates.FindOrAdd(DataKey).AddUnique(Delegate);
}

void UMDGameDataContainer::UnbindOnEntryChanged(const FGameplayTag& DataKey, FMDOnGameDataEntryChanged Delegate)
{
	if (TArray<FMDOnGameDataEntryChanged>* Delegates = DataEntryDynamicDelegates.Find(DataKey))
	{
		Delegates->Remove(Delegate);
	}
}

FDelegateHandle UMDGameDataContainer::BindOnEntryChangedDelegate(const FGameplayTag& DataKey, FSimpleDelegate&& Delegate)
{
	return DataEntryDelegates.FindOrAdd(DataKey).Add(MoveTemp(Delegate));
}

void UMDGameDataContainer::UnbindOnEntryChangedDelegate(const FGameplayTag& DataKey, const FDelegateHandle& DelegateHandle)
{
	if (FSimpleMulticastDelegate* Delegates = DataEntryDelegates.Find(DataKey))
	{
		Delegates->Remove(DelegateHandle);
	}
}

FString UMDGameDataContainer::GetEntryTypeString(const FGameplayTag& DataKey) const
{
	if (const FMDGameDataEntry* Entry = DataEntries.Find(DataKey))
	{
		return MDGameDataUtils::GetPropertyTypeAsString(Entry->EntryProperty);
	}

	return TEXT("None");
}

const FMDGameDataEntry& UMDGameDataContainer::AddEntry(const FGameplayTag& DataKey, FMDGameDataEntry&& Entry)
{
	const FMDGameDataEntry& NewEntry = DataEntries.Emplace(DataKey, MoveTemp(Entry));

	TSet<const FProperty*> SeenList;
	if (MDGameDataContainer_Private::DoesPropertyReferenceUObjects(NewEntry.EntryProperty, SeenList))
	{
		// TODO - Do anything we can here to make AddReferencedObjects quicker
		EntriesWithUObjects.Add(DataKey);
	}

	return NewEntry;
}

void UMDGameDataContainer::BroadcastEntryChanged(const FGameplayTag& DataKey) const
{
	if (const FSimpleMulticastDelegate* Delegates = DataEntryDelegates.Find(DataKey))
	{
		Delegates->Broadcast();
	}

	if (const TArray<FMDOnGameDataEntryChanged>* Delegates = DataEntryDynamicDelegates.Find(DataKey))
	{
		for (const FMDOnGameDataEntryChanged& Delegate : *Delegates)
		{
			Delegate.ExecuteIfBound();
		}
	}
}

#undef LOCTEXT_NAMESPACE
