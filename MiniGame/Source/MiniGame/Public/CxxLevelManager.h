// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxDispatchManager.h"
#include "CxxPlayerController.h"
#include "CxxScenePCGTool.h"
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
	UPROPERTY(EditAnywhere) TArray<EStationType> stationsTypeArray;
	UPROPERTY(EditAnywhere) TArray<FString> stationNameArray;
	UPROPERTY(EditAnywhere) TArray<FVector> stationsLocationArray;
	UPROPERTY(EditAnywhere) int stationCapacity = 10;
	UPROPERTY(EditAnywhere) TArray<int32> indexArray;
	UPROPERTY(EditAnywhere) ACxxDispatchManager* dispatchManager;
	UPROPERTY(EditAnywhere) float MinRailAngle = 35.f;
	
	// 站点
	UPROPERTY()
	TSoftClassPtr<AActor> businessClass;
	UPROPERTY()
	TSoftClassPtr<AActor> culturalClass;
	UPROPERTY()
	TSoftClassPtr<AActor> recreationalClass;
	UPROPERTY()
	TSoftClassPtr<AActor> residentialClass;
	
	
	UPROPERTY(EditAnywhere) UClass* business;
	UPROPERTY(EditAnywhere) UClass* cultural;
	UPROPERTY(EditAnywhere) UClass* recreational;
	UPROPERTY(EditAnywhere) UClass* residential;

	// UI
	UPROPERTY() UCanvasPanel* rootPanel;
	UPROPERTY() UCanvasPanel* itemCanvasPanel;
	UPROPERTY() UCanvasPanel* rebuildPanel;
	UPROPERTY() UCanvasPanel* gameOverPanel;
	UPROPERTY() UCanvasPanel* settleGamePanel;
	UPROPERTY() UCanvasPanel* tutorialA;
	UPROPERTY() UCanvasPanel* tutorialB;
	UPROPERTY() UCanvasPanel* tagCurveInvaild;
	UPROPERTY() UCanvasPanel* EndGamePlanel;
	
	UPROPERTY() UTextBlock* stageTextBlock;
	UPROPERTY() UTextBlock* timeTextBlock;
	UPROPERTY() UTextBlock* dayTextBlock;
	UPROPERTY() UTextBlock* trainNumberBlock;
	UPROPERTY() UTextBlock* railNumberBlock;
	UPROPERTY() UTextBlock* ringRailNumberBlock;
	
	UPROPERTY() UButton* nextDayButton = nullptr;
	UPROPERTY() UButton* itemCardOpen = nullptr;
	UPROPERTY() UButton* itemCardClose = nullptr;
	UPROPERTY() UButton* buyTrain = nullptr;
	UPROPERTY() UButton* sellTrain = nullptr;
	UPROPERTY() UButton* buyRail = nullptr;
	UPROPERTY() UButton* sellRail = nullptr;
	UPROPERTY() UButton* buyRing = nullptr;
	UPROPERTY() UButton* sellRing = nullptr;
	UPROPERTY() UButton* rebuildRailPanelButton = nullptr;
	UPROPERTY() UButton* rebuildButton = nullptr;
	UPROPERTY() UButton* cancelRebuildPlane = nullptr;
	UPROPERTY() UButton* gameOverReBuildRailLine = nullptr;
	UPROPERTY() UButton* replayGame = nullptr;
	UPROPERTY() UButton* endGamePlay = nullptr;
	UPROPERTY() UButton* replayGame2 = nullptr;
	UPROPERTY() UButton* endGamePlay2 = nullptr;
	UPROPERTY() UButton* tutorialAutton = nullptr;
	UPROPERTY() UButton* tutorialButton = nullptr;
	UPROPERTY() UButton* gamePlayCancelButton = nullptr;
	UPROPERTY() UButton* endGameButton = nullptr;

	UPROPERTY() UTexture2D* textureA = nullptr;
	UPROPERTY() UTexture2D* textureB = nullptr;
	UPROPERTY() UTexture2D* textureC = nullptr;
	UPROPERTY() UTexture2D* textureD = nullptr;

	UPROPERTY() UTexture2D* textureTA = nullptr;
	UPROPERTY() UTexture2D* textureTB = nullptr;
	UPROPERTY() UTexture2D* textureTC = nullptr;
	UPROPERTY() UTexture2D* textureTD = nullptr;

	// 价格
	int trainValue = 80;
	int railValue = 150;
	int ringValue = 100;
	
	// UI button 
	UFUNCTION() void OnNextDayButtonClicked();
	UFUNCTION() void OpenItemCard();
	UFUNCTION() void CloseItemCard();
	UFUNCTION() void CloseTutorialA();
	UFUNCTION() void CloseTutorialB();
	UFUNCTION() void ItemBuyTrain();
	UFUNCTION() void ItemSellTrain(); 
	UFUNCTION() void ItemBuyRail();
	UFUNCTION() void ItemSellRail();
	UFUNCTION() void ItemBuyRing();
	UFUNCTION() void ItemSellRing();
	UFUNCTION()	void CancelRebuildPlane();
	UFUNCTION()	void ShowRebuildPlane();
	UFUNCTION()	void ResetDay();
	UFUNCTION()	void ResetLevel();
	UFUNCTION()	void EndGamePlay();
	UFUNCTION()	void CancelEndGamePlay();

	
	void HiddenUI();
	void ShowEditStageUI();
	void ShowGameStageUI();
	void GetControlActor();
	void SetUIComponent();

	// UI生成
	// UPROPERTY() TMap<ACxxStation*, UHorizontalBox*> stationHorizontalBoxMap;
	UPROPERTY() ACxxScenePCGTool* scenePCGTool;
	bool bBeginPlay = false;

	// 游戏状态
	UPROPERTY(EditAnywhere) bool bIsGameStage = false;

	// 游戏时间控制（运营时间6：00 ~ 24：00）
	UPROPERTY(EditAnywhere) int gameDay = 1;
	UPROPERTY(EditAnywhere) int stationIdx = 3;
	UPROPERTY(EditAnywhere) float oneDayInRealTime = 10.0f;
	UPROPERTY(EditAnywhere) float gameDayTime = 0;
	UPROPERTY(EditAnywhere) TArray<int> dayCraeteStationArray = {3, 2, 1, 2, 1, 2, 2};
	UPROPERTY(EditAnywhere) int createStationIdx = 0;

	// 玩家资源数量（修改用）
	UPROPERTY(EditAnywhere) int trainNum = 1;
	UPROPERTY(EditAnywhere) int trainNumberSum = 1;
	UPROPERTY(EditAnywhere) int railNum = 1;
	UPROPERTY(EditAnywhere) int ringRingNum = 1;
	
	void TestFunc_SetStations();
	void GetGeneratedStations(); 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SwitchGameStage();
	void SwitchCameraProject();
	void CreateToDayStation(int num);
	void CreateStaion(int idx);
	FString TimeControl(float DeltaTime);
	void UpdataTimeTextBlock(FString time);
	void EmptyAllPassenger();
	void CreatePassagerUI(ACxxStation* station);
	void UpdatePassagerUI();
	void UpdateItemUI();
	void GameOver();
	void SettleGame();
	void CheckPressedGameOver();

	void AddStation(ACxxStation* station);

	// 乘客生成
	float passengerTimer = 0.0f;
	UPROPERTY(EditAnywhere)
	float maxSpawnRate = 0.5f;
	UPROPERTY(EditAnywhere)
	float decreaseRatio = 0.7f;
	UPROPERTY(EditAnywhere)
	float minRate = 0.1f;
	UPROPERTY(EditAnywhere)
	int32 passengerRandomSeed = 10086;
	FRandomStream passengerRandom;
	
	void CreateCxxPassenger(float DeltaTime);
	void CheckPassengerNumber();
	void ValidateRouteCompliance();
	void SetRailIsvaild(bool bVaild);
	bool GetRailIsvaild();
};