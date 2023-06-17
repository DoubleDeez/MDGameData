#pragma once

#include "MDGameDataUtils.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"

#define MDGAMEDATA_DECLARETYPEUTILS(CPP_TYPE) template<> \
struct TMDGameDataTypeUtils<CPP_TYPE> \
{ \
	MDGAMEDATA_API static const TCHAR* GetTypeName() { return TEXT( #CPP_TYPE ); } \
	MDGAMEDATA_API static FProperty* ConstructProperty(const FName& PropName); \
};

#define MDGAMEDATA_DEFINETYPEUTILS(CPP_TYPE, PROP_TYPE) \
FProperty* TMDGameDataTypeUtils<CPP_TYPE>::ConstructProperty(const FName& PropName) \
{ \
	FProperty* Prop = static_cast<FProperty*>(PROP_TYPE::Construct({}, PropName, RF_Transient | RF_Public)); \
	Prop->SetPropertyFlags(CPF_HasGetValueTypeHash); \
	return Prop;\
}

#define MDGAMEDATA_DEFINETYPEUTILS_STRUCT(CPP_TYPE) \
FProperty* TMDGameDataTypeUtils<CPP_TYPE>::ConstructProperty(const FName& PropName) \
{ \
	FStructProperty* StructProperty = static_cast<FStructProperty*>(FStructProperty::Construct({}, PropName, RF_Transient | RF_Public));\
	StructProperty->Struct = TBaseStructure<CPP_TYPE>::Get();\
	StructProperty->ElementSize = StructProperty->Struct->PropertiesSize;\
	if (StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasGetTypeHash())\
	{\
		StructProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);\
	}\
	if (StructProperty->Struct->StructFlags & STRUCT_HasInstancedReference)\
	{\
		StructProperty->SetPropertyFlags(CPF_ContainsInstancedReference);\
	}\
	return StructProperty;\
}

template<typename T, typename = void>
struct TMDGameDataTypeUtils
{
	static const TCHAR* GetTypeName() { return TEXT("INVALID"); }

	static FProperty* ConstructProperty(const FName& PropName)
	{
		static_assert(sizeof(T) == 0, "Missing implementation of TMDGameDataTypeUtils<T>::ConstructProperty");
		return nullptr;
	}
};

template<typename T>
using TMDGameDataForceResolveRawType = typename TRemoveCV<typename TRemoveObjectPointer<typename TRemoveCV<typename TRemovePointer<T>::Type>::Type>::Type>::Type;

template<typename T, typename ForcedRawT = TMDGameDataForceResolveRawType<T>>
using TMDGameDataResolvedType = typename TChooseClass<TIsDerivedFrom<ForcedRawT, UObject>::IsDerived, ForcedRawT, T>::Result;

namespace MDGameDataUtils
{
	template<typename T, typename RawT = TMDGameDataResolvedType<T>>
	FORCEINLINE static FProperty* GetPropertyForType()
	{
		static FProperty* StaticProp = nullptr;
		if (StaticProp == nullptr)
		{
			const FName PropName = *FString::Printf(TEXT("MDGameDataProp_%s"), TMDGameDataTypeUtils<RawT>::GetTypeName());
			StaticProp = TMDGameDataTypeUtils<RawT>::ConstructProperty(PropName);
		}

		return StaticProp;
	}
}

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
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TSubclassOf<%s>"), TMDGameDataTypeUtils<T>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FClassProperty* ClassProperty = static_cast<FClassProperty*>(FClassProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ClassProperty->SetMetaClass(T::StaticClass());
		ClassProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		return ClassProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived>::Type>
{
	static const TCHAR* GetTypeName() { return *T::StaticClass()->GetName(); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(FObjectProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		if (ObjectProperty->PropertyClass->HasAnyClassFlags(CLASS_DefaultToInstanced))
		{
			ObjectProperty->SetPropertyFlags(CPF_InstancedReference);
		}
		ObjectProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TWeakObjectPtr<T>>
{
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TWeakObjectPtr<%s>"), TMDGameDataTypeUtils<T>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FWeakObjectProperty* ObjectProperty = static_cast<FWeakObjectProperty*>(FWeakObjectProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		if (ObjectProperty->PropertyClass->HasAnyClassFlags(CLASS_DefaultToInstanced))
		{
			ObjectProperty->SetPropertyFlags(CPF_InstancedReference);
		}
		ObjectProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TObjectPtr<T>>
{
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TObjectPtr<%s>"), TMDGameDataTypeUtils<T>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FObjectPtrProperty* ObjectProperty = static_cast<FObjectPtrProperty*>(FObjectPtrProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ObjectProperty->SetPropertyClass(T::StaticClass());
		if (ObjectProperty->PropertyClass->HasAnyClassFlags(CLASS_DefaultToInstanced))
		{
			ObjectProperty->SetPropertyFlags(CPF_InstancedReference);
		}
		ObjectProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		return ObjectProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TMDGDVoid<decltype(T::StaticStruct())>::Type>
{
	static const TCHAR* GetTypeName() { return *T::StaticStruct()->GetName(); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FStructProperty* StructProperty = static_cast<FStructProperty*>(FStructProperty::Construct({}, PropName, RF_Transient | RF_Public));
		StructProperty->Struct = T::StaticStruct();
		StructProperty->ElementSize = StructProperty->Struct->PropertiesSize;

		if (StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasGetTypeHash())
		{
			StructProperty->SetPropertyFlags(CPF_HasGetValueTypeHash);
		}

		if (StructProperty->Struct->StructFlags & STRUCT_HasInstancedReference)
		{
			StructProperty->SetPropertyFlags(CPF_ContainsInstancedReference);
		}

		return StructProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<T, typename TEnableIf<TIsEnumClass<T>::Value>::Type>
{
	static const TCHAR* GetTypeName() { return *StaticEnum<T>()->CppType; }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FEnumProperty* EnumProperty = static_cast<FEnumProperty*>(FEnumProperty::Construct({}, PropName, RF_Transient | RF_Public));
		EnumProperty->SetEnum(StaticEnum<T>());

		static const FName UnderlyingPropertyName = TEXT("UnderlyingType");
		FProperty* UnderlyingProperty = static_cast<FByteProperty*>(FByteProperty::Construct(EnumProperty, UnderlyingPropertyName, RF_Transient | RF_Public));
		EnumProperty->AddCppProperty(UnderlyingProperty);
		return EnumProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TArray<T>>
{
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TArray<%s>"), TMDGameDataTypeUtils<T>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FArrayProperty* ArrayProperty = static_cast<FArrayProperty*>(FArrayProperty::Construct({}, PropName, RF_Transient | RF_Public));
		ArrayProperty->Inner = MDGameDataUtils::GetPropertyForType<T>();
		return ArrayProperty;
	}
};

template<typename T>
struct TMDGameDataTypeUtils
<TSet<T>>
{
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TSet<%s>"), TMDGameDataTypeUtils<T>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FSetProperty* SetProperty = static_cast<FSetProperty*>(FSetProperty::Construct({}, PropName, RF_Transient | RF_Public));
		SetProperty->ElementProp = MDGameDataUtils::GetPropertyForType<T>();
		return SetProperty;
	}
};

template<typename T, typename U>
struct TMDGameDataTypeUtils
<TMap<T, U>>
{
	static const TCHAR* GetTypeName() { return *FString::Printf(TEXT("TMap<%s, %s>"), TMDGameDataTypeUtils<T>::GetTypeName(), TMDGameDataTypeUtils<U>::GetTypeName()); }
	static FORCEINLINE FProperty* ConstructProperty(const FName& PropName)
	{
		FMapProperty* MapProperty = static_cast<FMapProperty*>(FMapProperty::Construct({}, PropName, RF_Transient | RF_Public));
		MapProperty->KeyProp = MDGameDataUtils::GetPropertyForType<T>();
		MapProperty->ValueProp = MDGameDataUtils::GetPropertyForType<U>();
		return MapProperty;
	}
};
