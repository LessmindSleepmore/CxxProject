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

	UPROPERTY(EditAnywhere) ACxxDispatchManager* dispatchManager;
	UPROPERTY(EditAnywhere) UCxxRailLineSegment* curSegment = nullptr;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* mesh = nullptr;
	UPROPERTY(EditAnywhere) float curSegmentMovedDist = 0.;
	UPROPERTY(EditAnywhere) float speed = 300.;
	UPROPERTY(EditAnywhere) bool bCurIsForward = true;
	UPROPERTY(EditAnywhere) float offSetRail = 30;
	UPROPERTY(EditAnywhere) int capacity = 10;
	UPROPERTY(EditAnywhere) TArray<ACxxPassenger*> passengers;
	UPROPERTY(EditAnywhere) bool bInStation = false;
	UPROPERTY(EditAnywhere) float inStationTime = 0;
	bool bIsHighlighted = false;
	bool bMarkedForRemoval = false;
public:
	// UI
	UPROPERTY() UImage* trainImage = nullptr;
	UPROPERTY() UTexture2D* traintexture = nullptr;
	UPROPERTY(EditAnywhere) UHorizontalBox* horizontalBox = nullptr;
	UPROPERTY(EditAnywhere) ACxxRailLine* railLine;
	
public:	
	// Sets default values for this actor's properties
	ACxxMonoRailTrain();

	void Init(ACxxRailLine* line, bool isForward, float tSpeed, float tOffSetRail, ACxxDispatchManager* tDispatchManager, UCxxRailLineSegment* curSegment, UCanvasPanel* rootPanel);
	void MovementControl(float DeltaTime);
	FVector GetNextPosition(float DeltaTime);
	FRotator GetNextRotation();
	void SwitchHightLightEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShowPassengerStatus();
	void BoardingAndAlighting();
	void AddPassenger(ACxxPassenger* passenger);
	void MarkTrainAsDelete();
	void DestroyTrain();
};
