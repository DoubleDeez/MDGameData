#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "MDGameDataNode_SetData.generated.h"

/**
 * 
 */
UCLASS()
class MDGAMEDATAGRAPH_API UMDGameDataNode_SetData : public UK2Node_CallFunction
{
	GENERATED_BODY()

public:
	UMDGameDataNode_SetData();

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
