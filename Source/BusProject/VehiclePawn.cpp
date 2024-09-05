// Fill out your copyright notice in the Description page of Project Settings.


#include "VehiclePawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"

// Define Steer and Throttle input names
// Variables usable in multiple places. Avoids mistyping
static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

AVehiclePawn::AVehiclePawn()
{
	UChaosWheeledVehicleMovementComponent* Vehicle = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	// ALL OF THE FOLLOWING SETTINGS CAN BE MODIFIED IN THE EDITOR
	
	// Tire load
	//
	//
	
	// Torque
	Vehicle->SetMaxEngineTorque(5700.0f);
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Steering curve - steering amount at given speed
	//
	//

	// Slip differential
	//
	//

	// Automatic gearbox
	Vehicle->TransmissionSetup.bUseAutomaticGears = true;
	// gear switch time
	// gear auto box latency

	// COMPONENTS

	// Create SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Springarm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.0f;
}

void AVehiclePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateInAirControl(DeltaSeconds);
}

void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &AVehiclePawn::ApplyThrottle);
	PlayerInputComponent->BindAxis(NAME_SteerInput, this, &AVehiclePawn::ApplySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &AVehiclePawn::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AVehiclePawn::Turn);
}

void AVehiclePawn::ApplyThrottle(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AVehiclePawn::ApplySteering(float Val)
{
	GetVehicleMovement()->SetSteeringInput(Val);
}

void AVehiclePawn::LookUp(float Val)
{
	if (Val != 0.f)
	{
		AddControllerPitchInput(Val);
	}
}

void AVehiclePawn::Turn(float Val)
{
	if (Val != 0.f)
	{
		AddControllerYawInput(Val);
	}
}

void AVehiclePawn::UpdateInAirControl(float DeltaTime)
{
}
