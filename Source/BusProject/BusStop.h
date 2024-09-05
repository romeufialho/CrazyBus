#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BusStop.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class BUSPROJECT_API ABusStop : public AActor
{
    GENERATED_BODY()

public:
    ABusStop();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void SetIsBusStopTargeted(bool bTargeted);
    void ResetToDefaultMaterial();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bus Stop")
    int32 MinNumberOfPassengers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bus Stop")
    int32 MaxNumberOfPassengers;

    UFUNCTION(BlueprintCallable, Category = "Bus Stop")
    int32 GetNumberOfPassengers() const { return NumberOfPassengers; }

    UFUNCTION(BlueprintCallable, Category = "Bus Stop")
    void RandomizeNumberOfPassengers();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* TriggerBox;

    UPROPERTY(EditDefaultsOnly, Category = "Materials")
    UMaterialInterface* DefaultMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Materials")
    UMaterialInterface* TargetedMaterial;

    UPROPERTY(Transient, VisibleInstanceOnly, Category = "Bus Stop")
    int32 NumberOfPassengers;

    bool IsBusStopTargeted;
};
