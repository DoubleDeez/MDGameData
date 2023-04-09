#include "Subsystems/MDGameDataSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/MDGameDataConfig.h"

#define LOCTEXT_NAMESPACE "MDGameDataSubsystem"

FMDGameDataEntry::FMDGameDataEntry(const FProperty* SourceProp)
{
	check(SourceProp != nullptr);
	
	EntryProperty = static_cast<FProperty*>(FField::Duplicate(SourceProp, {}, NAME_None, RF_Transient, EInternalObjectFlags::None));
	// TODO - Consider using pre-allocated blocks of memory for the data entries
	EntryValuePtr = FMemory::Malloc(EntryProperty->GetSize(), EntryProperty->GetMinAlignment());
	EntryProperty->InitializeValue(EntryValuePtr);
}

FMDGameDataEntry::FMDGameDataEntry(FMDGameDataEntry&& Other) noexcept
{
	*this = MoveTemp(Other);
}

FMDGameDataEntry::~FMDGameDataEntry()
{
	if (EntryProperty != nullptr)
	{
		delete EntryProperty;
		EntryProperty = nullptr;
	}

	if (EntryValuePtr != nullptr)
	{
		FMemory::Free(EntryValuePtr);
		EntryValuePtr = nullptr;
	}
}

FMDGameDataEntry& FMDGameDataEntry::operator=(FMDGameDataEntry&& Other) noexcept
{
	EntryProperty = Other.EntryProperty;
	Other.EntryProperty = nullptr;
	
	EntryValuePtr = Other.EntryValuePtr;
	Other.EntryValuePtr = nullptr;
	
	return *this;
}

UMDGameDataSubsystem* UMDGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine != nullptr)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return UGameInstance::GetSubsystem<UMDGameDataSubsystem>(World->GetGameInstance());
		}
	}
	
	return nullptr;
}

void UMDGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UMDGameDataConfig* Config = GetDefault<UMDGameDataConfig>();

	DataEntries.Reserve(Config->InitialGameDataReserveSize);
}

void UMDGameDataSubsystem::Deinitialize()
{
	Super::Deinitialize();

	DataEntries.Reset();
}

TTuple<const FProperty*, const void*> UMDGameDataSubsystem::GetData(const FGameplayTag& DataKey) const
{
	const FMDGameDataEntry* Entry = DataEntries.Find(DataKey);
	if (Entry == nullptr)
	{
		return {};
	}

	return { Entry->EntryProperty, Entry->EntryValuePtr };
}

bool UMDGameDataSubsystem::SetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, const void* ValuePtr)
{
	if (Prop == nullptr || ValuePtr == nullptr || !DataKey.IsValid())
	{
		return false;
	}
	
	if (const FMDGameDataEntry* Entry = DataEntries.Find(DataKey))
	{
		// TODO - better type naming than just the property type (eg. element types of array, class type of objects, etc)
		if (!ensure(Entry->EntryProperty->SameType(Prop)))
		{
			return false;
		}

		Entry->EntryProperty->CopyCompleteValue(Entry->EntryValuePtr, ValuePtr);
		return true;
	}
	else
	{
		const FMDGameDataEntry& NewEntry = DataEntries.Emplace(DataKey, FMDGameDataEntry(Prop));
		NewEntry.EntryProperty->CopyCompleteValue(NewEntry.EntryValuePtr, ValuePtr);
		return true;
	}
}

bool UMDGameDataSubsystem::GetDataFromProperty(const FGameplayTag& DataKey, const FProperty* Prop, void* ValuePtr) const
{
	if (Prop == nullptr || ValuePtr == nullptr)
	{
		return false;
	}
	
	const TTuple<const FProperty*, const void*> Entry = GetData(DataKey);
	if (Entry.Key == nullptr || Entry.Value == nullptr)
	{
		return false;
	}

	if (!Entry.Key->SameType(Prop))
	{
		return false;
	}

	Prop->CopyCompleteValue(ValuePtr, Entry.Value);
	return true;
}

#pragma optimize("", off)
bool UMDGameDataSubsystem::SetDataFromPropertyWithExceptions(const FGameplayTag& DataKey, const FProperty* Prop,
	const void* ValuePtr, const UObject* ContextObject, FFrame& Stack)
{
	if (FMDGameDataEntry* Entry = DataEntries.Find(DataKey))
	{
		if (!Entry->EntryProperty->SameType(Prop))
		{
			// TODO - better type naming than just the property type (eg. element types of array, class type of objects, etc)
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				FText::Format(LOCTEXT("GetDataFailed", "Failed to SetData for key [{0}], caller is setting type [{1}] but key is of type [{2}]"),
					FText::FromName(DataKey.GetTagName()),
					Prop->GetClass()->GetDisplayNameText(),
					Entry->EntryProperty->GetClass()->GetDisplayNameText()
				)
			);
			FBlueprintCoreDelegates::ThrowScriptException(ContextObject, Stack, ExceptionInfo);
			return false;
		}

		Entry->EntryProperty->CopyCompleteValue(Entry->EntryValuePtr, ValuePtr);
		return true;
	}
	else
	{
		FMDGameDataEntry& NewEntry = DataEntries.Emplace(DataKey, FMDGameDataEntry(Prop));
		NewEntry.EntryProperty->CopyCompleteValue(NewEntry.EntryValuePtr, ValuePtr);
		return true;
	}
}
#pragma optimize("", on)

bool UMDGameDataSubsystem::GetDataFromPropertyWithExceptions(const FGameplayTag& DataKey, const FProperty* Prop,
	void* ValuePtr, const UObject* ContextObject, FFrame& Stack) const
{
	const TTuple<const FProperty*, const void*> Entry = GetData(DataKey);
	if (Entry.Key == nullptr || Entry.Value == nullptr)
	{
		// TODO - log that entry doesn't exist
		return false;
	}
	
	if (!Entry.Key->SameType(Prop))
	{
		// TODO - better type naming than just the property type (eg. element types of array, class type of objects, etc)
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			FText::Format(LOCTEXT("GetDataFailed", "Failed to GetData for key [{0}], caller expected type [{1}] but key is of type [{2}]"),
				FText::FromName(DataKey.GetTagName()),
				Prop->GetClass()->GetDisplayNameText(),
				Entry.Key->GetClass()->GetDisplayNameText()
			)
		);
		FBlueprintCoreDelegates::ThrowScriptException(ContextObject, Stack, ExceptionInfo);
		return false;
	}

	Prop->CopyCompleteValue(ValuePtr, Entry.Value);
	return true;
}

#undef LOCTEXT_NAMESPACE
