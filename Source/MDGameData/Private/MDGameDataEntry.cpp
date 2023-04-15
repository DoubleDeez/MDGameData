#include "MDGameDataEntry.h"

#include "UObject/UnrealType.h"

FMDGameDataEntry::FMDGameDataEntry(const FName& Name, const FProperty* SourceProp)
{
	check(SourceProp != nullptr);
	
	EntryProperty = static_cast<FProperty*>(FField::Duplicate(SourceProp, {}, Name, RF_Transient, EInternalObjectFlags::None));
	InitializeEntry();
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

void FMDGameDataEntry::InitializeEntry()
{
	check(EntryProperty);
	
	// TODO - Consider using pre-allocated blocks of memory for the data entries
	EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
	EntryProperty->InitializeValue(EntryValuePtr);
}

FMDGameDataEntry& FMDGameDataEntry::operator=(FMDGameDataEntry&& Other) noexcept
{
	EntryProperty = Other.EntryProperty;
	Other.EntryProperty = nullptr;
	
	EntryValuePtr = Other.EntryValuePtr;
	Other.EntryValuePtr = nullptr;
	
	return *this;
}

