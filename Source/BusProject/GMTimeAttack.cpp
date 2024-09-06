#include "GMTimeAttack.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

AGMTimeAttack::AGMTimeAttack()
{
    PrimaryActorTick.bCanEverTick = true;
    TimeSinceLastBusStop = 0.0f;
    PregameGoText = FText::FromString(TEXT(""));
}

void AGMTimeAttack::BeginPlay()
{
    Super::BeginPlay();
    CountdownTime = StartCountdownDuration;
    UWorld* World = GetWorld();
    if (!World)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("World is null in BeginPlay!"));
        return;
    }

    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), SpawnLocations);

    if (BusStopClass)
    {
        UGameplayStatics::GetAllActorsOfClass(World, BusStopClass, BusStops);

        if (BusStops.Num() > 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Found %d BusStop actors"), BusStops.Num()));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No BusStop actors found in the level!"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to load BusStop class!"));
    }

    SpawnPlayer();

    FindClosestBusStop();

    StartPreGameCountdown();
}

void AGMTimeAttack::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdatePreGameCountdown(DeltaTime);
    if (bIsGameActive)
    {
        UpdateCountdown(DeltaTime);
        TimeSinceLastBusStop += DeltaTime;

        if (TimeSinceLastBusStop > StreakResetTime)
        {
            ResetMultiplier();
        }
    }
}

void AGMTimeAttack::StartPreGameCountdown()
{
    PreGameCountdownTime = 3.0f;
    bIsPreGameCountdownActive = true;
}

void AGMTimeAttack::UpdatePreGameCountdown(float DeltaTime)
{
    if (bIsPreGameCountdownActive)
    {
        PreGameCountdownTime -= DeltaTime;

        if (PreGameCountdownTime <= 0.0f)
        {
            PreGameCountdownTime = 0.0f;
            bIsPreGameCountdownActive = false;
            bIsGameActive = true;

            PregameGoText = FText::FromString(TEXT("GO!"));
            OnPreGameCountdownCompleted();

            CountdownTime = StartCountdownDuration;
            StartCountdown(StartCountdownDuration);

            GetWorld()->GetTimerManager().SetTimer(GoTextTimerHandle, this, &AGMTimeAttack::ClearGoText, 1.0f, false);
        }
    }
}

void AGMTimeAttack::ClearGoText()
{
    PregameGoText = FText::FromString(TEXT(""));
}


void AGMTimeAttack::OnPreGameCountdownCompleted()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("GO!"));
}

void AGMTimeAttack::StartCountdown(float Duration)
{
    CountdownTime = Duration;
}

void AGMTimeAttack::UpdateCountdown(float DeltaTime)
{
    if (CountdownTime > 0)
    {
        CountdownTime -= DeltaTime;

        if (CountdownTime <= 0)
        {
            CountdownTime = 0;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Time is up!"));
            GetFinalScore();
        }
    }
}

void AGMTimeAttack::AddTimeToTimerWithMultiplier(int32 BaseTimeToAdd)
{
    int32 TimeToAdd = BaseTimeToAdd * CurrentMultiplier;
    CountdownTime += TimeToAdd;
    if (CountdownTime > StartCountdownDuration)
    {
        CountdownTime = StartCountdownDuration;
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Time added: %d, Multiplier: %f, Total: %f"), TimeToAdd, CurrentMultiplier, CountdownTime));
}

void AGMTimeAttack::HandleBusStopPass(int32 NumberOfPassengers)
{
    UpdateMultiplier();

    AddTimeToTimerWithMultiplier(NumberOfPassengers);

    UpdateTotalPassengers(NumberOfPassengers);

    TimeSinceLastBusStop = 0.0f;
}

void AGMTimeAttack::ResetMultiplier()
{
    CurrentMultiplier = 0.5f;
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Multiplier reset to 0.5"));
}

void AGMTimeAttack::UpdateMultiplier()
{
    if (CurrentMultiplier < 5.0f)
    {
        CurrentMultiplier += 0.5f;
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Current Multiplier: %f"), CurrentMultiplier));
}

void AGMTimeAttack::SpawnPlayer()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("World is null!"));
        return;
    }

    if (SpawnLocations.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No PlayerStart actors found in the level!"));
        return;
    }

    int32 RandomIndex = FMath::RandRange(0, SpawnLocations.Num() - 1);
    APlayerStart* SelectedPlayerStart = Cast<APlayerStart>(SpawnLocations[RandomIndex]);
    if (!SelectedPlayerStart)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to cast to APlayerStart!"));
        return;
    }

    FTransform SpawnTransform = SelectedPlayerStart->GetActorTransform();
    FString SpawnPointName = SelectedPlayerStart->GetActorLabel();
    FVector SpawnLocation = SpawnTransform.GetLocation();
    FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();

    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Spawning car at PlayerStart: %s, location: %s, rotation: %s"), *SpawnPointName, *SpawnLocation.ToString(), *SpawnRotation.ToString()));

    if (CarPawnClass)
    {
        APawn* NewPawn = World->SpawnActor<APawn>(CarPawnClass, SpawnTransform);
        if (NewPawn)
        {
            APlayerController* PC = World->GetFirstPlayerController();
            if (PC)
            {
                PC->Possess(NewPawn);
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to spawn the car!"));
        }
    }
}

void AGMTimeAttack::FindClosestBusStop()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("World is null in FindClosestBusStop!"));
        return;
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!PlayerController)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No PlayerController found!"));
        return;
    }

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerPawn is null!"));
        return;
    }

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    float ClosestDistance = FLT_MAX;
    AActor* ClosestBusStop = nullptr;

    for (AActor* BusStop : BusStops)
    {
        if (BusStop)
        {
            float Distance = FVector::Dist(PlayerLocation, BusStop->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestBusStop = BusStop;
            }
        }
    }

    if (ClosestBusStop)
    {
        TargetedBusStop = ClosestBusStop;
        /*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Closest BusStop found at location: %s, distance: %f"),
            *ClosestBusStop->GetActorLocation().ToString(), ClosestDistance));*/

        ABusStop* BusStopActor = Cast<ABusStop>(TargetedBusStop);
        if (BusStopActor)
        {
            BusStopActor->SetIsBusStopTargeted(true);
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No BusStop found!"));
    }
}

void AGMTimeAttack::SetNewTargetBusStop()
{
    if (BusStops.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No BusStop actors available to target!"));
        return;
    }

    if (TargetedBusStop)
    {
        ABusStop* PreviousBusStopActor = Cast<ABusStop>(TargetedBusStop);
        if (PreviousBusStopActor)
        {
            PreviousBusStopActor->ResetToDefaultMaterial();
            PreviousBusStopActor->SetIsBusStopTargeted(false);
        }
    }

    TArray<AActor*> ValidBusStops = BusStops;
    if (TargetedBusStop)
    {
        ValidBusStops.Remove(TargetedBusStop);
    }

    if (ValidBusStops.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No valid BusStop actors available to target!"));
        return;
    }

    int32 RandomIndex = FMath::RandRange(0, ValidBusStops.Num() - 1);
    AActor* NewTargetBusStop = ValidBusStops[RandomIndex];

    if (NewTargetBusStop)
    {
        TargetedBusStop = NewTargetBusStop;

        ABusStop* BusStopActor = Cast<ABusStop>(TargetedBusStop);
        if (BusStopActor)
        {
            BusStopActor->SetIsBusStopTargeted(true);
        }

        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("New target BusStop set!"));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to set new target BusStop!"));
    }
}


void AGMTimeAttack::UpdateTotalPassengers(int32 NewPassengers)
{
    TotalPassengers += NewPassengers;
}

int32 AGMTimeAttack::GetTotalPassengers() const
{
    return TotalPassengers;
}
FText AGMTimeAttack::GetCurrentMultiplier() const
{
    return FText::FromString(FString::Printf(TEXT("%.1f x"), CurrentMultiplier));
}

float AGMTimeAttack::GetMultiplierProgress() const
{
    return (CurrentMultiplier - 0.5f) / (5.0f - 0.5f);
}

float AGMTimeAttack::GetProgressBarValue() const
{
    const float MinMultiplier = 0.5f;

    if (CurrentMultiplier <= MinMultiplier)
    {
        return 1.0f;
    }

    if (StreakResetTime <= 0.0f)
    {
        return 0.0f;
    }

    float Progress = TimeSinceLastBusStop / StreakResetTime;

    return FMath::Clamp(Progress, 0.0f, 1.0f);
}


float AGMTimeAttack::GetFinalScore() const
{
    float FinalScore = TotalPassengers * TotalPassengersMult;

    // Log final score for debugging
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Final Score: %.2f"), FinalScore));

    return FinalScore;
}