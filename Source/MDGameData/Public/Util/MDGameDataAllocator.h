#pragma once

#include "Containers/Array.h"
#include "HAL/CriticalSection.h"
#include "HAL/Platform.h"


// Ripped off version of FSlabAllocator
class FMDGameDataAllocator
{
public:
	~FMDGameDataAllocator();

	void* Allocate(uint64 Size);

private:
	FCriticalSection Cs;
	TArray<void*> Slabs;
	uint8* CurrentSlab = nullptr;
	const uint64 SlabSize = (16 << 10);
	uint64 CurrentSlabAllocatedSize = 0;
	uint64 TotalAllocatedSize = 0;
};
