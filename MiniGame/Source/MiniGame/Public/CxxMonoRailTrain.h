// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxDispatchManager.h"
#include "CxxRailLine.h"
#include "GameFramework/Actor.h"
#include "CxxMonoRailTrain.generated.h"

UCLASS()
class MINIGAME_API ACxxMonoRailTrain : public AActor
{
	GENERATED_BODY()

	UPROPERTY() ACxxDispatchManager* dispatchManager;
	UPROPERTY() ACxxRailLine* railLine;
	UPROPERTY() int curSegmentIdx = 0;
	UPROPERTY() int curPointIdx = 0;
	UPROPERTY() float curBlockMovedDist = 0.;
	UPROPERTY() float speed = 100.;
	UPROPERTY() bool bCurIsForward = true;
	UPROPERTY() float offSetRail = 20;
	UPROPERTY() int capacity = 10;
	UPROPERTY() TArray<ACxxPassenger*> passengers;
	UPROPERTY() bool bInStation = true;
	UPROPERTY() float inStationTime = 0;
	// UPROPERTY() UStaticMeshComponent* trainMesh;
	
public:	
	// Sets default values for this actor's properties
	ACxxMonoRailTrain();

	void Init(ACxxRailLine* line, bool isForward, float tSpeed, float tOffSetRail, ACxxDispatchManager* tDispatchManager);
	void MovementControl(float DeltaTime);
	FVector GetNextPosition(float DeltaTime);
	FVector GetNextRotation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShowPassengerStatus();
	void BoardingAndAlighting();
};
