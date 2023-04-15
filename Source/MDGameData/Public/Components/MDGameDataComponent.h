#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MDGameDataComponent.generated.h"


class UMDGameDataContainer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDGAMEDATA_API UMDGameDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game Data")
	UMDGameDataContainer* GetGameDataContainer() const { return GameDataContainer; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Game Data")
	int32 InitialGameDataReserveSize = 8;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMDGameDataContainer> GameDataContainer = nullptr;
	
};
