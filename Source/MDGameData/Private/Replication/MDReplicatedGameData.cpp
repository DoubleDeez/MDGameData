#include "Replication/MDReplicatedGameData.h"

#include "MDGameDataEntry.h"
#include "MDGameDataContainer.h"
#include "MDGDPropertyConstructors.h"
#include "Components/MDGameDataComponent.h"
#include "Util/MDGameDataUtils.h"


FMDReplicatedGameDataEntry::FMDReplicatedGameDataEntry(const FMDReplicatedGameDataEntry& Other)
{
	*this = Other;
}

FMDReplicatedGameDataEntry& FMDReplicatedGameDataEntry::operator=(const FMDReplicatedGameDataEntry& Other)
{
	CleanUpEntry();
	
	EntryKey = Other.EntryKey;
	
	if (Other.EntryProperty != nullptr)
	{
		EntryProperty = CastFieldChecked<FProperty>(FField::Duplicate(Other.EntryProperty, GetTransientPackage()));
		EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
		EntryProperty->InitializeValue(EntryValuePtr);
		EntryProperty->CopyCompleteValue(EntryValuePtr, Other.EntryValuePtr);
	}

	return *this;
}

FMDReplicatedGameDataEntry::FMDReplicatedGameDataEntry(FMDReplicatedGameDataEntry&& Other) noexcept
{
	*this = MoveTemp(Other);
}

FMDReplicatedGameDataEntry& FMDReplicatedGameDataEntry::operator=(FMDReplicatedGameDataEntry&& Other) noexcept
{
	EntryKey = MoveTemp(Other.EntryKey);
	
	EntryProperty = Other.EntryProperty;
	Other.EntryProperty = nullptr;
	
	EntryValuePtr = Other.EntryValuePtr;
	Other.EntryValuePtr = nullptr;

	return *this;
}

FMDReplicatedGameDataEntry::~FMDReplicatedGameDataEntry()
{
	CleanUpEntry();
}

void FMDReplicatedGameDataEntry::PopulateFromEntry(const FMDGameDataEntry& Entry)
{
	if (EntryProperty == nullptr)
	{
		check(EntryValuePtr == nullptr);
		
		EntryProperty = CastFieldChecked<FProperty>(FField::Duplicate(Entry.EntryProperty, GetTransientPackage()));
		EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
		EntryProperty->InitializeValue(EntryValuePtr);
	}

	check(EntryProperty->SameType(Entry.EntryProperty));

	EntryProperty->CopyCompleteValue(EntryValuePtr, Entry.EntryValuePtr);
}

bool FMDReplicatedGameDataEntry::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << EntryKey;

	FName PropertyName = EntryProperty != nullptr ? EntryProperty->GetFName() : NAME_None;
	Ar << PropertyName;

	uint64 CastFlags = EntryProperty != nullptr ? EntryProperty->GetClass()->GetCastFlags() : 0;
	Ar << CastFlags;

	if (Ar.IsLoading())
	{
		if (EntryProperty == nullptr)
		{
			EntryProperty = MDGDPropertyConstructors::CreatePropertyFromCastFlags(PropertyName, static_cast<EClassCastFlags>(CastFlags));
			if (EntryProperty == nullptr)
			{
				checkNoEntry();
				Ar.SetError();
				return false;
			}
		}
		else
		{
			check(EntryProperty->GetClass()->GetCastFlags() == CastFlags);
		}
	}

	EntryProperty->Serialize(Ar);

	// FProperty::Serialize skips computed flags but we don't compute them so we need them serialized
	EPropertyFlags ComputedFlags = EntryProperty->GetPropertyFlags() & CPF_ComputedFlags;
	Ar << (uint64&)ComputedFlags;

	if (Ar.IsLoading())
	{
		EntryProperty->SetPropertyFlags(ComputedFlags);

		if (EntryValuePtr == nullptr)
		{
			EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
			EntryProperty->InitializeValue(EntryValuePtr);
		}
	}

	return NetSerializeProperty(EntryProperty, EntryValuePtr, Ar, Map, bOutSuccess);
}

bool FMDReplicatedGameDataEntry::NetSerializeProperty(const FProperty* Property, void* ValuePtr, FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		if (StructProperty->Struct->StructFlags & STRUCT_NetSerializeNative)
		{
			bOutSuccess = StructProperty->NetSerializeItem(Ar, Map, ValuePtr);
		}
		else
		{
			// Structs must be manually replicated
			for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
			{
				if (It->PropertyFlags & CPF_RepSkip)
				{
					continue;
				}

				void* PropertyData = It->ContainerPtrToValuePtr<void*>(ValuePtr);
				bOutSuccess = NetSerializeProperty(*It, PropertyData, Ar, Map, bOutSuccess);
			}
		}
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper Helper = FScriptArrayHelper(ArrayProperty, ValuePtr);

		uint16 OutArrayNum = Helper.Num();
		Ar << OutArrayNum;
		
		const int32 ArrayNum = Ar.IsLoading() ? (int32)OutArrayNum : Helper.Num();

		if (!Ar.IsError())
		{
			// When loading, we may need to resize the array to properly fit the number of elements.
			if (Ar.IsLoading() && OutArrayNum != Helper.Num())
			{
				Helper.Resize(OutArrayNum);
			}

			for (int32 i = 0; i < ArrayNum && !Ar.IsError(); i++)
			{
				bOutSuccess |= NetSerializeProperty(ArrayProperty->Inner, Helper.GetRawPtr(i), Ar, Map, bOutSuccess);
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		FScriptSetHelper Helper = FScriptSetHelper(SetProperty, ValuePtr);

		uint16 OutSetNum = Helper.Num();
		Ar << OutSetNum;
		
		const int32 SetNum = Ar.IsLoading() ? (int32)OutSetNum : Helper.Num();

		if (!Ar.IsError())
		{
			// When loading, we may need to resize the set to properly fit the number of elements.
			if (Ar.IsLoading() && OutSetNum != Helper.Num())
			{
				if (OutSetNum > Helper.Num())
				{
					Helper.RemoveAt(OutSetNum, OutSetNum - Helper.Num());
				}
				else
				{
					while (Helper.Num() < OutSetNum)
					{
						Helper.AddDefaultValue_Invalid_NeedsRehash();
					}
				}
			}

			for (int32 i = 0; i < SetNum && !Ar.IsError(); i++)
			{
				bOutSuccess |= NetSerializeProperty(SetProperty->ElementProp, Helper.GetElementPtr(i), Ar, Map, bOutSuccess);
			}

			Helper.Rehash();
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		FScriptMapHelper Helper = FScriptMapHelper(MapProperty, ValuePtr);

		uint16 OutMapNum = Helper.Num();
		Ar << OutMapNum;
		
		const int32 MapNum = Ar.IsLoading() ? (int32)OutMapNum : Helper.Num();

		if (!Ar.IsError())
		{
			// When loading, we may need to resize the set to properly fit the number of elements.
			if (Ar.IsLoading() && OutMapNum != Helper.Num())
			{
				if (OutMapNum > Helper.Num())
				{
					Helper.RemoveAt(OutMapNum, OutMapNum - Helper.Num());
				}
				else
				{
					while (Helper.Num() < OutMapNum)
					{
						Helper.AddDefaultValue_Invalid_NeedsRehash();
					}
				}
			}

			for (int32 i = 0; i < MapNum && !Ar.IsError(); i++)
			{
				bOutSuccess |= NetSerializeProperty(MapProperty->KeyProp, Helper.GetKeyPtr(i), Ar, Map, bOutSuccess);
				bOutSuccess |= NetSerializeProperty(MapProperty->ValueProp, Helper.GetValuePtr(i), Ar, Map, bOutSuccess);
			}

			Helper.Rehash();
		}
	}
	else
	{
		bOutSuccess = Property->NetSerializeItem(Ar, Map, ValuePtr);
	}

	return bOutSuccess;
}

void FMDReplicatedGameDataEntry::CleanUpEntry()
{
	if (EntryProperty != nullptr)
	{
		if (EntryValuePtr != nullptr)
		{
			EntryProperty->DestroyValue(EntryValuePtr);
		}
		
		delete EntryProperty;
		EntryProperty = nullptr;
	}

	if (EntryValuePtr != nullptr)
	{
		FMemory::Free(EntryValuePtr);
		EntryValuePtr = nullptr;
	}
}

void FMDReplicatedGameDataEntry::PreReplicatedRemove(const FMDReplicatedGameData& Serializer)
{
	UMDGameDataContainer* GameDataContainer = Serializer.GetGameDataContainer();
	if (ensure(IsValid(GameDataContainer)))
	{
		GameDataContainer->RemoveEntry(EntryKey);
	}
}

void FMDReplicatedGameDataEntry::PostReplicatedAdd(const FMDReplicatedGameData& Serializer)
{
	UMDGameDataContainer* GameDataContainer = Serializer.GetGameDataContainer();
	if (ensure(IsValid(GameDataContainer)))
	{
		GameDataContainer->SetDataFromProperty(EntryKey, EntryProperty, EntryValuePtr);
	}
}

void FMDReplicatedGameDataEntry::PostReplicatedChange(const FMDReplicatedGameData& Serializer)
{
	UMDGameDataContainer* GameDataContainer = Serializer.GetGameDataContainer();
	if (ensure(IsValid(GameDataContainer)))
	{
		GameDataContainer->SetDataFromProperty(EntryKey, EntryProperty, EntryValuePtr);
	}
}

FString FMDReplicatedGameDataEntry::GetDebugString() const
{
	return FString::Printf(TEXT("Entry [%s], Type [%s], Value [%s]"),
		*EntryKey.ToString(),
		*MDGameDataUtils::GetPropertyTypeAsString(EntryProperty),
		*MDGameDataUtils::GetPropertyValueAsString(EntryProperty, EntryValuePtr)
	);
}

bool FMDReplicatedGameData::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FastArrayDeltaSerialize<FMDReplicatedGameDataEntry, FMDReplicatedGameData>(GameDataEntries, DeltaParams, *this);
}

#if WITH_SERVER_CODE
void FMDReplicatedGameData::AddOrUpdateEntry(const FGameplayTag& EntryKey, const FMDGameDataEntry& RawEntry)
{
	check(EntryKey.IsValid());
	
	FMDReplicatedGameDataEntry* EntryPtr = GameDataEntries.FindByPredicate([EntryKey](const FMDReplicatedGameDataEntry& Entry)
	{
		return Entry.EntryKey == EntryKey;
	});

	if (EntryPtr == nullptr)
	{
		FMDReplicatedGameDataEntry NewEntry;
		NewEntry.EntryKey = EntryKey;
		NewEntry.PopulateFromEntry(RawEntry);

		MarkItemDirty(GameDataEntries.Add_GetRef(MoveTemp(NewEntry)));
	}
	else
	{
		EntryPtr->PopulateFromEntry(RawEntry);
		MarkItemDirty(*EntryPtr);
	}
}

void FMDReplicatedGameData::RemoveEntry(const FGameplayTag& EntryKey)
{
	const int32 Index = GameDataEntries.IndexOfByPredicate([EntryKey](const FMDReplicatedGameDataEntry& Entry)
	{
		return Entry.EntryKey == EntryKey;
	});

	if (Index != INDEX_NONE)
	{
		GameDataEntries.RemoveAt(Index);
		MarkArrayDirty();
	}
}
#endif

UMDGameDataContainer* FMDReplicatedGameData::GetGameDataContainer() const
{
	if (const UMDGameDataComponent* Component = GameDataComponent.Get())
	{
		return Component->GetGameDataContainer();
	}

	return nullptr;
}