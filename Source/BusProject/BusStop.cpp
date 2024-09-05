#include "BusStop.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "GMTimeAttack.h"
#include "Materials/MaterialInterface.h"

ABusStop::ABusStop()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = Root;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABusStop::OnTriggerBoxOverlapBegin);

    IsBusStopTargeted = false;
    DefaultMaterial = nullptr;
    TargetedMaterial = nullptr;
}

void ABusStop::BeginPlay()
{
    Super::BeginPlay();

    TArray<UStaticMeshComponent*> FlagComponents;
    GetComponents<UStaticMeshComponent>(FlagComponents);

    for (UStaticMeshComponent* Component : FlagComponents)
    {
        if (Component->ComponentHasTag(TEXT("Flag")))
        {
            if (Component && DefaultMaterial)
            {
                Component->SetMaterial(0, DefaultMaterial);
            }
        }
    }
}

void ABusStop::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABusStop::OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        if (IsBusStopTargeted)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Valid: Player passed through the targeted bus stop!"));

            AGMTimeAttack* GameMode = Cast<AGMTimeAttack>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->SetNewTargetBusStop();
            }

            ResetToDefaultMaterial();
            SetIsBusStopTargeted(false);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid: Player passed through a non-targeted bus stop!"));
        }
    }
}

void ABusStop::SetIsBusStopTargeted(bool bTargeted)
{
    IsBusStopTargeted = bTargeted;

    TArray<UStaticMeshComponent*> FlagComponents;
    GetComponents<UStaticMeshComponent>(FlagComponents);

    for (UStaticMeshComponent* Component : FlagComponents)
    {
        if (Component->ComponentHasTag(TEXT("Flag")))
        {
            if (Component)
            {
                if (IsBusStopTargeted && TargetedMaterial)
                {
                    Component->SetMaterial(0, TargetedMaterial);
                }
                else if (!IsBusStopTargeted && DefaultMaterial)
                {
                    Component->SetMaterial(0, DefaultMaterial);
                }
            }
        }
    }
}

void ABusStop::ResetToDefaultMaterial()
{
    TArray<UStaticMeshComponent*> FlagComponents;
    GetComponents<UStaticMeshComponent>(FlagComponents);

    for (UStaticMeshComponent* Component : FlagComponents)
    {
        if (Component->ComponentHasTag(TEXT("Flag")))
        {
            if (Component && DefaultMaterial)
            {
                Component->SetMaterial(0, DefaultMaterial);
            }
        }
    }
}