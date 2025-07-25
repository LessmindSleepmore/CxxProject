// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxPassenger.h"
#include "GameFramework/Actor.h"
#include "CxxStation.generated.h"

UCLASS()
class MINIGAME_API ACxxStation : public AActor
{
	GENERATED_BODY()

	bool isHighlighted = false;
	UPROPERTY(EditAnywhere) EStationType stationType;
	
public:	
	// Sets default values for this actor's properties
	ACxxStation();
	UPROPERTY(EditAnywhere) TArray<ACxxPassenger*> passengers;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddPassenger(ACxxPassenger* passenger);
	void RemovePassenger(ACxxPassenger* passenger);
	void SwitchOutlineEffect();
	EStationType GetStationType();
	void ShowPassengerStatus();
};
