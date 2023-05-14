#include "Util/MDGameDataUtils.h"


using namespace MDGameDataUtils;

FString MDGameDataUtils::GetPropertyTypeAsString(const FProperty* Property)
{
	if (Property == nullptr)
	{
		return TEXT("None");
	}

	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		return FString::Printf(TEXT("TArray<%s>"), *GetPropertyTypeAsString(ArrayProperty->Inner));
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		return FString::Printf(TEXT("TSet<%s>"), *GetPropertyTypeAsString(SetProperty->ElementProp));
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		return FString::Printf(TEXT("TMap<%s, %s>"), *GetPropertyTypeAsString(MapProperty->KeyProp), *GetPropertyTypeAsString(MapProperty->ValueProp));
	}

	return Property->GetCPPType();
}
