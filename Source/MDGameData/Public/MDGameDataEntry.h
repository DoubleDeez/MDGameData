#pragma once

#include "CoreMinimal.h"

struct FMDGameDataEntry
{
	FMDGameDataEntry(const FName& Name, const FProperty* SourceProp);

	template<typename T>
	FMDGameDataEntry(const FName& Name, const T& Value);
	
	FMDGameDataEntry(const FMDGameDataEntry& Other) = delete;
	FMDGameDataEntry& operator=(const FMDGameDataEntry& Other) = delete;
	
	FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept;
	FMDGameDataEntry& operator=(FMDGameDataEntry&& Other) noexcept;
	
	~FMDGameDataEntry();

	const FProperty* EntryProperty = nullptr;

	void* EntryValuePtr = nullptr;

private:
	void InitializeEntry();
};

template <typename T>
FMDGameDataEntry::FMDGameDataEntry(const FName& Name, const T& Value)
{
	// TODO - Construct EntryProperty from T
	InitializeEntry();
}

