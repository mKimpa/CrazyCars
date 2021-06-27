// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CrazyCar.generated.h"

UCLASS()
class CRAZYCARS_API ACrazyCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACrazyCar();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	FVector GetAirResistence();

	FVector GetRollingResistence();

	// Minimum turning radius (m)
	UPROPERTY(EditAnywhere)
	float MinTurnCircleRadius = 10;
	
	// The mass of the car (kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// (Kg/m)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistenceCoefficient = 0.1;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	void MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

	void MoveRight(float Value);

	void UpdateLocation(float DeltaTime);
	void UpdateRotation(float DeltaTime);

	UPROPERTY(Replicated)
	FVector Velocity;

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;

	UFUNCTION()
	void OnRep_ReplicatedTransform();

	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;

	UPROPERTY(Replicated)
	bool bEnableCarControl = true;

};
