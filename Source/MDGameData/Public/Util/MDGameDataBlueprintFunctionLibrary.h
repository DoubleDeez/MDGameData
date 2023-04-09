#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MDGameDataBlueprintFunctionLibrary.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class MDGAMEDATA_API UMDGameDataBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category="Game Data", meta = (WorldContext="WorldContextObject", CustomStructureParam = "OutValue", AutoCreateRefTerm = "DataKey"))
	static UPARAM(DisplayName="Success") bool GetGameDataValue(UObject* WorldContextObject, const FGameplayTag& DataKey, UPARAM(DisplayName="Value") int32& OutValue);
	DECLARE_FUNCTION(execGetGameDataValue);

	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category="Game Data", meta = (WorldContext="WorldContextObject", CustomStructureParam = "Value", AutoCreateRefTerm = "DataKey"))
	static UPARAM(DisplayName="Success") bool SetGameDataValue(UObject* WorldContextObject, const FGameplayTag& DataKey, const int32& Value);
	DECLARE_FUNCTION(execSetGameDataValue);

	static bool GetDataFromPropertyWithExceptions(UObject* WorldContextObject, const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr
		, const UObject* ContextObject, FFrame& Stack);

	static bool SetDataFromPropertyWithExceptions(UObject* WorldContextObject, const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr
		, const UObject* ContextObject, FFrame& Stack);
};
