#include "Util/MDGameDataBlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "MDGameDataContainer.h"

#define LOCTEXT_NAMESPACE "MDGameDataBlueprintFunctionLibrary"

bool UMDGameDataBlueprintFunctionLibrary::GetGameDataValue(UMDGameDataContainer* GameDataContainer, const FGameplayTag& DataKey, int32& OutValue)
{
	checkf(false, TEXT("Calling a CustomThunk!"));
	return false;
}

bool UMDGameDataBlueprintFunctionLibrary::SetGameDataValue(UMDGameDataContainer* GameDataContainer, const FGameplayTag& DataKey, const int32& Value)
{
	checkf(false, TEXT("Calling a CustomThunk!"));
	return false;
}

DEFINE_FUNCTION(UMDGameDataBlueprintFunctionLibrary::execGetGameDataValue)
{
	P_GET_OBJECT(UMDGameDataContainer, GameDataContainer);
	P_GET_STRUCT_REF(FGameplayTag, DataKey);

	Stack.StepCompiledIn<FProperty>(nullptr);
	const FProperty* ValueProp = Stack.MostRecentProperty;
	void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (GameDataContainer == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("GetData_MissingGameDataContainer", "A valid Game Data Container must be passed in.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

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

	EMDGameDataContainerResult Result;
	P_NATIVE_BEGIN
	Result = GameDataContainer->GetDataFromProperty(DataKey, ValueProp, ValuePtr);
	P_NATIVE_END

	if (Result == EMDGameDataContainerResult::Failure_TypeMismatch)
	{
		// TODO - better type naming than just the property type (eg. element types of array, class type of objects, etc)
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("GetDataFailed", "Failed to GetData for key [{0}], caller is expecting type [{1}]"),
				FText::FromName(DataKey.GetTagName()),
				ValueProp->GetClass()->GetDisplayNameText()
			)
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}
	else if (Result == EMDGameDataContainerResult::Failure_NoEntry)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("GetDataFailed", "Failed to GetData for key [{0}], the entry doesn't exist"),
				FText::FromName(DataKey.GetTagName())
			)
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}
	
	*StaticCast<bool*>(RESULT_PARAM) = true;
}

DEFINE_FUNCTION(UMDGameDataBlueprintFunctionLibrary::execSetGameDataValue)
{
	P_GET_OBJECT(UMDGameDataContainer, GameDataContainer);
	P_GET_STRUCT_REF(FGameplayTag, DataKey);

	Stack.StepCompiledIn<FProperty>(nullptr);
	const FProperty* ValueProp = Stack.MostRecentProperty;
	const void* ValuePtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (GameDataContainer == nullptr)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			LOCTEXT("SetData_MissingGameDataContainer", "A valid Game Data Container must be passed in.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

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

	EMDGameDataContainerResult Result;
	P_NATIVE_BEGIN
	Result = GameDataContainer->SetDataFromProperty(DataKey, ValueProp, ValuePtr);
	P_NATIVE_END
	
	if (Result == EMDGameDataContainerResult::Failure_TypeMismatch)
	{
		// TODO - better type naming than just the property type (eg. element types of array, class type of objects, etc)
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("SetDataFailed", "Failed to SetData for key [{0}], caller is expecting type [{1}]"),
				FText::FromName(DataKey.GetTagName()),
				ValueProp->GetClass()->GetDisplayNameText()
			)
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

	*StaticCast<bool*>(RESULT_PARAM) = true;
}


#undef NSLOCTEXT
