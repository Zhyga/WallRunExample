// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAdvancedMovementComponent::UAdvancedMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAdvancedMovementComponent::WallRunJump()
{
	if(WallRunning)
	{
		EndWallRun(0.35);
		float WallRunJumpHigh = 400;
		float WallRunSideJumpX = WallRunNormal.X * 300;
		float WallRunSideJumpY = WallRunNormal.Y * 300;
		FVector LaunchVelocity(WallRunSideJumpX,WallRunSideJumpY,WallRunJumpHigh);
		Character->LaunchCharacter(LaunchVelocity,false,true);
	}
}

bool UAdvancedMovementComponent::WallRunMovement(FVector Start, FVector End, float Direction)
{
	FHitResult HitResult;
    GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECollisionChannel::ECC_Visibility);
	if(!HitResult.bBlockingHit)
	{
		return false;
	}
	else{
		if(!CharacterMovement->IsFalling() || !bValidWallRun(HitResult.Normal))
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Invalid NOrmal")));
			return false;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("In WallRunMovement")));
			WallRunNormal = HitResult.Normal;
			Character->LaunchCharacter(PlayerToWallVector(),false,false);
			FVector WallRunDirection;
			WallRunDirection = UKismetMathLibrary::Cross_VectorVector(WallRunNormal,FVector(0,0,1));
			WallRunDirection *= Direction * WallRunSpeed;
			Character->LaunchCharacter(WallRunDirection,true,true);
			return true;
		}
	}
}

void UAdvancedMovementComponent::EndWallRun(float ResetTime)
{
	if(WallRunning)
	{
		WallRunning = false;
		WallRunLeft = false;
		WallRunRight = false;
		CharacterMovement->GravityScale = GravityScale;
	}
}

void UAdvancedMovementComponent::SuppressWallRun(float Delay)
{
	WallRunSuppressed = true;
	GetWorld()->GetTimerManager().SetTimer(ResetWallRunHandler,this,&UAdvancedMovementComponent::ResetWallRunSuppresion,Delay,false);
}

void UAdvancedMovementComponent::ResetWallRunSuppresion()
{
	GetWorld()->GetTimerManager().ClearTimer(ResetWallRunHandler);
	WallRunSuppressed = false;
}

void UAdvancedMovementComponent::LandEvent_Implementation()
{
	EndWallRun(0);
	WallRunSuppressed = false;
}

void UAdvancedMovementComponent::WallRunUpdateEvent_Implementation()
{
	if(!WallRunSuppressed)
	{
		WallRunUpdateMovement();
	}
}

void UAdvancedMovementComponent::WallRunUpdateMovement()
{
	if(WallRunMovement(Character->GetActorLocation(),ToWallEndVector(true),-1))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("In  WallRunUpdateMovement")));
		WallRunning = true;
		WallRunRight = true;
		WallRunLeft = false;
		float GravityScaleTemp;
		GravityScaleTemp = UKismetMathLibrary::FInterpTo(CharacterMovement->GravityScale,WallRunTargetGravity,GetWorld()->GetDeltaSeconds(),50);
		CharacterMovement->GravityScale = GravityScaleTemp;
	}
	else
	{
		if(WallRunRight)
		{
			EndWallRun(1);
		}
		else
		{
			if(WallRunMovement(Character->GetActorLocation(),ToWallEndVector(false),1))
			{
				WallRunning = true;
				WallRunRight = false;
				WallRunLeft = true;
				float GravityScaleTemp;
				GravityScaleTemp = UKismetMathLibrary::FInterpTo(CharacterMovement->GravityScale,WallRunTargetGravity,GetWorld()->GetDeltaSeconds(),50);
				CharacterMovement->GravityScale = GravityScaleTemp;
			}
			else
			{
				EndWallRun(1);
			}
		}
	}
}


void UAdvancedMovementComponent::InitializeInfo_Implementation(ACharacter* CharacterRef)
{
	Character = CharacterRef;
	CharacterMovement = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	GravityScale = CharacterMovement->GravityScale;
	FTimerHandle TempHandle;
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Initialize Component")));
	GetWorld()->GetTimerManager().SetTimer(TempHandle,this,&UAdvancedMovementComponent::WallRunUpdateEvent_Implementation,0.2,true);
}

void UAdvancedMovementComponent::CameraTick()
{
	if(WallRunLeft)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Camera Tilt Left")));
		CameraTilt(15);
	}
	if(WallRunRight)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Camera Tilt Right")));
		CameraTilt(-15);
	}
	else
	{
		CameraTilt(0);
	}
	
}

void UAdvancedMovementComponent::CameraTilt(float Roll)
{
	if(IsValid(Character->GetController()))
	{
		FRotator CharRotation = Character->GetController()->GetControlRotation();
		FRotator TargetRotation(CharRotation.Pitch,CharRotation.Yaw,Roll);
		FRotator NewRotation = UKismetMathLibrary::RInterpTo(CharRotation,TargetRotation,GetWorld()->GetDeltaSeconds(),10);
		Character->GetController()->SetControlRotation(NewRotation);
	}
}

// Called when the game starts
void UAdvancedMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAdvancedMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CameraTick();
	// ...
}

bool UAdvancedMovementComponent::bValidWallRun(FVector Vec)
{
	if(Vec.Z > -0,52 && Vec.Z < 0,52)
		return true;
	return false;
}

FVector UAdvancedMovementComponent::PlayerToWallVector()
{
	FVector ReturnVector;
	ReturnVector = WallRunNormal - Character->GetActorLocation();
	ReturnVector =  WallRunNormal * ReturnVector.Size();
	return ReturnVector;
}

FVector UAdvancedMovementComponent::ToWallEndVector(bool bRightSide)
{
	FVector WallEndVector;
	if(bRightSide)
	{
		WallEndVector = Character->GetActorLocation();
		WallEndVector += Character->GetActorRightVector() * 75.;
		WallEndVector += Character->GetActorForwardVector() * -35.;
	}
	else
	{
		WallEndVector = Character->GetActorLocation();
		WallEndVector += Character->GetActorRightVector() * -75.;
		WallEndVector += Character->GetActorForwardVector() * -35.;
	}
	return WallEndVector;
}

