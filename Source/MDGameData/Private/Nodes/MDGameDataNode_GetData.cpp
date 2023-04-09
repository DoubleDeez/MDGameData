#include "Nodes/MDGameDataNode_GetData.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Util/MDGameDataBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "MDGameDataNode_GetData"

UMDGameDataNode_GetData::UMDGameDataNode_GetData()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UMDGameDataBlueprintFunctionLibrary, GetGameDataValue), UMDGameDataBlueprintFunctionLibrary::StaticClass());
}

void UMDGameDataNode_GetData::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Validate that the pins are what we expect
	struct FPinStatics
	{
		int32 InputSelfPinIndex = -1;
		int32 InputDataKeyPinIndex = -1;

		int32 OutputValuePinIndex = -1;
		int32 OutputResultPinIndex = -1;

		FPinStatics(const TArray<UEdGraphPin*>& InPins)
		{
			// Pins as per UMDGameDataBlueprintFunctionLibrary::GetData(...)
			InputSelfPinIndex = FindPinByName(InPins, TEXT("self"));		// default name, won't change
			InputDataKeyPinIndex = FindPinByName(InPins, TEXT("DataKey"));

			OutputValuePinIndex = FindPinByName(InPins, TEXT("OutValue"));
			OutputResultPinIndex = FindPinByName(InPins, TEXT("ReturnValue"));	// default name, won't change
		}

		static int32 FindPinByName(const TArray<UEdGraphPin*>& InPins, const FName& InName)
		{
			return InPins.IndexOfByPredicate([&InName](const UEdGraphPin* InPin)
			{
				return InPin->GetFName() == InName;
			});			
		}
	};
	static FPinStatics PinInfo(Pins);
	
	UEdGraphPin* InputSelfPin = GetPinAt(PinInfo.InputSelfPinIndex);	
	check(InputSelfPin);
	
	UEdGraphPin* InputDataKeyPin = GetPinAt(PinInfo.InputDataKeyPinIndex);	
	check(InputDataKeyPin);

	UEdGraphPin* OutputValuePin = GetPinAt(PinInfo.OutputValuePinIndex);
	check(OutputValuePin);
	
	UEdGraphPin* OutputResultPin = GetPinAt(PinInfo.OutputResultPinIndex);
	check(OutputResultPin);
}

FText UMDGameDataNode_GetData::GetTooltipText() const
{
	return LOCTEXT("TooltipText", "Get the specified Game Data Value, the output target property must match the stored value's type.");
}

bool UMDGameDataNode_GetData::IsNodePure() const
{
	return false;
}

void UMDGameDataNode_GetData::GetMenuActions(FBlueprintActionDatabaseRegistrar& InActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	if (InActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		InActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UMDGameDataNode_GetData::GetMenuCategory() const
{
	static FText MenuCategory = LOCTEXT("MenuCategory", "Game Data");
	return MenuCategory;
}

#undef LOCTEXT_NAMESPACE
