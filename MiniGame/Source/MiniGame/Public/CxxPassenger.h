// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CxxPassenger.generated.h"

// 商业区 居住区 文化区 休闲区
UENUM()
enum class EStationType : uint8
{
	BusinessDistrict,
	ResidentialArea,
	CulturalArea,
	RecreationalArea
};

UCLASS()
class MINIGAME_API ACxxPassenger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACxxPassenger();

	UPROPERTY(EditAnywhere) EStationType targetStationType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsBoardingRequired(TArray<EStationType> path);
	bool IsAlightingRequired(TArray<EStationType> path);
};
