#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BusGameInstance.generated.h"


UCLASS()
class BUSPROJECT_API UBusGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UBusGameInstance();

    UPROPERTY(BlueprintReadWrite, Category = "GameData")
    int32 TotalPassengers;

    UPROPERTY(BlueprintReadWrite, Category = "GameData")
    float ScoreMultiplier;

    UPROPERTY(BlueprintReadWrite, Category = "GameData")
    int32 Score;
};