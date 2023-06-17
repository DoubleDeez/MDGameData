#pragma once

#include "K2Node_CallFunction.h"
#include "MDGameDataNode_GetData.generated.h"

/**
 *
 */
UCLASS()
class MDGAMEDATAGRAPH_API UMDGameDataNode_GetData : public UK2Node_CallFunction
{
	GENERATED_BODY()

public:
	UMDGameDataNode_GetData();

	// //~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	// //~ End UEdGraphNode Interface.

	//~ Begin K2Node Interface
	virtual bool IsNodePure() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& InActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End K2Node Interface
};
