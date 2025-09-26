// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "WallRunTestCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AWallRunTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Character movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovementComponent;

	/** Collider for wall detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* WallDetectionCapsuleLeft;

	/** Collider for wall detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* WallDetectionCapsuleRight;

	/** Tag for walls that can be wallrun on */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	FName wallRunTag;

	/** Boolean for if actor is wallrunning */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	bool isWallRunning;

	/** Vector for wallrunning direction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	FVector wallRunningDirection;

	/** Default gravity scale */
	float defaultGravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallRunning", meta = (AllowPrivateAccess = "true"))
	float wallJumpForce = 1300.f;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	void PerformWallJump();

	
public:
	AWallRunTestCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	// Overlap function
	UFUNCTION()
	void OnWallCapsuleLeftBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnWallCapsuleRightBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// End overlap function
	UFUNCTION()
	void OnWallCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);


	// Check to see if wallrun can be initiated
	UFUNCTION()
	bool CanWallRun(AActor* Wall);

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Tick override */
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual double DotProductWithCamera(FVector otherVector);

	UFUNCTION()
	virtual void  StartWallRun(float wallRunDir, float cameraRotation);

	UFUNCTION()
	virtual void  EndWallRun();

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

