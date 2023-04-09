#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MDGameDataSubsystem.generated.h"

struct FMDGameDataEntry
{
	//FMDGameDataEntry() = default;

	explicit FMDGameDataEntry(const FProperty* SourceProp);
	
	FMDGameDataEntry(const FMDGameDataEntry& Other) = delete;
	FMDGameDataEntry& operator=(const FMDGameDataEntry& Other) = delete;
	
	FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept;
	FMDGameDataEntry& operator=(FMDGameDataEntry&& Other) noexcept;
	
	~FMDGameDataEntry();

	const FProperty* EntryProperty = nullptr;

	void* EntryValuePtr = nullptr;
};

/**
 * 
 */
UCLASS()
class MDGAMEDATA_API UMDGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UMDGameDataSubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	TTuple<const FProperty*, const void*> GetData(const FGameplayTag& DataKey) const;

	bool SetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr);
	bool GetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr) const;
	
	bool SetDataFromPropertyWithExceptions(const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr, const UObject* ContextObject, FFrame& Stack);
	bool GetDataFromPropertyWithExceptions(const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr, const UObject* ContextObject, FFrame& Stack) const;

protected:
	// TODO - Track UObject references
	TMap<FGameplayTag, FMDGameDataEntry> DataEntries;
};
