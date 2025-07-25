// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxPlayerPawn.h"

// Sets default values
ACxxPlayerPawn::ACxxPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACxxPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACxxPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

