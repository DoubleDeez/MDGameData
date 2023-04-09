#include "Util/MDGameDataBlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "Subsystems/MDGameDataSubsystem.h"

#define LOCTEXT_NAMESPACE "MDGameDataBlueprintFunctionLibrary"

bool UMDGameDataBlueprintFunctionLibrary::GetGameDataValue(UObject* WorldContextObject, const FGameplayTag& DataKey, int32& OutValue)
{
	checkf(false, TEXT("Calling a CustomThunk!"));
	return false;
}

bool UMDGameDataBlueprintFunctionLibrary::SetGameDataValue(UObject* WorldContextObject, const FGameplayTag& DataKey, const int32& Value)
{
	checkf(false, TEXT("Calling a CustomThunk!"));
	return false;
}

DEFINE_FUNCTION(UMDGameDataBlueprintFunctionLibrary::execGetGameDataValue)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_STRUCT_REF(FGameplayTag, DataKey);

	Stack.StepCompiledIn<FProperty>(nullptr);
	const FProperty* ValueProp = Stack.MostRecentProperty;
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (ValueProp == nullptr || ValuePtr == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("GetData_MissingOutputProperty", "Failed to resolve the output parameter for GetData.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

	P_NATIVE_BEGIN
	*StaticCast<bool*>(RESULT_PARAM) = GetDataFromPropertyWithExceptions(WorldContextObject, DataKey, ValueProp, ValuePtr, P_THIS, Stack);
	P_NATIVE_END
}

DEFINE_FUNCTION(UMDGameDataBlueprintFunctionLibrary::execSetGameDataValue)
{
	P_GET_OBJECT(UObject, WorldContextObject);
	P_GET_STRUCT_REF(FGameplayTag, DataKey);

	Stack.StepCompiledIn<FProperty>(nullptr);
	const FProperty* ValueProp = Stack.MostRecentProperty;
	const void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (ValueProp == nullptr || ValuePtr == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("SetData_MissingInputProperty", "Failed to resolve the input parameter for SetData.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

	P_NATIVE_BEGIN
	*StaticCast<bool*>(RESULT_PARAM) = SetDataFromPropertyWithExceptions(WorldContextObject, DataKey, ValueProp, ValuePtr, P_THIS, Stack);
	P_NATIVE_END
}

bool UMDGameDataBlueprintFunctionLibrary::GetDataFromPropertyWithExceptions(UObject* WorldContextObject,
	const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr, const UObject* ContextObject, FFrame& Stack)
{
	const UMDGameDataSubsystem* Subsystem = UMDGameDataSubsystem::Get(WorldContextObject);
	if (Subsystem == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("GetData_ResolveSubsystemFailure", "Failed to retrieve the Game Data Subsystem for GetData.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(ContextObject, Stack, ExceptionInfo);
		return false;
	}
	
	return Subsystem->GetDataFromPropertyWithExceptions(DataKey, Prop, ValuePtr, ContextObject, Stack);
}

bool UMDGameDataBlueprintFunctionLibrary::SetDataFromPropertyWithExceptions(UObject* WorldContextObject,
                                                                            const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr, const UObject* ContextObject, FFrame& Stack)
{
	UMDGameDataSubsystem* Subsystem = UMDGameDataSubsystem::Get(WorldContextObject);
	if (Subsystem == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("SetData_ResolveSubsystemFailure", "Failed to retrieve the Game Data Subsystem for SetData.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(ContextObject, Stack, ExceptionInfo);
		return false;
	}

	return Subsystem->SetDataFromPropertyWithExceptions(DataKey, Prop, ValuePtr, ContextObject, Stack);
}


#undef NSLOCTEXT
