#include "MDGameDataEntry.h"

#include "UObject/UnrealType.h"
#include "Util/MDGameDataAllocator.h"

FMDGameDataEntry::FMDGameDataEntry(FMDGameDataAllocator& Allocator, const FName& Name, const FProperty* SourceProp)
{
	check(SourceProp != nullptr);
	
	EntryProperty = CastFieldChecked<FProperty>(FField::Duplicate(SourceProp, {}, Name, RF_Transient, EInternalObjectFlags::None));
	EntryValuePtr = Allocator.Allocate(EntryProperty->GetSize());
	EntryProperty->InitializeValue(EntryValuePtr);
}

FMDGameDataEntry::FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept
{
	*this = MoveTemp(Other);
}

FMDGameDataEntry& FMDGameDataEntry::operator=(FMDGameDataEntry&& Other) noexcept
{
	EntryProperty = Other.EntryProperty;
	Other.EntryProperty = nullptr;
	
	EntryValuePtr = Other.EntryValuePtr;
	Other.EntryValuePtr = nullptr;
	
	return *this;
}

FMDGameDataEntry::~FMDGameDataEntry()
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
}

