// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxPassenger.h"

// Sets default values
ACxxPassenger::ACxxPassenger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACxxPassenger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACxxPassenger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACxxPassenger::IsBoardingRequired(TArray<EStationType> path)
{
	for (auto stationType : path)
	{
		if (stationType == targetStationType)
		{
			return true;
		}
	}
	return false;
}

