// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxPassenger.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "CxxStation.generated.h"

UCLASS()
class MINIGAME_API ACxxStation : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) EStationType stationType;
	// UPROPERTY(EditAnywhere) UWidgetComponent* widgetComponent;
	// UPROPERTY(EditAnywhere) UHorizontalBox* horizontalBox;
	//
public:	
	// Sets default values for this actor's properties
	ACxxStation();
	bool isHighlighted = false;
	UPROPERTY(EditAnywhere) TArray<ACxxPassenger*> passengers;

	// UI
	UPROPERTY() UHorizontalBox* horizontalBox = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 线路图
	UPROPERTY() TArray<ACxxStation*> businessDistrictPath;
	UPROPERTY() TArray<ACxxStation*> residentialAreaPath;
	UPROPERTY() TArray<ACxxStation*> culturalAreaPath;
	UPROPERTY() TArray<ACxxStation*> recreationalAreaPath;

	void AddPassenger(ACxxPassenger* passenger);
	void RemovePassenger(ACxxPassenger* passenger);
	void SwitchOutlineEffect();
	EStationType GetStationType();
	void ShowPassengerStatus();
};
