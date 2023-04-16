#include "MDGameDataEntry.h"

#include "UObject/UnrealType.h"
#include "Util/MDGameDataAllocator.h"

FMDGameDataEntry::FMDGameDataEntry(FMDGameDataAllocator& Allocator, const FName& Name, const FProperty* SourceProp)
{
	check(SourceProp != nullptr);
	
	EntryProperty = static_cast<FProperty*>(FField::Duplicate(SourceProp, {}, Name, RF_Transient, EInternalObjectFlags::None));
	InitializeEntry(Allocator);
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
}

void FMDGameDataEntry::InitializeEntry(FMDGameDataAllocator& Allocator)
{
	check(EntryProperty);
	
	EntryValuePtr = Allocator.Allocate(EntryProperty->GetSize());
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

