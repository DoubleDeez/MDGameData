#pragma once


class FMDGameDataAllocator;
class FName;
class FProperty;

struct FMDGameDataEntry
{
	FMDGameDataEntry(FMDGameDataAllocator& Allocator, const FName& Name, const FProperty* SourceProp);

	FMDGameDataEntry(const FMDGameDataEntry& Other) = delete;
	FMDGameDataEntry& operator=(const FMDGameDataEntry& Other) = delete;

	FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept;
	FMDGameDataEntry& operator=(FMDGameDataEntry&& Other) noexcept;

	~FMDGameDataEntry();

	const FProperty* EntryProperty = nullptr;
	void* EntryValuePtr = nullptr;
};

