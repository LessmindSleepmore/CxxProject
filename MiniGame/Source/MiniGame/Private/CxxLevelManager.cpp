// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxLevelManager.h"

#include "CxxMonoRailTrain.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Internationalization/FastDecimalFormat.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACxxLevelManager::ACxxLevelManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
 
void ACxxLevelManager::OnNextDayButtonClicked()
{
	if (GetRailIsvaild())
	{
		if(dispatchManager) dispatchManager->EndEditRailLine();
		SwitchGameStage();	
	}
}

void ACxxLevelManager::OpenItemCard()
{
	itemCanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxLevelManager::CloseItemCard()
{
	itemCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::CloseTutorialA()
{
	tutorialA->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::CloseTutorialB()
{
	tutorialB->SetVisibility(ESlateVisibility::Hidden);
	UWorld* world = GetWorld();
	if (world)
	{
		world->GetWorldSettings()->SetTimeDilation(1.f);
	}
}

void ACxxLevelManager::ItemBuyTrain()
{
	if(dispatchManager)
	{
		int score = dispatchManager->GetScore();
		if(score >= trainValue)
		{
			dispatchManager->AddScore(-trainValue);
			dispatchManager->trainNum++;
			trainNum++;
			trainNumberSum++;
			trainNumberBlock->SetText(FText::FromString("x" + FString::FromInt(trainNum)));
		}
	}
}

void ACxxLevelManager::ItemSellTrain()
{
	if(dispatchManager)
	{
		if(trainNum > 0)
		{
			dispatchManager->AddScore(trainValue / 2);
			dispatchManager->trainNum--;
			trainNum--;
			trainNumberSum--;
			trainNumberBlock->SetText(FText::FromString("x" + FString::FromInt(trainNum)));
		}
	}
}

void ACxxLevelManager::ItemBuyRail()
{
	if(dispatchManager)
	{
		int score = dispatchManager->GetScore();
		if(score >= railValue)
		{
			dispatchManager->AddScore(-railValue);
			dispatchManager->railNum++;
			railNum++;
			railNumberBlock->SetText(FText::FromString("x" + FString::FromInt(railNum)));
		}
	}
}

void ACxxLevelManager::ItemSellRail()
{
	if(dispatchManager)
	{
	 if(railNum > 0)
	 {
		 dispatchManager->AddScore(railValue / 2);
		 dispatchManager->railNum--;
		 railNum--;
		 railNumberBlock->SetText(FText::FromString("x" + FString::FromInt(railNum)));
	 }
	}
}

void ACxxLevelManager::ItemBuyRing()
{
	if(dispatchManager)
	{
		int score = dispatchManager->GetScore();
		if(score >= railValue)
		{
			dispatchManager->AddScore(-ringValue);
			dispatchManager->ringRingNum++;
			ringRingNum++;
			ringRailNumberBlock->SetText(FText::FromString("x" + FString::FromInt(ringRingNum)));
		}
	}
}

void ACxxLevelManager::ItemSellRing()
{
	if(dispatchManager)
	{
	 if(ringRingNum > 0)
	 {
		 dispatchManager->AddScore(ringValue / 2);
		 dispatchManager->ringRingNum--;
		 ringRingNum--;
		 ringRailNumberBlock->SetText(FText::FromString("x" + FString::FromInt(ringRingNum)));
	 }
	}
}


void ACxxLevelManager::HiddenUI()
{
	if(dispatchManager) dispatchManager->HiddenUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Hidden);
 }

void ACxxLevelManager::ShowEditStageUI()
{
	HiddenUI();
	if(dispatchManager) dispatchManager->ShowEditStageUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Visible);
	if(itemCanvasPanel) itemCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	if(itemCardOpen) itemCardOpen->SetVisibility(ESlateVisibility::Visible);
	if(rebuildPanel) rebuildPanel->SetVisibility(ESlateVisibility::Hidden);
	if(rebuildRailPanelButton) rebuildRailPanelButton->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::ShowGameStageUI()
{
	HiddenUI();
	if(dispatchManager) dispatchManager->ShowGameStageUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Hidden);
	if(itemCanvasPanel) itemCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	if(itemCardOpen) itemCardOpen->SetVisibility(ESlateVisibility::Hidden);
	if(rebuildPanel) rebuildPanel->SetVisibility(ESlateVisibility::Hidden);
	if(rebuildRailPanelButton) rebuildRailPanelButton->SetVisibility(ESlateVisibility::Visible);
}

void ACxxLevelManager::GetControlActor()
{
	// 获取场景中的dispatchManager和UI组件
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxDispatchManager::StaticClass(), foundActors);
	if (foundActors.Num() > 0)
	{
		dispatchManager = Cast<ACxxDispatchManager>(foundActors[0]);
	}

	// 获取scenePCGTool
	TArray<AActor*> foundPCGTool;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxScenePCGTool::StaticClass(), foundPCGTool); 
	if (foundPCGTool.Num() > 0)
	{
		scenePCGTool = Cast<ACxxScenePCGTool>(foundPCGTool[0]);
	}
}

void ACxxLevelManager::SetUIComponent()
{
	if (dispatchManager && dispatchManager->hudWidget)
	{
		rootPanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RootPanel")));
		itemCanvasPanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("ItemCanvasPanel")));
		rebuildPanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RebuildPanel")));
		gameOverPanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("GameOver")));
		settleGamePanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("SettleGame")));
		tutorialA = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TutorialA")));
		tutorialB = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TutorialB")));
		tagCurveInvaild = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TagCurveInvaild")));
		EndGamePlanel = Cast<UCanvasPanel>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("GamEnd")));
		
		stageTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("StageText")));
		timeTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("DayTime")));
		dayTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("DayText")));
		trainNumberBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TrainNumber")));
		railNumberBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RailNumber")));
		ringRailNumberBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RingRailNumber")));
		
		nextDayButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("NextDayButton")));
		itemCardOpen = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("ItemCardOpen")));
		itemCardClose = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("ItemCardClose")));
		buyTrain = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("BuyTrain")));
		sellTrain = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("SellTrain")));
		buyRail = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("BuyRail")));
		sellRail = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("SellRail")));
		buyRing = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("BuyRing")));
		sellRing = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("SellRing")));
		rebuildButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RebuildButton")));
		cancelRebuildPlane = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("CancelRebuildPlane")));
		rebuildRailPanelButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("RebuildRailPanelButton")));
		gameOverReBuildRailLine = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("GameOverReBuildRailLine")));
		replayGame = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("ReplayGame")));
		endGamePlay = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("EndGamePlay")));
		replayGame2 = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("ReplayGame_1")));
		endGamePlay2 = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("EndGamePlay_1")));
		tutorialAutton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TutorialAButton")));
		tutorialButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("TutorialButton")));
		gamePlayCancelButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("GameOverCancel")));
		endGameButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("EndGamePlay_2")));
		
		// 显示编辑UI
		ShowEditStageUI();
		gameOverPanel->SetVisibility(ESlateVisibility::Hidden);
		settleGamePanel->SetVisibility(ESlateVisibility::Hidden);
		tutorialA->SetVisibility(ESlateVisibility::Visible);
		tutorialB->SetVisibility(ESlateVisibility::Hidden);
		tagCurveInvaild->SetVisibility(ESlateVisibility::Hidden);
		EndGamePlanel->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if(nextDayButton)
	{
		nextDayButton->OnClicked.AddDynamic(this, &ACxxLevelManager::OnNextDayButtonClicked);
	}

	if(itemCardOpen)
	{
		itemCardOpen->OnClicked.AddDynamic(this, &ACxxLevelManager::OpenItemCard);
	}

	if(itemCardClose)
	{
		itemCardClose->OnClicked.AddDynamic(this, &ACxxLevelManager::CloseItemCard);
	}

	if(buyTrain)
	{
		buyTrain->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemBuyTrain);
	}

	if(sellTrain)
	{
		sellTrain->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemSellTrain);
	}

	if(buyRail)
	{
		buyRail->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemBuyRail);
	}
	
	if(sellRail)
	{
		sellRail->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemSellRail);
	}

	if(buyRing)
	{
		buyRing->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemBuyRing);
	}

	if(sellRing)
	{
		sellRing->OnClicked.AddDynamic(this, &ACxxLevelManager::ItemSellRing);
	}

	if(cancelRebuildPlane)
	{
		cancelRebuildPlane->OnClicked.AddDynamic(this, &ACxxLevelManager::CancelRebuildPlane);
	}

	if (rebuildButton)
	{
		rebuildButton->OnClicked.AddDynamic(this, &ACxxLevelManager::ResetDay);
	}

	if (rebuildRailPanelButton)
	{
		rebuildRailPanelButton->OnClicked.AddDynamic(this, &ACxxLevelManager::ShowRebuildPlane);
	}

	if (gameOverReBuildRailLine)
	{
		gameOverReBuildRailLine->OnClicked.AddDynamic(this, &ACxxLevelManager::ResetDay);
	}

	if (replayGame)
	{
		replayGame->OnClicked.AddDynamic(this, &ACxxLevelManager::ResetLevel);
	}

	if (endGamePlay)
	{
		endGamePlay->OnClicked.AddDynamic(this, &ACxxLevelManager::EndGamePlay);
	}

	if (replayGame2)
	{
		replayGame2->OnClicked.AddDynamic(this, &ACxxLevelManager::ResetLevel);
	}

	if (endGamePlay2)
	{
		endGamePlay2->OnClicked.AddDynamic(this, &ACxxLevelManager::EndGamePlay);
	}

	if (tutorialAutton)
	{
		tutorialAutton->OnClicked.AddDynamic(this, &ACxxLevelManager::CloseTutorialA);
	}

	if (tutorialButton)
	{
		tutorialButton->OnClicked.AddDynamic(this, &ACxxLevelManager::CloseTutorialB);
	}

	if (gamePlayCancelButton)
	{
		gamePlayCancelButton->OnClicked.AddDynamic(this, &ACxxLevelManager::CancelEndGamePlay);
	}


	if (endGameButton)
	{
		endGameButton->OnClicked.AddDynamic(this, &ACxxLevelManager::EndGamePlay);
	}


	for (auto station:stationsArray)
	{
		dispatchManager->stationArray.Add(station);
		CreatePassagerUI(station);
	}
}

void ACxxLevelManager::TestFunc_SetStations()
{
	stationNameArray.Add(TEXT("residential"));
	stationNameArray.Add(TEXT("cultural"));
	stationNameArray.Add(TEXT("business"));
	stationNameArray.Add(TEXT("residential"));
	stationNameArray.Add(TEXT("recreational"));

	stationsLocationArray.Add(FVector(1460.0, -1140.0, 0));
	stationsLocationArray.Add(FVector(1500.0, 110.0, 0));
	stationsLocationArray.Add(FVector(1070.0, -650.0, 0));
	stationsLocationArray.Add(FVector(810.0, 380.0, 0));
	stationsLocationArray.Add(FVector(700.0, -180.0, 0));
}
 
void ACxxLevelManager::GetGeneratedStations()
{
	if(scenePCGTool->stationLoc.Num() == 0) return;
	if(scenePCGTool)
	{
		for(auto v:scenePCGTool->stationLoc)
		{
			stationsLocationArray.Add(scenePCGTool->GetBlockLoction(v.X, v.Y) + FVector(0, 0, 70));
		}

		for(auto type:scenePCGTool->stationType)
		{
			switch (type)
			{
			case 1:
				stationNameArray.Add(TEXT("residential"));
				break;
			case 2:
				stationNameArray.Add(TEXT("cultural"));
				break;
			case 3:
				stationNameArray.Add(TEXT("recreational"));
				break;
			case 4:
				stationNameArray.Add(TEXT("business"));
				break;
			default:
				stationNameArray.Add(TEXT("residential"));
				break;
			}
		}
	}
	
	for (int32 i = 0; i < stationNameArray.Num(); ++i)
	{
		indexArray.Add(i);
	}
	
	bool bValidShuffle = false;

	// 避免无限循环
	int shuffleCount = 10;
	while (!bValidShuffle)
	{
		Algo::RandomShuffle(indexArray);
		bValidShuffle = true;
		for (int32 i = 2; i < indexArray.Num(); ++i)
		{
			if (stationNameArray[indexArray[i]] == stationNameArray[indexArray[i - 1]] &&
				stationNameArray[indexArray[i]] == stationNameArray[indexArray[i - 2]])
			{
				bValidShuffle = false;
				break;
			}
		}
		if(shuffleCount <= 0) break;
		shuffleCount--;
	}
}

// Called when the game starts or when spawned
void ACxxLevelManager::BeginPlay()
{
	Super::BeginPlay();
	// TestFunc_SetStations();
	GetControlActor();

	textureA = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Image/Passager1.Passager1"));
	textureB = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Image/Passager2.Passager2"));
	textureC = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Image/Passager3.Passager3"));
	textureD = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Image/Passager4.Passager4"));

	textureTA = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/Image/p1.p1'"));
	textureTB = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/Image/p2.p2'"));
	textureTC = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/Image/p3.p3'"));
	textureTD = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/Image/p4.p4'"));
	
	passengerRandom.Initialize(passengerRandomSeed);
}

// Called every frame
void ACxxLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(scenePCGTool && dispatchManager && scenePCGTool->bBeginPlay && dispatchManager->bBeginPlay && !bBeginPlay)
	{
		SetUIComponent();
		GetGeneratedStations();
		CreateToDayStation(dayCraeteStationArray[gameDay - 1]);
		bBeginPlay = true;
	}else
	{
		if(bIsGameStage){
			FString timeStr = TimeControl(DeltaTime);
			UpdataTimeTextBlock(timeStr);
			CheckPassengerNumber();
			CreateCxxPassenger(DeltaTime);
			UpdatePassagerUI();
		}
		UpdateItemUI();
		ValidateRouteCompliance();
		CheckPressedGameOver();
	}
}

// 当天结束和开始运营都会切换状态
void ACxxLevelManager::SwitchGameStage()
{
	bIsGameStage = !bIsGameStage;
	// 编辑阶段
	if(!bIsGameStage)
	{
		gameDay++;
		// 初始化生成种子
		passengerRandomSeed = static_cast<int32>(FDateTime::UtcNow().GetTicks() % INT32_MAX);
		passengerRandom.Initialize(passengerRandomSeed);
		// 结算
		if (gameDay > 7)
		{
			SettleGame();
			return;
		}
		
		gameDayTime = 0.0f;
		// 增加车站
		CreateToDayStation(dayCraeteStationArray[gameDay - 1]);
		stageTextBlock->SetText(FText::FromString(TEXT("编辑阶段")));
		dayTextBlock->SetText(FText::FromString(TEXT("第" + FString::FromInt(gameDay) + TEXT("天"))));
		dispatchManager->RemoveTrainInAllRailine();
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxMonoRailTrain::StaticClass(), foundActors);

		for (AActor* actor : foundActors)
		{
			ACxxMonoRailTrain* train = Cast<ACxxMonoRailTrain>(actor);
			if (IsValid(train) && train)
			{
				train->DestroyTrain();
			}
		}
		EmptyAllPassenger();
		ShowEditStageUI();
		// 设置为正交相机
		SwitchCameraProject();
		// 修正列车数量
		dispatchManager->trainNum = trainNumberSum;
		UpdataTimeTextBlock("6:00");
	} 
	// 游戏阶段
	else
	{
		if (gameDay == 1 && tutorialB)
		{
			tutorialB->SetVisibility(ESlateVisibility::Visible);
			UWorld* world = GetWorld();
			if (world)
			{
				world->GetWorldSettings()->SetTimeDilation(0.01f);
			}
		}
		for (auto stat: dispatchManager->stationArray)
		{
			UStaticMeshComponent* comp = Cast<UStaticMeshComponent>(stat->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (comp)
			{
				stat->isHighlighted = false;
				comp->SetRenderCustomDepth(false);
			}
		}
		stageTextBlock->SetText(FText::FromString(TEXT("运营阶段")));
		dispatchManager->CreateTrainInAllRailine();
		// 生成路线图
		dispatchManager->GenerateRailLineGraph();
		ShowGameStageUI();
		// 设置为透视相机 
		SwitchCameraProject();
	}
}

void ACxxLevelManager::SwitchCameraProject()
{
	APawn* pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if(pawn)
	{
		UCameraComponent* camera = pawn->FindComponentByClass<UCameraComponent>();
		if(camera)
		{
			if(camera->ProjectionMode == ECameraProjectionMode::Perspective)
			{
				camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
				camera->SetRelativeRotation(FRotator(-90, 0, 0));
				camera->OrthoWidth = 13000.0f;
			}else
			{ 
				camera->SetProjectionMode(ECameraProjectionMode::Perspective);
				camera->SetRelativeRotation(FRotator(-60, 0, 0));
			}
		}
	}
}

void ACxxLevelManager::CreateToDayStation(int num)
{
	for (int i = 0; i < num; i++)
	{
		CreateStaion(createStationIdx++);
	}
}

void ACxxLevelManager::CreateStaion(int idx)
{
	if(!scenePCGTool || indexArray.Num() <= idx) return;
	FActorSpawnParameters spawnParams;
	ACxxStation* station = nullptr;

	FRotator rot = FRotator(0, 0, 0);
	if (FMath::RandBool())
	{
		rot.Yaw += 90.0f;
	}
	
	// 隐藏当前位置的模型
	scenePCGTool->SetBuildVisible(scenePCGTool->stationLoc[indexArray[idx]], false);
	scenePCGTool->buildCollisionCount[FMath::FloorToInt(scenePCGTool->stationLoc[indexArray[idx]].X)][FMath::FloorToInt(scenePCGTool->stationLoc[indexArray[idx]].Y)]++;

	if (stationNameArray[indexArray[idx]] == TEXT("residential"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(residential, stationsLocationArray[indexArray[idx]], rot, spawnParams));
		AddStation(station);
	}
	else if (stationNameArray[indexArray[idx]] == TEXT("cultural"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(cultural, stationsLocationArray[indexArray[idx]], rot, spawnParams));
		AddStation(station);
	}
	else if (stationNameArray[indexArray[idx]] == TEXT("business"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(business, stationsLocationArray[indexArray[idx]], rot, spawnParams));
		AddStation(station);
	}
	else if (stationNameArray[indexArray[idx]] == TEXT("recreational"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(recreational, stationsLocationArray[indexArray[idx]], rot, spawnParams));
		AddStation(station);
	}

	if(dispatchManager) dispatchManager->stationArray.Add(station);
}

FString ACxxLevelManager::TimeControl(float DeltaTime) 
{
	gameDayTime += DeltaTime;
	if (gameDayTime > oneDayInRealTime)
	{
		SwitchGameStage();
	}

	float realTime = 6.0f + (gameDayTime * (24.0f - 6.0f)) / oneDayInRealTime;
	int hour = FMath::FloorToInt(realTime);
	int minute = FMath::FloorToInt((realTime - hour) * 60);
	return FString::Printf(TEXT("%02d:%02d"), hour, minute);
}

void ACxxLevelManager::UpdataTimeTextBlock(FString time)
{
	timeTextBlock->SetText(FText::FromString(time));
}

void ACxxLevelManager::EmptyAllPassenger()
{
	for (auto station : stationsArray)
	{
		for (auto passenger:station->passengers)
		{
			passenger->DestroyPassenger();
		}
		if (!station->passengers.IsEmpty()) station->passengers.Empty();
	}
}

void ACxxLevelManager::CreatePassagerUI(ACxxStation* station)
{
	station->horizontalBox = NewObject<UHorizontalBox>(dispatchManager->hudWidget);
	
	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(rootPanel->AddChild(station->horizontalBox));
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	if (cxxPC)
	{
		if (slot)
		{
			slot->SetOffsets(FMargin(-1000.0, -1000.0, 320, 64));
			slot->SetAlignment(FVector2D(0.5, 0.5));
			slot->SetAutoSize(true);
		}
	}
}

void ACxxLevelManager::UpdatePassagerUI()
{
	for(auto station:stationsArray)
	{
		FVector2D sPosition;
		ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
		FVector stationLoc = station->GetActorLocation();
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(station->horizontalBox->Slot);
		if (cxxPC && cxxPC->ProjectWorldLocationToScreen(stationLoc, sPosition))
		{
			if (slot)
			{
				slot->SetPosition(sPosition);
				slot->SetZOrder(-1);   
			}
		}		
	}

	for(auto train:dispatchManager->trains)
	{
		if(!train) continue;;
		FVector2D sPosition;
		ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
		FVector stationLoc = train->GetActorLocation();
		if(train->horizontalBox)
		{
			UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(train->horizontalBox->Slot);
			if (cxxPC && cxxPC->ProjectWorldLocationToScreen(stationLoc, sPosition))
			{
				if (slot)
				{
					slot->SetPosition(sPosition);
					slot->SetZOrder(-1);   
				}
			}	
		}
	}
}

void ACxxLevelManager::UpdateItemUI()
{
	if (dispatchManager)
	{
		if (dispatchManager->trainNum != trainNum)
		{
			trainNumberBlock->SetText(FText::FromString("x" + FString::FromInt(dispatchManager->trainNum)));
			trainNum = dispatchManager->trainNum;
		}
		if (dispatchManager->railNum != railNum)
		{
			railNumberBlock->SetText(FText::FromString("x" + FString::FromInt(dispatchManager->railNum)));
			railNum = dispatchManager->railNum;
		}
		if (dispatchManager->ringRingNum != ringRingNum)
		{
			ringRailNumberBlock->SetText(FText::FromString("x" + FString::FromInt(dispatchManager->ringRingNum)));
			ringRingNum = dispatchManager->ringRingNum;
		}
	}
}

void ACxxLevelManager::GameOver()
{
	if (gameOverPanel) gameOverPanel->SetVisibility(ESlateVisibility::Visible);
	UWorld* world = GetWorld();
	if (world)
	{
		world->GetWorldSettings()->SetTimeDilation(0.01f);
	}
}

void ACxxLevelManager::SettleGame()
{
	settleGamePanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxLevelManager::CheckPressedGameOver()
{
	APlayerController* cxxPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (cxxPC && cxxPC->IsInputKeyDown(EKeys::Escape))
	{
		EndGamePlanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACxxLevelManager::CancelRebuildPlane()
{
	rebuildPanel->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::ShowRebuildPlane()
{
	rebuildPanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxLevelManager::ResetDay()
{
	passengerRandom.Initialize(passengerRandomSeed);
	bIsGameStage = !bIsGameStage;
	gameDayTime = 0.0f;
	stageTextBlock->SetText(FText::FromString(TEXT("编辑阶段")));
	dayTextBlock->SetText(FText::FromString(TEXT("第" + FString::FromInt(gameDay) + TEXT("天"))));
	if (dispatchManager) {
		dispatchManager->AddScore(-dispatchManager->GetToDayScore());
		dispatchManager->RemoveTrainInAllRailine();
		// 修正列车数量
		dispatchManager->trainNum = trainNumberSum;
	}
	if (gameOverPanel) gameOverPanel->SetVisibility(ESlateVisibility::Hidden);
	EmptyAllPassenger();
	ShowEditStageUI();
	// 设置为正交相机
	SwitchCameraProject();
	UpdataTimeTextBlock("6:00");
}

void ACxxLevelManager::ResetLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("LevelBulid"));
}

void ACxxLevelManager::EndGamePlay()
{
	if (UWorld* world = GetWorld())
	{
		APlayerController* cxxPC = world->GetFirstPlayerController();
		UKismetSystemLibrary::QuitGame(world, cxxPC, EQuitPreference::Quit, true);
	}
}

void ACxxLevelManager::CancelEndGamePlay()
{
	EndGamePlanel->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::AddStation(ACxxStation* station)
{
	stationsArray.Add(station);
	stationsTypeArray.Add(station->GetStationType());
	// 创建一个UHorizontalBox
	if (dispatchManager)
	{
		CreatePassagerUI(station);
	}
}

void ACxxLevelManager::CreateCxxPassenger(float DeltaTime)
{
	// 每隔一段1/60s随机生成一次乘客
	passengerTimer += DeltaTime;
	const float sampleTime = 1.f / 60.f;
	if(passengerTimer < sampleTime) return;
	passengerTimer = 0.f;
	
	float timeFactor = 1.f;
	if (oneDayInRealTime > 0.f)
	{
		timeFactor = 0.5f * (1.f - FMath::Cos(2.f * PI * FMath::Fmod(gameDayTime, oneDayInRealTime) / oneDayInRealTime));
	}
	
	// 每个车站随机生成乘客
	for(ACxxStation* station : stationsArray)
	{
		if (!station) continue;
		float stationCapacityRate = FMath::Clamp(static_cast<float>(station->passengers.Num()) / static_cast<float>(stationCapacity), 0.f, 1.f);
		float rate = maxSpawnRate * timeFactor * (stationCapacityRate > decreaseRatio?minRate:1.f);

		// 速率转为本次抽样的命中概率
		float p = 1.f - FMath::Exp(-rate * sampleTime);
     	if(passengerRandom.FRand() < p)
     	{
			// 随机生成一个不是当前站EStationType的EStationType
			EStationType randType = static_cast<EStationType>(FMath::RandRange(0, 3));
     		int count = 1000;
			while(randType == station->GetStationType() || !stationsTypeArray.Contains(randType))
			{
				randType = static_cast<EStationType>(FMath::RandRange(0, 3));
				if (count-- < 0) return;
			}
			// 创建一个CxxPassenger对象
			ACxxPassenger* passenger = GetWorld()->SpawnActor<ACxxPassenger>(ACxxPassenger::StaticClass());
			if(passenger)
			{
				passenger->AttachToActor(station, FAttachmentTransformRules::KeepRelativeTransform);
				passenger->targetStationType = randType;
				// 创建乘客UI
				passenger->passagerImage = NewObject<UImage>(dispatchManager->hudWidget);
				UTexture2D* texture = nullptr;

				// 设置路径
				switch (passenger->targetStationType)
				{
				case EStationType::BusinessDistrict:
					texture = textureA;
					passenger->textureA = textureA;
					passenger->textureTA = textureTA;
					passenger->path = station->businessDistrictPath;
					break;
				case EStationType::ResidentialArea:
					texture = textureB;
					passenger->textureA = textureB;
					passenger->textureTA = textureTB;
					passenger->path = station->residentialAreaPath;
					break;
				case EStationType::CulturalArea:
					texture = textureD;
					passenger->textureA = textureD;
					passenger->textureTA = textureTD;
					passenger->path = station->culturalAreaPath;
					break;
				case EStationType::RecreationalArea:
					texture = textureC;
					passenger->textureA = textureC;
					passenger->textureTA = textureTC;
					passenger->path = station->recreationalAreaPath;
					break;
				default:
					texture = textureA;
					passenger->textureA = textureA;
					passenger->textureTA = textureTA;
					break;
				}
				
				if (texture && passenger->passagerImage)
				{
					FSlateBrush brush; 
					brush.SetResourceObject(texture);
					brush.ImageSize = FVector2D(16, 32);
					passenger->passagerImage->SetBrush(brush);
				}
				station->horizontalBox->AddChildToHorizontalBox(passenger->passagerImage);
				
				station->AddPassenger(passenger);
			}
		}
	}
}

void ACxxLevelManager::CheckPassengerNumber()
{
	for(ACxxStation* station : stationsArray)
	{
		if (station->passengers.Num() > stationCapacity)
		{
			GameOver();
			break;
		}
	}
}

void ACxxLevelManager::ValidateRouteCompliance()
{
	if (dispatchManager && dispatchManager->bIsChanged)
	{
		dispatchManager->bIsChanged = false;
		// 检测合规性
		if (!dispatchManager->curSelectedRailLine) return;
		ACxxRailLine* rail = dispatchManager->curSelectedRailLine;

		// 找到端点(环线就任意取一个点
		UCxxPathControlPoint* startEndPoint = nullptr;
		if (rail->railSegments.Num() > 0)
		{
			startEndPoint = rail->railSegments[0]->startPoint;
			while (startEndPoint && !rail->bIsRingLine)
			{
				if (startEndPoint->preSegment)
				{
					startEndPoint = startEndPoint->preSegment->startPoint;
				}else
				{
					break;
				}
			}
		}
		if (!startEndPoint) return;
		UCxxPathControlPoint* curPoint = startEndPoint;
		
		TArray<FVector2D> lastThreePoints;
		bool bWhileEnd = false;
		bool bCurve = true;
		bool bStaiton = true;
		bool bIsLoop = false;
		while (curPoint &&
			curPoint->nextSegment &&
			!bWhileEnd)
		{
			if (curPoint == startEndPoint)
			{
				if (bIsLoop) break;
				bIsLoop = true;
			}
			
			float splineLength = curPoint->nextSegment->spline->GetSplineLength();
			int samplerNum = FMath::CeilToInt(splineLength / 325.f);
			for (float i = 0.0f; i < samplerNum; i += 1.0f)
			{
				FVector point = curPoint->nextSegment->spline->GetLocationAtDistanceAlongSpline(
					splineLength * i / static_cast<float>(samplerNum), ESplineCoordinateSpace::World);
				
				lastThreePoints.Add(FVector2D(point.X, point.Y));
				if (lastThreePoints.Num() > 3)
				{
					lastThreePoints.RemoveAt(0);
				}
				
				if (lastThreePoints.Num() == 3)
				{
					FVector p0(lastThreePoints[0], 300.f);
					FVector p1(lastThreePoints[1], 300.f);
					FVector p2(lastThreePoints[2], 300.f);
					
					FVector2D v1 = (lastThreePoints[0] - lastThreePoints[1]).GetSafeNormal();
					FVector2D v2 = (lastThreePoints[2] - lastThreePoints[1]).GetSafeNormal();
					float dot = FVector2D::DotProduct(v1, v2);
					dot = FMath::Clamp(dot, -1.f, 1.f);
					float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

					if (angle < MinRailAngle)
					{
						bWhileEnd = true;
						bCurve = false;
						break;
					}
				}
			}

			curPoint = curPoint->nextSegment->endPoint;
		}

		if (!bCurve || !bStaiton)
		{
			SetRailIsvaild(false);
			if(!bCurve) tagCurveInvaild->SetVisibility(ESlateVisibility::Visible);
		}else
		{
			SetRailIsvaild(true);
			tagCurveInvaild->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACxxLevelManager::SetRailIsvaild(bool bVaild)
{
	if (dispatchManager) dispatchManager->bRailIsVaild = bVaild;
}

bool ACxxLevelManager::GetRailIsvaild()
{
	if (dispatchManager) return dispatchManager->bRailIsVaild;
	return true;
}
