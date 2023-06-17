#include "Util/MDGameDataAllocator.h"
#include "Misc/ScopeLock.h"

FMDGameDataAllocator::~FMDGameDataAllocator()
{
	FScopeLock _(&Cs);
	for (void* Slab : Slabs)
	{
		FMemory::Free(Slab);
	}
}

void* FMDGameDataAllocator::Allocate(uint64 Size)
{
	FScopeLock _(&Cs);
	const uint64 AllocationSize = Size + (-int64(Size) & 15);
	if (!CurrentSlab || CurrentSlabAllocatedSize + AllocationSize > SlabSize)
	{
		TotalAllocatedSize += SlabSize;
		void* Allocation = FMemory::Malloc(SlabSize, 16);
		CurrentSlab = reinterpret_cast<uint8*>(Allocation);
		CurrentSlabAllocatedSize = 0;
		Slabs.Add(CurrentSlab);
	}

	void* Allocation = CurrentSlab + CurrentSlabAllocatedSize;
	CurrentSlabAllocatedSize += AllocationSize;
	return Allocation;
}
