#pragma once

#include "Containers/UnrealString.h"

class FProperty;

namespace MDGameDataUtils
{
	MDGAMEDATA_API FString GetPropertyTypeAsString(const FProperty* Property);
	MDGAMEDATA_API FString GetPropertyValueAsString(const FProperty* Property, const void* ValuePtr);
};
