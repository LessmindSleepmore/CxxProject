// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxDispatchManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "CxxPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class MINIGAME_API ACxxPlayerController : public APlayerController
{
	GENERATED_BODY()

	// 视角缩放
	UPROPERTY() float zoomScale = 0.0f;

	virtual void SetupInputComponent() override;
	void SetMouseVisable();
	void OnLeftMousePressed();
	void OnLeftMouseReleased();
	
	// 视角控制
	void CameraMovementControl(float DeltaTime);
	void CameraForwardControl(float DeltaTime);
	void SetZoomScale(float value);

	// void CheckMouseOnStation();
	//
	// void SwapRailLine(AStation* station);
	//
	// void EndRailLineDraw(bool bIsCreate);
	//
	// FColor GetRailColor();
	
	// UPROPERTY() ARailLine* railLine = nullptr;
	// UPROPERTY() TArray<ARailLine*> railLines;
	// UPROPERTY() AStation* lastStation = nullptr;
	// UPROPERTY() TArray<FColor> railColors = {FColor::Green, FColor::Red, FColor::Blue};
	// UPROPERTY() int railColorIdx = 0;

	UPROPERTY() UCameraComponent* palyerCamera;
	UPROPERTY() ACxxDispatchManager* dispatchManager = nullptr;
	UPROPERTY() bool bGetDispatchManager = false;
	

	// bool bDrawRailLine = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};