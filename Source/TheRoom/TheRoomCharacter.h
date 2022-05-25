// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Datatable.h"
#include "Engine/World.h"
#include "TheRoomGameMode.h"
#include "Item.h"
#include "components/sphereComponent.h"
#include "DrawDebugHelpers.h"
#include "DoorRotator.h"
#include "TheRoomCharacter.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase
{
	GENERATED_BODY()
public:
	FInventoryItem()
	{
		name = FText::FromString("item");
		isVisible = false;

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName itemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AItem> item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isVisible;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* image;
	bool operator==(const FInventoryItem& Item)const
	{
		if (itemId == Item.itemId)
			return true;
		else
			return false;
	}




};

UCLASS(config=Game)
class ATheRoomCharacter : public ACharacter
{
	GENERATED_BODY()
		

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ATheRoomCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	UFUNCTION(BlueprintCallable, Category = "items")
		void collect();
	UFUNCTION()
		void InventoryPlus();
	UFUNCTION()
		void InventoryMinus();
	void raycast();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	UPROPERTY(EditAnywhere, category = "Animation")
		AActor* animDoor;


	UFUNCTION()
		void Wielding();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FInventoryItem> inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int inventoryI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = camera, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CollectionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FInventoryItem wield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FInventoryItem empty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FInventoryItem>pickupableObjects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wielding Items")
		TArray<AActor*> wieldObjects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool justremoved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool justpickedup;

	UFUNCTION(BlueprintCallable)
		bool getpickedup();

	UFUNCTION(BlueprintCallable)
		bool getremoved();

	FORCEINLINE class USphereComponent* GetCollectionRange() const { return CollectionRange; }
	UPROPERTY()
		TArray<int> inputedcode;
};

