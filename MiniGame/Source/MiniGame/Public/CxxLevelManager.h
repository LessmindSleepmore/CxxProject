// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxDispatchManager.h"
#include "CxxStation.h"
#include "Components/TextBlock.h"
#include "GameFramework/Actor.h"
#include "CxxLevelManager.generated.h"

UCLASS()
class MINIGAME_API ACxxLevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACxxLevelManager();
	UPROPERTY(EditAnywhere) TArray<ACxxStation*> stationsArray;
	UPROPERTY(EditAnywhere) TArray<FString> stationNameArray;
	UPROPERTY(EditAnywhere) TArray<FVector> stationsLocationArray;
	UPROPERTY(EditAnywhere) ACxxDispatchManager* dispatchManager;
	
	// 站点
	UPROPERTY(EditAnywhere) UClass* business;
	UPROPERTY(EditAnywhere) UClass* cultural;
	UPROPERTY(EditAnywhere) UClass* recreational;
	UPROPERTY(EditAnywhere) UClass* residential;

	// UI
	UPROPERTY(EditAnywhere) UTextBlock* stageTextBlock;
	UPROPERTY(EditAnywhere) UTextBlock* timeTextBlock;
	UPROPERTY(EditAnywhere) UTextBlock* dayTextBlock;
	UPROPERTY(EditAnywhere) UButton* nextDayButton = nullptr;
	UFUNCTION()
	void OnNextDayButtonClicked();
	void HiddenUI();
	void ShowEditStageUI();
	void ShowGameStageUI();
	void GetAndSetUIComponent();

	// 游戏状态
	UPROPERTY(EditAnywhere) bool bIsGameStage = false;

	// 游戏时间控制（运营时间6：00 ~ 24：00）
	UPROPERTY(EditAnywhere) int gameDay = 1;
	UPROPERTY(EditAnywhere) float oneDayInRealTime = 60.0f;
	UPROPERTY(EditAnywhere) float gameDayTime = 0;
	
	void TestFunc_SetStations();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SwitchGameStage();
	void InitBeginStation();
	void CreateStaion(FString stationName, FVector location);
	FString TimeControl(float DeltaTime);
	void UpdataTimeTextBlock(FString time);

	// 乘客生成
	float passengerTimer = 0.0f;
	void CreateCxxPassenger(float DeltaTime);
};
