#pragma once

#include "CoreMinimal.h"

namespace MDGDPropertyConstructors
{
	inline FProperty* CreatePropertyFromCastFlags(const FName& Name, EClassCastFlags Flags)
	{
		if (Flags & CASTCLASS_FClassPtrProperty)
		{
			return static_cast<FProperty*>(FClassPtrProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}			
		else if (Flags & CASTCLASS_FClassProperty)
		{
			return static_cast<FProperty*>(FClassProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}
		else if (Flags & CASTCLASS_FObjectPtrProperty)
		{
			return static_cast<FProperty*>(FObjectPtrProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}
		else if (Flags & CASTCLASS_FObjectProperty)
		{
			return static_cast<FProperty*>(FObjectProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}		
		else if (Flags & CASTCLASS_FSoftClassProperty)
		{
			return static_cast<FProperty*>(FSoftClassProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}						
		else if (Flags & CASTCLASS_FSoftObjectProperty)
		{
			return static_cast<FProperty*>(FSoftObjectProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}					
		else if (Flags & CASTCLASS_FLazyObjectProperty)
		{
			return static_cast<FProperty*>(FLazyObjectProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FWeakObjectProperty)
		{
			return static_cast<FProperty*>(FWeakObjectProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}		
		else if (Flags & CASTCLASS_FEnumProperty)
		{
			return static_cast<FProperty*>(FEnumProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}					
		else if (Flags & CASTCLASS_FDoubleProperty)
		{
			return static_cast<FProperty*>(FDoubleProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FInt16Property)
		{
			return static_cast<FProperty*>(FInt16Property::Construct(GetTransientPackage(), Name, RF_Public));
		}			
		else if (Flags & CASTCLASS_FTextProperty)
		{
			return static_cast<FProperty*>(FTextProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}	
		else if (Flags & CASTCLASS_FInt64Property)
		{
			return static_cast<FProperty*>(FInt64Property::Construct(GetTransientPackage(), Name, RF_Public));
		}	
		else if (Flags & CASTCLASS_FUInt16Property)
		{
			return static_cast<FProperty*>(FUInt16Property::Construct(GetTransientPackage(), Name, RF_Public));
		}			
		else if (Flags & CASTCLASS_FBoolProperty)
		{
			return static_cast<FProperty*>(FBoolProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}
		else if (Flags & CASTCLASS_FStrProperty)
		{
			return static_cast<FProperty*>(FStrProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}			
		else if (Flags & CASTCLASS_FNameProperty)
		{
			return static_cast<FProperty*>(FNameProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}					
		else if (Flags & CASTCLASS_FInterfaceProperty)
		{
			return static_cast<FProperty*>(FInterfaceProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FUInt32Property)
		{
			return static_cast<FProperty*>(FUInt32Property::Construct(GetTransientPackage(), Name, RF_Public));
		}	
		else if (Flags & CASTCLASS_FUInt64Property)
		{
			return static_cast<FProperty*>(FUInt64Property::Construct(GetTransientPackage(), Name, RF_Public));
		}					
		else if (Flags & CASTCLASS_FFloatProperty)
		{
			return static_cast<FProperty*>(FFloatProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}					
		else if (Flags & CASTCLASS_FIntProperty)
		{
			return static_cast<FProperty*>(FIntProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}
		else if (Flags & CASTCLASS_FByteProperty)
		{
			return static_cast<FProperty*>(FByteProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}			
		else if (Flags & CASTCLASS_FInt8Property)
		{
			return static_cast<FProperty*>(FInt8Property::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FSetProperty)
		{
			return static_cast<FProperty*>(FSetProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FMapProperty)
		{
			return static_cast<FProperty*>(FMapProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FArrayProperty)
		{
			return static_cast<FProperty*>(FArrayProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}				
		else if (Flags & CASTCLASS_FStructProperty)
		{
			return static_cast<FProperty*>(FStructProperty::Construct(GetTransientPackage(), Name, RF_Public));
		}

		return nullptr;
	}
}