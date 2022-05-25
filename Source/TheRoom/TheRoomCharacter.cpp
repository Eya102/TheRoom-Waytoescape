// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheRoomCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATheRoomCharacter

ATheRoomCharacter::ATheRoomCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	inventoryI = 0;
	empty.itemId = "0";
	CollectionRange = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionRange"));
	CollectionRange->AttachTo(RootComponent);
	CollectionRange->SetSphereRadius(100.0f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATheRoomCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheRoomCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheRoomCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheRoomCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheRoomCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATheRoomCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATheRoomCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATheRoomCharacter::OnResetVR);
}


void ATheRoomCharacter::OnResetVR()
{
	// If TheRoom is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TheRoom.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATheRoomCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ATheRoomCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ATheRoomCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheRoomCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheRoomCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATheRoomCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void ATheRoomCharacter::collect()
{
	TArray<AActor*> CollectedItems;
	
	for (int i = 0; i < CollectedItems.Num(); i++)
	{
		AItem* const testItem = Cast<AItem>(CollectedItems[i]);
		if (testItem && testItem->GetActive())
		{
			testItem->Touched();
		
			testItem->SetActive(false);
		}
	}
}
void ATheRoomCharacter::InventoryPlus()
{
	inventoryI += 1;
	if (inventoryI >= 5)
	{
		inventoryI = 0;
	}
	Wielding();
}
void ATheRoomCharacter::InventoryMinus()
{
	inventoryI -= 1;
	if (inventoryI < 0)
	{
		inventoryI = 4;
	}
	Wielding();
}


void ATheRoomCharacter::Wielding()
{
	if (inventory.Num() > inventoryI)
	{
		wield = inventory[inventoryI];

		for (int i = 0; i < pickupableObjects.Num(); i++)
		{
			if (wield.itemId == pickupableObjects[i].itemId)
			{
				if (wieldObjects[i])
				{
					wieldObjects[i]->SetActorHiddenInGame(false);
				}
				else
				{
					if (wieldObjects[i])
					{
						wieldObjects[i]->SetActorHiddenInGame(true);
					}
				}
			}
		}

	}
	else
	{
		wield = empty;
	}
	if (wield == empty)
	{
		for (int i = 0; i < pickupableObjects.Num(); i++)
		{
			wieldObjects[i]->SetActorHiddenInGame(true);
		}
	}
}


bool ATheRoomCharacter::getpickedup()
{
	return justpickedup;
}

bool ATheRoomCharacter::getremoved()
{
	return justremoved;
}
void ATheRoomCharacter::raycast() {
	FHitResult* hit = new FHitResult();
	FVector forward = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector();
	FVector start = GetActorLocation();
	start.Z = 300.f;
	FVector end = (forward * 3000.f) + start;
	FCollisionQueryParams* col = new FCollisionQueryParams();
	col->AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(*hit, start, end, ECC_WorldStatic, *col)) {
		DrawDebugLine(GetWorld(), start, end, FColor::Red, true);
		if (hit->GetActor() != NULL) {
			UE_LOG(LogClass, Log, TEXT("compoent:%s"), +*hit->GetComponent()->GetName());
			if (*hit->GetComponent()->GetName() == FString("1")) {
				inputedcode.Add(1);
			}
			else if (*hit->GetComponent()->GetName() == FString("2")) {
				inputedcode.Add(2);
			}
			else if (*hit->GetComponent()->GetName() == FString("3")) {
				inputedcode.Add(3);
			}
			if (inputedcode.Num() == 3) {
				bool correct = false;
				for (int i = 0; i < inputedcode.Num(); i++) {
					if (inputedcode[i] == i + 1) {
						correct = true;
						UE_LOG(LogClass, Log, TEXT("correct"));
					}
					else {
						correct = false;
						UE_LOG(LogClass, Log, TEXT("correct"));
						break;
					}
				}
				inputedcode.Empty();
				
				
			}
		}
	}
};





