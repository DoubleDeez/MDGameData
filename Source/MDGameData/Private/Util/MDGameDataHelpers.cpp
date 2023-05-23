#include "Util/MDGameDataHelpers.h"

FDelegateHandle MDGameDataHelpers::BindOnActorEntryChangedDelegate(AActor* Actor, const FGameplayTag& DataKey, FSimpleDelegate&& Delegate)
{
	UMDGameDataContainer* Container = UMDGameDataBlueprintFunctionLibrary::FindOrCreateGameDataContainerForActor(Actor);
	if (IsValid(Container))
	{
		return Container->BindOnEntryChangedDelegate(DataKey, MoveTemp(Delegate));
	}

	return {};
}

void MDGameDataHelpers::UnbindOnActorEntryChangedDelegate(const AActor* Actor, const FGameplayTag& DataKey, const FDelegateHandle& DelegateHandle)
{
	UMDGameDataContainer* Container = UMDGameDataBlueprintFunctionLibrary::FindGameDataContainerForActor(Actor);
	if (IsValid(Container))
	{
		return Container->UnbindOnEntryChangedDelegate(DataKey, DelegateHandle);
	}
}
