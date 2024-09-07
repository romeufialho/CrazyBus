#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BusStop.h"
#include "UObject/ConstructorHelpers.h"
#include "GMTimeAttack.generated.h"

UCLASS()
class BUSPROJECT_API AGMTimeAttack : public AGameModeBase
{
    GENERATED_BODY()

public:
    AGMTimeAttack();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadWrite, Category = "Countdown")
    FText PregameGoText;

    UFUNCTION(BlueprintCallable, Category = "Default")
    void SpawnPlayer();

    UFUNCTION(BlueprintCallable, Category = "Default")
    void FindClosestBusStop();

    UFUNCTION(BlueprintCallable, Category = "Default")
    void SetNewTargetBusStop();

    UFUNCTION(BlueprintCallable, Category = "Default")
    void HandleBusStopPass(int32 NumberOfPassengers);

    UPROPERTY(BlueprintReadWrite, Category = "Timer")
    float CountdownTime;

    UFUNCTION(BlueprintCallable, Category = "Default")
    void StartCountdown(float Duration);

    UFUNCTION(BlueprintCallable, Category = "Default")
    void UpdateCountdown(float DeltaTime);

    UPROPERTY(BlueprintReadWrite, Category = "Default")
    int32 TotalPassengers;

    UFUNCTION(BlueprintCallable, Category = "Multiplier")
    FText GetCurrentMultiplier() const;

    UFUNCTION(BlueprintCallable, Category = "Multiplier")
    float GetMultiplierProgress() const;

    UFUNCTION(BlueprintCallable, Category = "TimeAttack")
    float GetProgressBarValue() const;

    void StartPreGameCountdown();

    void UpdatePreGameCountdown(float DeltaTime);

    void OnPreGameCountdownCompleted();

    void SetScore();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Score")
    int32 ScoreMultiplier;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<APawn> CarPawnClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
    TSubclassOf<AActor> BusStopClass;

    UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Default")
    TArray<AActor*> SpawnLocations;

    UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Default")
    TArray<AActor*> BusStops;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
    AActor* TargetedBusStop;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Countdown")
    float StartCountdownDuration;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
    float MinimumDistance = 5000.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplier")
    float CurrentMultiplier;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplier")
    float StreakResetTime;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Countdown")
    bool bIsPreGameCountdownActive;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Countdown")
    bool bIsGameActive;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Countdown")
    float PreGameCountdownTime;

    FTimerHandle GoTextTimerHandle;
    void ClearGoText();

private:
    float TimeSinceLastBusStop;

    void UpdateMultiplier();

    void ResetMultiplier();

    void AddTimeToTimerWithMultiplier(int32 BaseTimeToAdd);

    void UpdateTotalPassengers(int32 NewPassengers);

    int32 GetTotalPassengers() const;

    UPROPERTY(EditDefaultsOnly, Category = "Sounds")
    USoundBase* Countdown3Sound;

    UPROPERTY(EditDefaultsOnly, Category = "Sounds")
    USoundBase* Countdown2Sound;

    UPROPERTY(EditDefaultsOnly, Category = "Sounds")
    USoundBase* Countdown1Sound;

    UPROPERTY(EditDefaultsOnly, Category = "Sounds")
    USoundBase* GoSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    UAudioComponent* Countdown3AudioComponent;

    UPROPERTY(EditAnywhere, Category = "Audio")
    UAudioComponent* Countdown2AudioComponent;

    UPROPERTY(EditAnywhere, Category = "Audio")
    UAudioComponent* Countdown1AudioComponent;

    UPROPERTY(EditAnywhere, Category = "Audio")
    UAudioComponent* GoAudioComponent;

    void PlayCountdownSound(int32 CountdownValue);
};