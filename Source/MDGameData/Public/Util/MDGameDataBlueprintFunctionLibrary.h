#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MDGameDataBlueprintFunctionLibrary.generated.h"

enum class EMDGameDataContainerSource : uint8;
class UMDGameDataContainer;
struct FGameplayTag;

/**
 *
 */
UCLASS()
class MDGAMEDATA_API UMDGameDataBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category="Game Data", meta = (CustomStructureParam = "OutValue", AutoCreateRefTerm = "DataKey"))
	static UPARAM(DisplayName="Success") bool GetGameDataValue(UMDGameDataContainer* GameDataContainer, const FGameplayTag& DataKey, UPARAM(DisplayName="Value") int32& OutValue);
	DECLARE_FUNCTION(execGetGameDataValue);

	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category="Game Data", meta = (CustomStructureParam = "Value", AutoCreateRefTerm = "DataKey"))
	static UPARAM(DisplayName="Success") bool SetGameDataValue(UMDGameDataContainer* GameDataContainer, const FGameplayTag& DataKey, const int32& Value);
	DECLARE_FUNCTION(execSetGameDataValue);

	UFUNCTION(BlueprintCallable, Category="Game Data")
	static UMDGameDataContainer* GetGameDataContainerForActor(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="Game Data", meta = (DefaultToSelf = "Context"))
	static UMDGameDataContainer* ResolveGameDataSource(EMDGameDataContainerSource Source, UObject* Context);
};
