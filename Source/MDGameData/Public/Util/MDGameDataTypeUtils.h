#pragma once

#include "CoreMinimal.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"

#define MDGAMEDATA_DECLARETYPEUTILS(CPP_TYPE) template<> \
struct TMDGameDataTypeUtils<CPP_TYPE> \
{ \
	MDGAMEDATA_API static const FProperty* ConstructProperty(const FName& PropName); \
};

#define MDGAMEDATA_DEFINETYPEUTILS(CPP_TYPE, PROP_TYPE) \
const FProperty* TMDGameDataTypeUtils<CPP_TYPE>::ConstructProperty(const FName& PropName) \
{ \
	return static_cast<FProperty*>(PROP_TYPE::Construct({}, PropName, RF_Transient | RF_Public)); \
}

#define MDGAMEDATA_DEFINETYPEUTILS_STRUCT(CPP_TYPE) \
const FProperty* TMDGameDataTypeUtils<CPP_TYPE>::ConstructProperty(const FName& PropName) \
{ \
	FStructProperty* StructProperty = static_cast<FStructProperty*>(FStructProperty::Construct({}, PropName, RF_Transient | RF_Public));\
	StructProperty->Struct = TBaseStructure<CPP_TYPE>::Get();\
	return StructProperty;\
}

template<typename T, typename = void>
struct TMDGameDataTypeUtils
{
	MDGAMEDATA_API static const FProperty* ConstructProperty(const FName& PropName)
	{
		static_assert(false, "Missing implementation of TMDGameDataTypeUtils<T>::ConstructProperty");
		return nullptr;
	}
};

template<typename ...Ts>
struct TMDGDVoid { using Type = void; };
template<auto ...Ts>
struct TMDGDVoidValue { using Type = void; };

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

MDGAMEDATA_DECLARETYPEUTILS(FIntPoint)
MDGAMEDATA_DECLARETYPEUTILS(FIntVector)
MDGAMEDATA_DECLARETYPEUTILS(FIntVector4)
struct FLinearColor;
MDGAMEDATA_DECLARETYPEUTILS(FLinearColor)
struct FColor;
MDGAMEDATA_DECLARETYPEUTILS(FColor)
struct FRandomStream;
MDGAMEDATA_DECLARETYPEUTILS(FRandomStream)
struct FGuid;
MDGAMEDATA_DECLARETYPEUTILS(FGuid)
struct FFallbackStruct;
MDGAMEDATA_DECLARETYPEUTILS(FFallbackStruct)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointFloat)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointVector2D)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointVector)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointQuat)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointTwoVectors)
MDGAMEDATA_DECLARETYPEUTILS(FInterpCurvePointLinearColor)
struct FFloatRangeBound;
MDGAMEDATA_DECLARETYPEUTILS(FFloatRangeBound)
struct FFloatRange;
MDGAMEDATA_DECLARETYPEUTILS(FFloatRange)
/*struct FDoubleRangeBound;
MDGAMEDATA_DECLARETYPEUTILS(FDoubleRangeBound)*/
/*struct FDoubleRange;
MDGAMEDATA_DECLARETYPEUTILS(FDoubleRange)*/
struct FInt32RangeBound;
MDGAMEDATA_DECLARETYPEUTILS(FInt32RangeBound)
struct FInt32Range;
MDGAMEDATA_DECLARETYPEUTILS(FInt32Range)
struct FFloatInterval;
MDGAMEDATA_DECLARETYPEUTILS(FFloatInterval)
/*struct FDoubleInterval;
MDGAMEDATA_DECLARETYPEUTILS(FDoubleInterval)*/
struct FInt32Interval;
MDGAMEDATA_DECLARETYPEUTILS(FInt32Interval)
struct FFrameNumber;
MDGAMEDATA_DECLARETYPEUTILS(FFrameNumber)
struct FFrameTime;
MDGAMEDATA_DECLARETYPEUTILS(FFrameTime)
struct FSoftObjectPath;
MDGAMEDATA_DECLARETYPEUTILS(FSoftObjectPath)
struct FSoftClassPath;
MDGAMEDATA_DECLARETYPEUTILS(FSoftClassPath)
struct FPrimaryAssetType;
MDGAMEDATA_DECLARETYPEUTILS(FPrimaryAssetType)
struct FPrimaryAssetId;
MDGAMEDATA_DECLARETYPEUTILS(FPrimaryAssetId)
struct FDateTime;
MDGAMEDATA_DECLARETYPEUTILS(FDateTime)
struct FPolyglotTextData;
MDGAMEDATA_DECLARETYPEUTILS(FPolyglotTextData)
struct FAssetBundleData;
MDGAMEDATA_DECLARETYPEUTILS(FAssetBundleData)
struct FTestUninitializedScriptStructMembersTest;
MDGAMEDATA_DECLARETYPEUTILS(FTestUninitializedScriptStructMembersTest)
struct FTopLevelAssetPath;
MDGAMEDATA_DECLARETYPEUTILS(FTopLevelAssetPath)

MDGAMEDATA_DECLARETYPEUTILS(FVector2D)
MDGAMEDATA_DECLARETYPEUTILS(FVector)
MDGAMEDATA_DECLARETYPEUTILS(FVector4)
MDGAMEDATA_DECLARETYPEUTILS(FPlane)
MDGAMEDATA_DECLARETYPEUTILS(FQuat)
MDGAMEDATA_DECLARETYPEUTILS(FRotator)
MDGAMEDATA_DECLARETYPEUTILS(FTransform)
MDGAMEDATA_DECLARETYPEUTILS(FMatrix)
MDGAMEDATA_DECLARETYPEUTILS(FBox2D)
/*MDGAMEDATA_DECLARETYPEUTILS(FRay)*/
/*MDGAMEDATA_DECLARETYPEUTILS(FSphere)*/

template<typename T>
struct TMDGameDataTypeUtils
<TSubclassOf<T>>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FClassProperty* ClassProperty = static_cast<FClassProperty*>(FClassProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ClassProperty->SetMetaClass(T::StaticClass());
		return ClassProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived>::Type>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(FObjectProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TWeakObjectPtr<T>>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FWeakObjectProperty* ObjectProperty = static_cast<FWeakObjectProperty*>(FWeakObjectProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TObjectPtr<T>>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FObjectPtrProperty* ObjectProperty = static_cast<FObjectPtrProperty*>(FObjectPtrProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TMDGDVoid<decltype(T::StaticStruct())>::Type>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FStructProperty* StructProperty = static_cast<FStructProperty*>(FStructProperty::Construct({}, PropName, RF_Transient | RF_Public));
		StructProperty->Struct = T::StaticStruct();//(TBaseStructure<T>::Get());
		return StructProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TEnableIf<TIsUEnumClass<T>::Value>::Type>
{
	MDGAMEDATA_API static FORCEINLINE const FProperty* ConstructProperty(const FName& PropName)
	{
		FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(FEnumProperty::Construct({}, PropName, RF_Transient | RF_Public));
		EnumProperty->SetEnum(StaticEnum<T>());

		static const FName UnderlyingPropertyName = TEXT("UnderlyingType");
		FProperty* UnderlyingProperty = static_cast<FByteProperty*>(FByteProperty::Construct(EnumProperty, UnderlyingPropertyName, RF_Transient | RF_Public));
		EnumProperty->AddCppProperty(UnderlyingProperty);
		return EnumProperty;
	}
};
