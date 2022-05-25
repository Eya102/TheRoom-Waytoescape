// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorRotator.h"

// Sets default values
ADoorRotator::ADoorRotator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("mesh"));

}

// Called when the game starts or when spawned
void ADoorRotator::BeginPlay()
{
	Super::BeginPlay();
    if (animation)
    {
        FOnTimelineFloat rotationFunction;

        rotationFunction.BindUFunction(this, FName("updateRotation"));
        timeline.AddInterpFloat(animation, rotationFunction);
        timeline.SetLooping(false);

        initialRot = targetRot = GetActorRotation();

        targetRot.Yaw += angle;

    }
	
}

// Called every frame
void ADoorRotator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    timeline.TickTimeline(DeltaTime);

}

void ADoorRotator::UpdateRotation(float value)
{
    FRotator tempRot = FMath::Lerp(initialRot, targetRot, value);
    SetActorRotation(tempRot);
}

void ADoorRotator::CallAnimation()
{
    if (!timeline.IsPlaying())
    {
        timeline.PlayFromStart();
    }
}

