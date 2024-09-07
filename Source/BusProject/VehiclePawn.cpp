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
	if (UChaosWheeledVehicleMovementComponent* Vehicle = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		const FVector TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		const FVector TraceEnd = GetActorLocation() - FVector(0.f, 0.f, 200.f);

		FHitResult Hit;

		// check if car is flipped on its side, and check if the car is in air
		const bool bInAir = !GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		const bool bNotGrounded = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < 0.1f;

		// only allow in air-movement if we are not on the ground, or are in the air
		if (bInAir || bNotGrounded)
		{
			const float ForwardInput = InputComponent->GetAxisValue(NAME_ThrottleInput);
			const float RightInput = InputComponent->GetAxisValue(NAME_SteerInput);

			// car is grounded -- allow player to roll the car over
			const float AirMovementForcePitch = 3.f;
			const float AirMovementForceRoll = !bInAir && bNotGrounded ? 20.f : 3.f; // this is because a lot more force is required to roll the car if it is grounded,
																				     // compared to when it in mid air
			
			// Movement Vector - Describes the direction or angle in which the vehicle should stop moving
			if (UPrimitiveComponent* VehicleMesh = Vehicle->UpdatedPrimitive)
			{
				const FVector MovementVector = FVector(RightInput * -AirMovementForceRoll, ForwardInput * AirMovementForcePitch, 0.f) * DeltaTime * 200.f;
				// rotate this vector by our angular rotation
				const FVector NewAngularMovement = GetActorRotation().RotateVector(MovementVector);

				VehicleMesh->SetPhysicsAngularVelocityInDegrees(NewAngularMovement, true);
			}
		}
	}
}
