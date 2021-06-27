// Fill out your copyright notice in the Description page of Project Settings.


#include "CrazyCar.h"

#include <filesystem>

#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACrazyCar::ACrazyCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called to bind functionality to input
void ACrazyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACrazyCar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACrazyCar::MoveRight);

}

void ACrazyCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ACrazyCar, ReplicatedTransform);
	DOREPLIFETIME(ACrazyCar, Velocity);
	DOREPLIFETIME(ACrazyCar, Throttle);
	DOREPLIFETIME(ACrazyCar, SteeringThrow);
	DOREPLIFETIME(ACrazyCar, bEnableCarControl);
}

// Called when the game starts or when spawned
void ACrazyCar::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACrazyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bEnableCarControl)
	{
		FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();

		Force += GetAirResistence();
		Force += GetRollingResistence();
	
		FVector Acceleration = Force / Mass;

		float GravityForce = GetWorld()->GetGravityZ();

		Velocity += Acceleration * DeltaTime;
	
		UpdateRotation(DeltaTime);
		UpdateLocation(DeltaTime);
	}
	if (HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();
	}
}

FVector ACrazyCar::GetAirResistence()
{
	return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector ACrazyCar::GetRollingResistence()
{
	float Acceleration = - GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * Acceleration;
	return - Velocity.GetSafeNormal() * RollingResistenceCoefficient * NormalForce;
}

void ACrazyCar::UpdateLocation(float DeltaTime)
{
	const FVector Translation = Velocity * DeltaTime * 100;
	FHitResult Hit;
	AddActorWorldOffset(Translation, true, &Hit);
	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

void ACrazyCar::UpdateRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotaionAngle = DeltaLocation / MinTurnCircleRadius * SteeringThrow; 
	FQuat RotationDelta(GetActorUpVector(), RotaionAngle);
	Velocity = RotationDelta.RotateVector(Velocity);
	AddActorWorldRotation(RotationDelta);
}

void ACrazyCar::OnRep_ReplicatedTransform()
{
	SetActorTransform(ReplicatedTransform);
}

void ACrazyCar::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool ACrazyCar::Server_MoveForward_Validate(float Value)
{
	return true;
}

void ACrazyCar::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void ACrazyCar::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool ACrazyCar::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}

void ACrazyCar::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}