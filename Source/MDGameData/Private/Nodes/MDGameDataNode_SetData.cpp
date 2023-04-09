#include "Nodes/MDGameDataNode_SetData.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Util/MDGameDataBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "MDGameDataNode_SetData"

UMDGameDataNode_SetData::UMDGameDataNode_SetData()
{
	FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UMDGameDataBlueprintFunctionLibrary, SetGameDataValue), UMDGameDataBlueprintFunctionLibrary::StaticClass());
}

void UMDGameDataNode_SetData::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	// Validate that the pins are what we expect
	struct FPinStatics
	{
		int32 InputSelfPinIndex = -1;
		int32 InputDataKeyPinIndex = -1;
		int32 InputValuePinIndex = -1;

		int32 OutputResultPinIndex = -1;

		FPinStatics(const TArray<UEdGraphPin*>& InPins)
		{
			// Pins as per UMDGameDataBlueprintFunctionLibrary::SetData(...)
			InputSelfPinIndex = FindPinByName(InPins, TEXT("self"));		// default name, won't change
			InputDataKeyPinIndex = FindPinByName(InPins, TEXT("DataKey"));
			InputValuePinIndex = FindPinByName(InPins, TEXT("Value"));

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

	UEdGraphPin* InputValuePin = GetPinAt(PinInfo.InputValuePinIndex);	
	check(InputValuePin);

	UEdGraphPin* OutputResultPin = GetPinAt(PinInfo.OutputResultPinIndex);
	check(OutputResultPin);
}

FText UMDGameDataNode_SetData::GetTooltipText() const
{
	return LOCTEXT("TooltipText", "Set the specified Game Data Value, the type must match the stored value's type if there is one.");
}

bool UMDGameDataNode_SetData::IsNodePure() const
{
	return false;
}

void UMDGameDataNode_SetData::GetMenuActions(FBlueprintActionDatabaseRegistrar& InActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	if (InActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		InActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UMDGameDataNode_SetData::GetMenuCategory() const
{
	static FText MenuCategory = LOCTEXT("MenuCategory", "Game Data");
	return MenuCategory;
}

#undef LOCTEXT_NAMESPACE
