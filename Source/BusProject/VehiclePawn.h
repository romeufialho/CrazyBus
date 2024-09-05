// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "VehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class BUSPROJECT_API AVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AVehiclePawn();

	// tick will be used to control the vehicle if its in mid-air or tipped over
	virtual void Tick(float DeltaSeconds) override;

	// detect key inputs
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// throttle and steering values
	void ApplyThrottle(float Val);
	void ApplySteering(float Val);

	// looking aroud
	void LookUp(float Val); // pitch camera
	void Turn(float Val); // yaw camera

	// enable in air controls
	void UpdateInAirControl(float DeltaTime);

protected:

	// COMPONENTS
	
	// camera spring arm
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
};
