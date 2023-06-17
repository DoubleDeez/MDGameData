#pragma once

#include "MDGameDataBlueprintFunctionLibrary.h"
#include "MDGameDataContainer.h"

struct FGameplayTag;

/**
 * Exposed helper functions for using Game Data Containers
 */
namespace MDGameDataHelpers
{
	MDGAMEDATA_API FDelegateHandle BindOnActorEntryChangedDelegate(AActor* Actor, const FGameplayTag& DataKey, FSimpleDelegate&& Delegate);
	MDGAMEDATA_API void UnbindOnActorEntryChangedDelegate(const AActor* Actor, const FGameplayTag& DataKey, const FDelegateHandle& DelegateHandle);

	template<typename T>
	FDelegateHandle BindOnActorEntryChangedDelegate(AActor* Actor, const FGameplayTag& DataKey, TDelegate<void(T)>&& Delegate)
	{
		UMDGameDataContainer* Container = UMDGameDataBlueprintFunctionLibrary::FindOrCreateGameDataContainerForActor(Actor);
		if (IsValid(Container))
		{
			return Container->BindOnEntryChangedDelegate(DataKey, MoveTemp(Delegate));
		}

		return {};
	}

	template<typename T>
	static bool GetGameDataForActor(const AActor* Actor, const FGameplayTag& DataKey, T& OutValue)
	{
		UMDGameDataContainer* Container = UMDGameDataBlueprintFunctionLibrary::FindGameDataContainerForActor(Actor);
		if (IsValid(Container))
		{
			const EMDGameDataContainerResult Result = Container->GetData(DataKey, OutValue);
			return Result == EMDGameDataContainerResult::Success_ExistingEntry;
		}

		return false;
	}

	template<typename T>
	static bool SetGameDataForActor(AActor* Actor, const FGameplayTag& DataKey, const T& Value)
	{
		UMDGameDataContainer* Container = UMDGameDataBlueprintFunctionLibrary::FindOrCreateGameDataContainerForActor(Actor);
		if (IsValid(Container))
		{
			const EMDGameDataContainerResult Result = Container->SetData(DataKey, Value);
			return Result == EMDGameDataContainerResult::Success_ExistingEntry
				|| Result == EMDGameDataContainerResult::Success_NewEntry;
		}

		return false;
	}
};
