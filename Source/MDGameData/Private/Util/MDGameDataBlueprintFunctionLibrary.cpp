#include "Util/MDGameDataBlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "MDGameDataContainer.h"
#include "Components/MDGameDataComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Subsystems/MDGlobalGameDataSubsystem.h"
#include "Subsystems/MDLocalPlayerGameDataSubsystem.h"
#include "Subsystems/MDWorldGameDataSubsystem.h"
#include "Util/MDGameDataLogging.h"
#include "Util\MDGameDataTypes.h"

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
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("GetDataFailed", "Failed to GetData for key [{0}], caller is expecting type [{1}] but entry is of type [{2}]"),
				FText::FromName(DataKey.GetTagName()),
				FText::FromString(MDGameDataUtils::GetPropertyTypeAsString(ValueProp)),
				FText::FromString(GameDataContainer->GetEntryTypeString(DataKey))
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
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("SetDataFailed", "Failed to SetData for key [{0}], caller is expecting type [{1}] but entry is of type [{2}]"),
				FText::FromName(DataKey.GetTagName()),
				FText::FromString(MDGameDataUtils::GetPropertyTypeAsString(ValueProp)),
				FText::FromString(GameDataContainer->GetEntryTypeString(DataKey))
			)
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		*StaticCast<bool*>(RESULT_PARAM) = false;
		return;
	}

	*StaticCast<bool*>(RESULT_PARAM) = true;
}

UMDGameDataContainer* UMDGameDataBlueprintFunctionLibrary::FindGameDataContainerForActor(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	const UMDGameDataComponent* Component = Actor->FindComponentByClass<UMDGameDataComponent>();
	if (!IsValid(Component))
	{
		return nullptr;
	}

	return Component->GetGameDataContainer();
}

UMDGameDataContainer* UMDGameDataBlueprintFunctionLibrary::FindOrCreateGameDataContainerForActor(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}

	const UMDGameDataComponent* Component = Actor->FindComponentByClass<UMDGameDataComponent>();
	if (!IsValid(Component))
	{
		constexpr bool bManualAttachment = false;
		constexpr bool bDeferredFinish = false;
		Component = Cast<UMDGameDataComponent>(Actor->AddComponentByClass(UMDGameDataComponent::StaticClass(), bManualAttachment, FTransform::Identity, bDeferredFinish));
	}

	return Component->GetGameDataContainer();
}

UMDGameDataContainer* UMDGameDataBlueprintFunctionLibrary::ResolveGameDataSource(EMDGameDataContainerSource Source, UObject* Context)
{
	if (Source == EMDGameDataContainerSource::Global)
	{
		return UMDGlobalGameDataSubsystem::GetGlobalGameDataContainer(Context);
	}

	if (Source == EMDGameDataContainerSource::World)
	{
		return UMDWorldGameDataSubsystem::GetWorldGameDataContainer(Context);
	}

	if (Source == EMDGameDataContainerSource::LocalPlayer)
	{
		return UMDLocalPlayerGameDataSubsystem::GetFirstLocalPlayerGameDataContainer(Context);
	}

	if (Source == EMDGameDataContainerSource::Component)
	{
		if (const AActor* Actor = Cast<AActor>(Context))
		{
			return FindGameDataContainerForActor(Actor);
		}
		else
		{
			UE_LOG(LogMDGameData, Error, TEXT("UMDGameDataBlueprintFunctionLibrary::ResolveGameDataSource called with Source type 'Actor' without a valid Actor Context"));
			return nullptr;
		}
	}

	return nullptr;
}


#undef NSLOCTEXT
