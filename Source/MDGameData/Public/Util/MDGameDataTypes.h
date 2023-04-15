#pragma once

#include "CoreMinimal.h"
#include "MDGameDataTypes.generated.h"

UENUM(BlueprintType)
enum class EMDGameDataContainerSource : uint8
{
	Global,
	World,
	LocalPlayer,
	Component,
	Other
};
