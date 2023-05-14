#pragma once

#include "CoreMinimal.h"
#include <typeinfo>
#include "MDGameDataTypeUtils.h"

template<typename T>
using TMDGameDataForceResolveRawType = typename TRemoveCV<typename TRemoveObjectPointer<typename TRemoveCV<typename TRemovePointer<T>::Type>::Type>::Type>::Type;

template<typename T, typename ForcedRawT = TMDGameDataForceResolveRawType<T>>
using TMDGameDataResolvedType = typename TChooseClass<TIsDerivedFrom<ForcedRawT, UObject>::IsDerived, ForcedRawT, T>::Result;

/**
 * Utilities for internal usage
 */
namespace MDGameDataUtils
{
	template<typename T, typename RawT = TMDGameDataResolvedType<T>>
	FORCEINLINE static const FProperty* GetPropertyForType()
	{
		static const FProperty* StaticProp = nullptr;
		if (StaticProp == nullptr)
		{
			const FName PropName = *FString::Printf(TEXT("MDGameDataProp_%s"), ANSI_TO_TCHAR(typeid(RawT).name()));
			StaticProp = TMDGameDataTypeUtils<RawT>::ConstructProperty(PropName);
		}

		return StaticProp;
	}

	MDGAMEDATA_API FString GetPropertyTypeAsString(const FProperty* Property);
};
