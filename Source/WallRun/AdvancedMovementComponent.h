// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AdvancedMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WALLRUN_API UAdvancedMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAdvancedMovementComponent();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LandEvent();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void WallRunUpdateEvent();

	UFUNCTION(BlueprintCallable)
	void WallRunUpdateMovement();

	UFUNCTION(BlueprintCallable)
	void WallRunJump();

	UFUNCTION(BlueprintCallable)
	bool WallRunMovement(FVector Start,FVector End, float Direction);

	UFUNCTION(BlueprintCallable)
	void EndWallRun(float ResetTime);

	UFUNCTION(BlueprintCallable)
	void SuppressWallRun(float Delay);

	UFUNCTION(BlueprintCallable)
	void ResetWallRunSuppresion();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void InitializeInfo(ACharacter* CharacterRef);

	UFUNCTION(BlueprintCallable)
	void CameraTick();

	UFUNCTION(BlueprintCallable)
	void CameraTilt(float Roll);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bValidWallRun(FVector Vec);
	FVector PlayerToWallVector();
	FVector ToWallEndVector(bool bRightSide);
public:
	UPROPERTY(BlueprintReadOnly)
	ACharacter* Character;
	UPROPERTY(BlueprintReadOnly)
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(BlueprintReadOnly)
	float GravityScale;
	UPROPERTY(BlueprintReadOnly)
	FVector WallRunNormal;
	UPROPERTY(BlueprintReadOnly)
	float WallRunSpeed = 850;
	UPROPERTY(BlueprintReadOnly)
	bool WallRunning = false;
	UPROPERTY(BlueprintReadOnly)
	bool WallRunLeft = false;
	UPROPERTY(BlueprintReadOnly)
	bool WallRunRight = false;
	UPROPERTY(BlueprintReadOnly)
	bool WallRunSuppressed = false;
	UPROPERTY(BlueprintReadOnly)
	float WallRunTargetGravity = 0.05;
	FTimerHandle ResetWallRunHandler;
};
