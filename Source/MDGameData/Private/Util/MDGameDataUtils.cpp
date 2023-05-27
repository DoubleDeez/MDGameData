#include "Util/MDGameDataUtils.h"

#include "UObject/Field.h"
#include "UObject/PropertyPortFlags.h"
#include "UObject/UnrealType.h"


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

FString MDGameDataUtils::GetPropertyValueAsString(const FProperty* Property, const void* ValuePtr)
{
	if (Property == nullptr || ValuePtr == nullptr)
	{
		return TEXT("NULL");
	}

	FString Result;
	Property->ExportTextItem_Direct(Result, ValuePtr, nullptr, nullptr, PPF_DebugDump);

	return Result;
}
