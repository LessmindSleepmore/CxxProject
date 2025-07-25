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
	UPROPERTY(EditAnywhere) TArray<ACxxStation*> stations;
	UPROPERTY(EditAnywhere) TArray<ACxxRailLine*> railLines;
	UPROPERTY(EditAnywhere) TArray<ACxxMonoRailTrain*> trains;
	UPROPERTY(EditAnywhere) bool bIsDrawRailLine = false;
	UPROPERTY(EditAnywhere) float trainOffSetRail = 10;
	UPROPERTY(EditAnywhere) float trainSpeed = 100.;

	UPROPERTY(EditAnywhere) UClass* trainClass;
	UPROPERTY(EditAnywhere) UClass* HubClass;
	
	UPROPERTY(EditAnywhere) UUserWidget* hudWidget;
	UPROPERTY(EditAnywhere) bool bIsAddTrain = false;
	UPROPERTY(EditAnywhere) UCanvasPanel* addTrainCanvasPanel;
	UPROPERTY(EditAnywhere) UCanvasPanel* railEditCanvasPanel;
	UPROPERTY(EditAnywhere) UTextBlock* scoreText;

	// 当前编辑轨道
	UPROPERTY(EditAnywhere) ACxxRailLine* curSelectedRailLine = nullptr;
	UPROPERTY(EditAnywhere) FVector mouseLocation = FVector::ZeroVector;
	// UPROPERTY() ACxxRailLine* editRailLine = nullptr;

	// 编辑轨道
	UPROPERTY(EditAnywhere)  bool bIsSelectRail = false;
	UPROPERTY(EditAnywhere)  bool bIsEditRail = false;
	UPROPERTY(EditAnywhere)  bool bIsDeleteRail = false;
	UPROPERTY(EditAnywhere)  bool bIsCreateRail = false;
	UPROPERTY(EditAnywhere)  bool bIsModifyRail = false;

	UPROPERTY(EditAnywhere)  bool bIsEditView = false;

	// 修改逻辑
	// UPROPERTY(EditAnywhere)  bool bIsSelectStation = false;
	// UPROPERTY(EditAnywhere)  bool bLeftMouseDown = false;
	UPROPERTY(EditAnywhere)  UCxxRailSegment* curSelectedSegment = nullptr;
	UPROPERTY(EditAnywhere)  ACxxStation* curSelectedStation = nullptr;
	// UPROPERTY(EditAnywhere)  UCxxRailSegment* curEditSegment = nullptr;
	UPROPERTY(EditAnywhere) ACxxStation* lastStation = nullptr; // 用于检测鼠标位置车站状态的切换
	UPROPERTY(EditAnywhere) int stationIdx = -1;
	
public:	
	// Sets default values for this actor's properties
	ACxxDispatchManager();

	// void StartCreateRailLine();
	// void EndCreateRailLine();
	void SwapRailLineActor(ACxxStation* station, bool bIsArrowDir);
	void UpdataTickOperate();
	void CreateTrainInRailLine(ACxxRailLine* railLine);

	// 生成乘客
	void CreateCxxPassenger(float DeltaTime);
	float passengerTimer = 0.0f;
	// 【测试用】获取所有场景中的Station对象
	void TestFuncGetAllStation();
	FColor GetRailColor();
	void CreateHUD();

	// 添加车辆
	UFUNCTION()
	void OnTrainCreateButtonClicked();
	UFUNCTION()
	// void OnTrainCreateButtonReleased();
	void MonitorAddTrainInput();
	void HighLightHoverObject(bool bRailLine, bool bStation);

	void BeginIsAddTrain();
	void EndIsAddTrain();
	
	// 创建轨道
	void CreateRailFromStation(ACxxStation* station);
	UPROPERTY() ACxxStation* createStation = nullptr;
	UPROPERTY() bool bCreateStationIsArrowDir = false;
	void CreateInputProcess();
	UPROPERTY() bool bInitRailLine = false;

	// 编辑轨道
	UFUNCTION()
	void OnEditRailLineButtonClicked();
	void MonitorEditRailLineInput();
	void EditRailFromStation();
	void UpdataMouseLocation();
	
	void BeginEditRailLine(); // 包括create delete modify select
	void EndEditRailLine();
	// void BeginSelectRailLine();
	// void EndSelectRailLine();
	
	void InputProcess();

	void UpdataButtonLocation();
	void InitButton();
	void BindButton();
	UFUNCTION() void OnCancelButtonClicked();
	UFUNCTION() void OnDeleteButtonClicked();
	UCanvasPanel* editCanvasPanel = nullptr;
	UButton* deleteButton = nullptr;
	UButton* cancelButton = nullptr;
	

	// 编辑视图
	void EnableEditView();
	void DisableEditView();
	void ShowEditViewRailLine();

	// 处理输入
	void ModifyInputProcess();
	void GetRailLineStationOrSegment();

	// 分数
	UPROPERTY() int score = 0;
	void AddScore(int s);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
