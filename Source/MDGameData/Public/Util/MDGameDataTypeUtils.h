#pragma once

#include "CoreMinimal.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"

#define MDGAMEDATA_DECLARETYPEUTILS(CPP_TYPE) template<> \
struct MDGAMEDATA_API TMDGameDataTypeUtils<CPP_TYPE> \
{ \
	static const FProperty* ConstructProperty(const FName& PropName); \
};

#define MDGAMEDATA_DEFINETYPEUTILS(CPP_TYPE, PROP_TYPE) \
const FProperty* TMDGameDataTypeUtils<CPP_TYPE>::ConstructProperty(const FName& PropName) \
{ \
	return static_cast<FProperty*>(PROP_TYPE::Construct({}, PropName, RF_Transient | RF_Public)); \
}

template<typename T, typename U = T>
struct MDGAMEDATA_API TMDGameDataTypeUtils
{
	static const FProperty* ConstructProperty(const FName& PropName)
	{
		static_assert(false, "Missing implementation of TMDGameDataTypeUtils<T>::ConstructProperty");
		return nullptr;
	}
};

MDGAMEDATA_DECLARETYPEUTILS(int8)
MDGAMEDATA_DECLARETYPEUTILS(int16)
MDGAMEDATA_DECLARETYPEUTILS(int32)
MDGAMEDATA_DECLARETYPEUTILS(int64)
MDGAMEDATA_DECLARETYPEUTILS(uint8)
MDGAMEDATA_DECLARETYPEUTILS(char)
MDGAMEDATA_DECLARETYPEUTILS(uint16)
MDGAMEDATA_DECLARETYPEUTILS(uint32)
MDGAMEDATA_DECLARETYPEUTILS(uint64)
MDGAMEDATA_DECLARETYPEUTILS(float)
MDGAMEDATA_DECLARETYPEUTILS(double)

MDGAMEDATA_DECLARETYPEUTILS(FString)
MDGAMEDATA_DECLARETYPEUTILS(FName)
MDGAMEDATA_DECLARETYPEUTILS(FText)

template<typename T>
struct MDGAMEDATA_API TMDGameDataTypeUtils
<TSubclassOf<T>, TSubclassOf<T>>
{
	static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FClassProperty* ClassProperty = static_cast<FClassProperty*>(FClassProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ClassProperty->SetMetaClass(T::StaticClass());
		return ClassProperty;
	}
};

template<typename T>
struct MDGAMEDATA_API TMDGameDataTypeUtils
<T, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T>::Type>
{
	static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(FObjectProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		return ObjectProperty;
	}
};

template<typename T>
struct MDGAMEDATA_API TMDGameDataTypeUtils
<T, typename TEnableIf<TIsUEnumClass<T>::Value, T>::Type>
{
	static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(FEnumProperty::Construct({}, PropName, RF_Transient | RF_Public));
		EnumProperty->SetEnum(StaticEnum<T>());
		static_assert(false, "Incomplete");
		
		// TODO - is this right?
		FProperty* UnderlyingProperty = static_cast<FByteProperty*>(FByteProperty::Construct(EnumProperty, PropName, RF_Transient | RF_Public));
		EnumProperty->AddCppProperty(UnderlyingProperty);
		return EnumProperty;
	}
};
