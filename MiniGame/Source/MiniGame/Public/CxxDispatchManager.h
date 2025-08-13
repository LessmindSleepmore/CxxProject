// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxRailLine.h"
#include "CxxStation.h"
#include "Components/CanvasPanel.h"
#include "GameFramework/Actor.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "CxxDispatchManager.generated.h"

#ifndef StationChannel
#define StationChannel ECC_GameTraceChannel1
#endif

#ifndef RailLineChannel
#define RailLineChannel ECC_GameTraceChannel2
#endif

class ACxxPlayerController;
class ACxxMonoRailTrain;

UCLASS()
class MINIGAME_API ACxxDispatchManager : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) FString metroName = "深月";
	UPROPERTY(EditAnywhere) TArray<ACxxStation*> stationArray;
	UPROPERTY(EditAnywhere) TArray<ACxxRailLine*> railLines;
	UPROPERTY(EditAnywhere) TArray<ACxxMonoRailTrain*> trains;
	UPROPERTY(EditAnywhere) bool bIsDrawRailLine = false;
	UPROPERTY(EditAnywhere) float trainOffSetRail = 10;
	UPROPERTY(EditAnywhere) float trainSpeed = 100.;

	UPROPERTY(EditAnywhere) UClass* trainClass;
	UPROPERTY(EditAnywhere) UClass* HubClass;
	
	UPROPERTY(EditAnywhere) UUserWidget* hudWidget;
	UPROPERTY(EditAnywhere) UCanvasPanel* rootPanel;
	UPROPERTY(EditAnywhere) UCanvasPanel* addTrainCanvasPanel;
	UPROPERTY(EditAnywhere) UCanvasPanel* railEditCanvasPanel;
	UPROPERTY(EditAnywhere) UTextBlock* scoreText;
	UPROPERTY(EditAnywhere) UTextBlock* railNameTextBlock;
	UPROPERTY(EditAnywhere) UImage* ringLineIcon;

	// 当前编辑轨道
	UPROPERTY(EditAnywhere) ACxxRailLine* curSelectedRailLine = nullptr;

	// 编辑轨道
	UPROPERTY(EditAnywhere)  bool bIsSelectRail = false;
	UPROPERTY(EditAnywhere)  bool bIsEditRail = false;
	UPROPERTY(EditAnywhere)  bool bIsDeleteRail = false;
	UPROPERTY(EditAnywhere)  bool bIsCreateRail = false;
	UPROPERTY(EditAnywhere)  bool bIsModifyRail = false;
	UPROPERTY(EditAnywhere)  bool bIsEditView = false;

	// 修改逻辑
	UPROPERTY(EditAnywhere)  UCxxRailLineSegment* curSelectedSegment = nullptr;
	UPROPERTY(EditAnywhere)  ACxxStation* curSelectedStation = nullptr;
	UPROPERTY(EditAnywhere)  UCxxPathControlPoint* curSelectedPoint = nullptr;
	UPROPERTY(EditAnywhere)  UCxxPathControlPoint* lastSelectedPoint = nullptr;
	UPROPERTY(EditAnywhere)  FString operatorName;
	UPROPERTY(EditAnywhere) ACxxStation* lastStation = nullptr; // 用于检测鼠标位置车站状态的切换
	UPROPERTY(EditAnywhere) int stationIdx = -1;

	// 游戏状态
	UPROPERTY(EditAnywhere) bool bIsGameStage = false;

	// 线路图
	UPROPERTY(EditAnywhere) TArray<EStationType> stationType = {
		EStationType::BusinessDistrict,
		EStationType::ResidentialArea,
		EStationType::CulturalArea,
		EStationType::RecreationalArea};
	void GenerateRailLineGraph();

	UPROPERTY(EditAnywhere) bool bBeginPlay = false;
	
public:	
	// Sets default values for this actor's properties
	ACxxDispatchManager();
	
	void SwapRailLineActor(ACxxStation* station, bool bIsArrowDir);
	void CreateTrainInRailLine(ACxxRailLine* railLine, bool bIsInit);
	void CreateTrainInAllRailine();
	void RemoveTrainInAllRailine();
	float passengerTimer = 0.0f;
	FColor GetRailColor();
	void CreateHUD();

	// 添加车辆
	UPROPERTY(EditAnywhere) bool bIsAddTrain = false;
	UPROPERTY(EditAnywhere)  ACxxMonoRailTrain* curSelectedTrain = nullptr;
	UFUNCTION() void OnTrainCreateButtonClicked();
	void MonitorAddTrainInput();
	void HighLightHoverObject(bool bRailLine, bool bStation);

	void BeginIsAddTrain();
	void EndIsAddTrain();
	 
	// 创建轨道
	UPROPERTY() ACxxStation* createStation = nullptr;
	UPROPERTY() bool bCreateStationIsArrowDir = false;;
	UPROPERTY() bool bInitRailLine = false;
	FString GetRailLineName();

	// 编辑轨道
	UFUNCTION()
	void OnEditRailLineButtonClicked();
	void EditRailFromStation();
	void BeginEditRailLine();
	void EndEditRailLine();
	void InputProcess();
	void InitButton();
	void BindButton();
	UFUNCTION() void OnCancelButtonClicked();
	UFUNCTION() void OnDeleteButtonClicked();
	UFUNCTION() void OnDeletePointButtonClicked();
	void UpdateButtonAndIcon();
	UPROPERTY(EditAnywhere) UCanvasPanel* editCanvasPanel = nullptr;
	UPROPERTY(EditAnywhere) UButton* deleteButton = nullptr;
	UPROPERTY(EditAnywhere) UButton* cancelButton = nullptr;
	UPROPERTY(EditAnywhere) UButton* trainCreateButton = nullptr;
	UPROPERTY(EditAnywhere) UButton* railLineEditButton = nullptr;
	UPROPERTY(EditAnywhere) UButton* pointDeleteButton = nullptr;
	

	// 编辑视图
	void EnableEditView();
	void DisableEditView();

	// 处理输入
	void ModifyInputProcess();
	void GetRailLineObject();

	// 分数
	UPROPERTY() int score = 0;
	UPROPERTY() int todayScore = 0;
	void AddScore(int s);
	int GetScore();
	int GetToDayScore();

	// UI
	void HiddenUI();
	void ShowGameStageUI();
	void ShowEditStageUI();

	// 编辑段
	UPROPERTY(EditAnywhere) UClass* movementComponentClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* tangentComponentAClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* tangentComponentBClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* railLineBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarRepeatClass;
	void SplitSegment();

	// 玩家资源数量
	UPROPERTY(EditAnywhere) int trainNum = 1;
	UPROPERTY(EditAnywhere) int railNum = 1;
	UPROPERTY(EditAnywhere) int ringRingNum = 1;

	// 线路合规性检测
	bool bIsChanged = false;
	bool bRailIsVaild = true;;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
