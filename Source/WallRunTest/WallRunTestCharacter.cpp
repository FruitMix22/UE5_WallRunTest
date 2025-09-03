// Copyright Epic Games, Inc. All Rights Reserved.

#include "WallRunTestCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WallRunTest.h"

AWallRunTestCharacter::AWallRunTestCharacter()
{


	// Set the tag for walls that the actor can wallrun on
	wallRunTag = "Can_Wallrun_On";

	// Set default state for wallrunning to false
	isWallRunning = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->BrakingDecelerationFalling = 1500.0f;
	CharacterMovementComponent->AirControl = 0.5f;
	CharacterMovementComponent->SetPlaneConstraintEnabled(true);
	defaultGravityScale = CharacterMovementComponent->GravityScale;

}

void AWallRunTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AWallRunTestCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWallRunTestCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWallRunTestCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWallRunTestCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AWallRunTestCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogWallRunTest, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AWallRunTestCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AWallRunTestCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AWallRunTestCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AWallRunTestCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AWallRunTestCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AWallRunTestCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AWallRunTestCharacter::OnWallCapsuleBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (!OtherActor->ActorHasTag(wallRunTag)) return;

	if (!CharacterMovementComponent->IsFalling()) return;
	

	// First, check if vectors are facing each other
	// If the wall normal and camera face eachover, the value will be 1!
	if (DotProductWithCamera(OtherActor) > 0)
	{
		wallRunningDirection = OtherActor->GetActorForwardVector() * 1;
		UE_LOG(LogTemp, Warning, TEXT("Facing the wall with dot product of:  %f."), DotProductWithCamera(OtherActor));
	}
	else if (DotProductWithCamera(OtherActor) < 0)
	{
		wallRunningDirection = OtherActor->GetActorForwardVector() * -1;
		UE_LOG(LogTemp, Warning, TEXT("Facing the wall with dot product of:  %f."), DotProductWithCamera(OtherActor));
	}



	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + wallRunningDirection * 300.f,
		50.f,
		FColor::Green,
		false,
		5.f,
		0,
		2.f
	);

	// Start wallrunning
	StartWallRun();
}

void AWallRunTestCharacter::OnWallCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	EndWallRun();
}

void AWallRunTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find the capsule from the Blueprint hierarchy
	WallDetectionCapsule = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("DetectWallCollider")));

	if (WallDetectionCapsule)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallDetectionCapsule found! Binding overlap."));
		WallDetectionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AWallRunTestCharacter::OnWallCapsuleBeginOverlap);
		WallDetectionCapsule->OnComponentEndOverlap.AddDynamic(this, &AWallRunTestCharacter::OnWallCapsuleEndOverlap);
	}
}

void AWallRunTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isWallRunning)
	{
		CharacterMovementComponent->Velocity = wallRunningDirection * 1000;
	}
}

double AWallRunTestCharacter::DotProductWithCamera(AActor* OtherActor)
{
	FVector otherForwardVector = OtherActor->GetActorForwardVector();
	FVector cameraForwardVector = FirstPersonCameraComponent->GetForwardVector();

	return FVector::DotProduct(otherForwardVector, cameraForwardVector);
}

void AWallRunTestCharacter::StartWallRun()
{
	isWallRunning = true;
	CharacterMovementComponent->GravityScale = 0.0f;
}

void AWallRunTestCharacter::EndWallRun()
{
	isWallRunning = false;
	CharacterMovementComponent->GravityScale = defaultGravityScale;
}
