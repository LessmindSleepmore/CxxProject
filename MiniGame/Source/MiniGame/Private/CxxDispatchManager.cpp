// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxDispatchManager.h"
#include "CxxPlayerController.h"
#include "CxxMonoRailTrain.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

void ACxxDispatchManager::GenerateRailLineGraph()
{
	//BFS生成线路图 TODO:这里的逻辑可以优化
	for(auto station:stationArray)
	{
		for(auto type:stationType)
		{
			if(station->GetStationType() == type) continue;
			TArray<ACxxStation*>* pathArray = nullptr;
			
			switch (type)
			{
			case EStationType::BusinessDistrict:
				pathArray = &(station->businessDistrictPath);
				break;
			case EStationType::ResidentialArea:
				pathArray = &(station->residentialAreaPath);
				break;
			case EStationType::CulturalArea:
				pathArray = &(station->culturalAreaPath);
				break;
			case EStationType::RecreationalArea:
				pathArray = &(station->recreationalAreaPath);
				break;
			default:
				break;
			}

			if(!pathArray) break;
			if(!pathArray->IsEmpty()) pathArray->Empty();

			TArray<ACxxStation*> visited;
			TArray<ACxxStation*> queue;
			TMap<ACxxStation*, ACxxStation*> predecessors;
			visited.Add(station);
			queue.Add(station);
			while(!queue.IsEmpty())
			{
				ACxxStation* curStation = queue[0];
				queue.RemoveAt(0);
				if(curStation->GetStationType() == type)
				{
					// 如果找到了目标站点，保存路径
					TArray<ACxxStation*> path;
					ACxxStation* tStation = curStation;
					while (tStation)
					{
						pathArray->Insert(tStation, 0);
						tStation = predecessors.Find(tStation)?*predecessors.Find(tStation):nullptr;
					}
					break;
				}
				// 获取当前station的邻居station
				for(auto line:railLines)
				{
					if(!line->stations.Contains(curStation)) continue;
					for(auto neighbor:line->GetNeighborStations(curStation))
					{
						if(visited.Contains(neighbor)) continue;
						visited.Add(neighbor);
						queue.Add(neighbor);
						predecessors.Add(neighbor, curStation);
					}
				}
			}
		}
	}
}

// Sets default values
ACxxDispatchManager::ACxxDispatchManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 获取列车Actor
	// static ConstructorHelpers::FObjectFinder<UBlueprint> trainActorFinder(
	// TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Train/BP_CxxTrain.BP_CxxTrain'"));
	// if (trainActorFinder.Succeeded())
	// {
	// 	trainClass = trainActorFinder.Object->GeneratedClass;
	// }
	//
	// static ConstructorHelpers::FObjectFinder<UBlueprint> HubFinder(
	// TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/HUD/GameWinHUD.GameWinHUD'"));
	// if (HubFinder.Succeeded())
	// {
	// 	HubClass = HubFinder.Object->GeneratedClass;
	// }
}

void ACxxDispatchManager::AddScore(int s)
{
	score += s;
	todayScore += s;
	scoreText->SetText(FText::FromString(TEXT("客流量: ") + FString::FromInt(score) + TEXT(" k")));
}

int ACxxDispatchManager::GetScore()
{
	return score;
}

int ACxxDispatchManager::GetToDayScore()
{
	return todayScore;
}

void ACxxDispatchManager::HiddenUI()
{
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	addTrainCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	railEditCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	trainCreateButton->SetVisibility(ESlateVisibility::Hidden);
	railLineEditButton->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxDispatchManager::ShowGameStageUI()
{
	railLineEditButton->SetVisibility(ESlateVisibility::Hidden);
	trainCreateButton->SetVisibility(ESlateVisibility::Visible);
}

void ACxxDispatchManager::ShowEditStageUI()
{
	trainCreateButton->SetVisibility(ESlateVisibility::Hidden);
	railLineEditButton->SetVisibility(ESlateVisibility::Visible);
}

void ACxxDispatchManager::SplitSegment()
{
	if(!curSelectedSegment->startPoint || !curSelectedSegment->endPoint || !curSelectedRailLine)
	{
		return;
	}
	// 创建一个手动点，转到操作手动点的逻辑
	UCxxPathControlPoint* manualPoint = NewObject<UCxxPathControlPoint>(curSelectedRailLine);
	manualPoint->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
	manualPoint->RegisterComponent();
	curSelectedRailLine->controlPoints.Add(manualPoint);

	UCxxRailLineSegment* seg = NewObject<UCxxRailLineSegment>(curSelectedRailLine);
	seg->RegisterComponent();
	
	manualPoint->Init(FVector::Zero(), FVector(1,0,0), PointType::ManualPoint, curSelectedSegment, seg);
	curSelectedSegment->endPoint->SetSegment(seg, curSelectedSegment->endPoint->nextSegment);
	seg->Init(manualPoint, curSelectedSegment->endPoint, 1300, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
	curSelectedSegment->Init(curSelectedSegment->startPoint, manualPoint, 1300, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
	curSelectedRailLine->railSegments.Add(seg);

	curSelectedSegment = nullptr;
	curSelectedPoint = manualPoint;
	operatorName = "movement";
}

// Called when the game starts or when spawned
void ACxxDispatchManager::BeginPlay()
{
	Super::BeginPlay();
	// TestFuncGetAllStation();
	CreateHUD();
	bBeginPlay = true;
}

// Called every frame
void ACxxDispatchManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(bIsEditRail)
	{
		if (bIsAddTrain) bIsAddTrain = false;
		// 创建
		if(bIsCreateRail)
		{
			if (railNum > 0)
			{
				SwapRailLineActor(createStation, bCreateStationIsArrowDir);
				lastStation = createStation; // 避免直接被删除掉
				bIsSelectRail = true;
				operatorName = "movement";
				curSelectedStation = nullptr;
				// 设置编辑UI可见性,并设置线路名称
				editCanvasPanel->SetVisibility(ESlateVisibility::Visible);
				if(curSelectedRailLine && railNameTextBlock)
				{
					railNameTextBlock->SetText(FText::FromString(curSelectedRailLine->railLineName));
				}
				// 减少可用线路
				railNum--;
			}else
			{
				// TODO:提示没有可用线路 
				UE_LOG(LogTemp, Error, TEXT("no rail"));
			}
			bIsCreateRail = false;
		}
		else if(bIsSelectRail)
		{
			ModifyInputProcess();
			UpdateButtonAndIcon();
			// 如果选择站点
			if(curSelectedStation)
			{
				operatorName = "movement";
				if(curSelectedRailLine->bIsRingLine && curSelectedStation == curSelectedRailLine->ringLineStation)
				{
					curSelectedPoint = curSelectedRailLine->DeleteRingLineStation(curSelectedStation);
					ringLineIcon->SetVisibility(ESlateVisibility::Hidden);
					ringRingNum++;
				}else
				{
					curSelectedPoint = curSelectedRailLine->DeleteStation(curSelectedStation);	
				}
				lastStation = curSelectedStation;
				curSelectedStation = nullptr;
			}
			// 如果为轨道
			// 选择已有顶点还是添加新顶点
			// 拉到的位置为站点，添加新站点
			// 拉到的位置不为站点，则修改当前轨道
			else if(curSelectedSegment)
			{
				// 添加点并设置为手动点
				SplitSegment();
			}
			// 选择操作点
			else if(curSelectedPoint)
			{
				if(operatorName == "movement")
				{
					EditRailFromStation();
					if(curSelectedPoint && curSelectedPoint->GetPointType() == PointType::ManualPoint) curSelectedPoint->UpdateMousePointTick();
				}
				else if(operatorName.Contains("tangent"))
				{
					if(operatorName == "tangentA")
					{
						curSelectedPoint->UpdateMouseRotateTick(false);
					}else
					{
						curSelectedPoint->UpdateMouseRotateTick(true);
					}
				}else
				{
					UE_LOG(LogTemp, Warning, TEXT("【ERROR】operatorName is not valid!"));
				}
			}else
			{
				
			}
		}
		// 选择阶段
		else
		{
			HighLightHoverObject(true, true);
			InputProcess();
		}
	}
	
	if(bIsAddTrain)
	{
		HighLightHoverObject(true, false);
		// 处理按键
		MonitorAddTrainInput();
	}
}

void ACxxDispatchManager::SwapRailLineActor(ACxxStation* station, bool bIsArrowDir)
{
	if(curSelectedRailLine)
	{
		curSelectedRailLine->Destroy();
		curSelectedRailLine = nullptr;
	}
	
	curSelectedRailLine = GetWorld()->SpawnActor<ACxxRailLine>(ACxxRailLine::StaticClass());
	curSelectedRailLine->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
	railLines.Add(curSelectedRailLine);

	FString name = GetRailLineName();
	curSelectedRailLine->Init(name);
	curSelectedRailLine->bIsHighlighted = true;
	
	// 添加当前车站
	curSelectedPoint = curSelectedRailLine->AddStation(station, bIsArrowDir);
}

void ACxxDispatchManager::CreateTrainInRailLine(ACxxRailLine* railLine, bool bIsInit)
{
	// 创建一个列车
	if(trainClass)
	{
		FActorSpawnParameters spawnParams;
		ACxxMonoRailTrain* train = Cast<ACxxMonoRailTrain>(GetWorld()->SpawnActor<AActor>(trainClass,
			FVector::ZeroVector, FRotator::ZeroRotator, spawnParams));
		ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
		if(train)
		{
			train->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			train->Init(railLine, true, trainSpeed, trainOffSetRail, this, railLine->railSegments[0], rootPanel);
			
			railLine->trains.Add(train);
			trains.Add(train);
		}
		// 环线创建两辆车
		if(railLine->bIsRingLine && bIsInit)
		{
			ACxxMonoRailTrain* train2 = Cast<ACxxMonoRailTrain>(GetWorld()->SpawnActor<AActor>(trainClass,
				FVector::ZeroVector, FRotator::ZeroRotator, spawnParams));
			if(train2)
			{
				train2->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				train2->Init(railLine, false, trainSpeed, trainOffSetRail, this, railLine->railSegments[0], rootPanel);
				
				railLine->trains.Add(train2);
				trains.Add(train2);
			}
		}
	}
}

void ACxxDispatchManager::CreateTrainInAllRailine()
{
	for (auto rail:railLines)
	{
		CreateTrainInRailLine(rail, true);
	}
}

void ACxxDispatchManager::RemoveTrainInAllRailine()
{
	if (trains.IsEmpty()) return;
	for(auto train:trains)
	{
		if (IsValid(train) && train)
		{
			train->DestroyTrain();
		}
	}
	if(!trains.IsEmpty()) trains.Empty();

	for (auto line:railLines)
	{
		for (auto train:line->trains)
		{
			if (IsValid(train) && train)
			{
				train->DestroyTrain();
			}
		}
	}
}


FColor ACxxDispatchManager::GetRailColor()
{
	return FColor::MakeRandomColor();
}

void ACxxDispatchManager::CreateHUD()
{
	if(HubClass)
	{
		hudWidget = CreateWidget(GetWorld(), HubClass);
		if(hudWidget)
		{ 
			hudWidget->AddToViewport();
			trainCreateButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("TrainButton")));
			if (trainCreateButton)
			{
				trainCreateButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnTrainCreateButtonClicked);
				// trainCreateButton->OnReleased.AddDynamic(this, &ACxxDispatchManager::OnTrainCreateButtonReleased);
			}
			
			railLineEditButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("LineEditButton")));
			if (railLineEditButton)
			{
				railLineEditButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnEditRailLineButtonClicked);
			}

			pointDeleteButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("DeletePointButton")));
			if (pointDeleteButton)
			{
				pointDeleteButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnDeletePointButtonClicked);
			}
			
			rootPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("RootPanel")));
			addTrainCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("AddTrainCanvasPanel")));
			railEditCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("RailEditCanvasPanel")));
			scoreText = Cast<UTextBlock>(hudWidget->GetWidgetFromName(TEXT("Score")));
			railNameTextBlock = Cast<UTextBlock>(hudWidget->GetWidgetFromName(TEXT("RailLineName")));
			ringLineIcon = Cast<UImage>(hudWidget->GetWidgetFromName(TEXT("RingLineIcon")));
			ringLineIcon->SetVisibility(ESlateVisibility::Hidden);
			pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
			InitButton();
		}
	}
}

void ACxxDispatchManager::OnTrainCreateButtonClicked()
{
	if(!bIsAddTrain)
	{
		BeginIsAddTrain();
	}
	else
	{
		EndIsAddTrain();
	}
}

void ACxxDispatchManager::MonitorAddTrainInput()
{
	// 鼠标左键放置
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if(curSelectedRailLine)
		{
			if (trainNum > 0)
			{
				trainNum--;
				CreateTrainInRailLine(curSelectedRailLine, false);	
			}else
			{
				// TODO:提示没有列车
			}
		}
		else if (curSelectedTrain)
		{
			// 确保有至少一辆车
			if (curSelectedTrain->railLine->trains.Num() > 1)
			{
				// add delete tag
				curSelectedTrain->MarkTrainAsDelete();
			}else
			{
				// 提示不能删除
			}
		}
	}
}

void ACxxDispatchManager::HighLightHoverObject(bool bRailLine, bool bStation)
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY; 
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;

	if(bIsAddTrain && bRailLine && GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel5, cParams))
	{
		// 列车
		if(curSelectedRailLine != nullptr)
		{
			curSelectedRailLine->SwitchHightLightEffect(true);
		}
		curSelectedRailLine = nullptr;
		
		if (curSelectedTrain != nullptr)
		{
			curSelectedTrain->SwitchHightLightEffect();
		}
		curSelectedTrain = Cast<ACxxMonoRailTrain>(hitRes.GetActor());
		if (curSelectedTrain)
		{
			curSelectedTrain->SwitchHightLightEffect();
		}
	}else if (bStation && GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		StationChannel, cParams))
	{
		// 站点
		if (curSelectedTrain != nullptr)
		{
			curSelectedTrain->SwitchHightLightEffect();
		}
		curSelectedRailLine = nullptr;
		
		if(curSelectedRailLine != nullptr)
		{
			curSelectedRailLine->SwitchHightLightEffect(true);
		}
		curSelectedRailLine = nullptr;

		if(curSelectedStation != Cast<ACxxStation>(hitRes.GetActor()))
		{
			if(curSelectedStation != nullptr)
			{
				curSelectedStation->SwitchOutlineEffect();
			}
			curSelectedStation = Cast<ACxxStation>(hitRes.GetActor());
			if(curSelectedStation)
			{
				curSelectedStation->SwitchOutlineEffect();
			}
		}
	}
	else if (bRailLine && GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel2, cParams))
	{
		// 线路
		if(curSelectedStation != nullptr)
		{
			curSelectedStation->SwitchOutlineEffect();
		}
		curSelectedStation = nullptr;
		// 如果发生切换
		if(curSelectedRailLine != Cast<ACxxRailLine>(hitRes.GetActor()))
		{
			// 把上一个高亮的rail取消高亮
			if(curSelectedRailLine != nullptr)
			{
				curSelectedRailLine->SwitchHightLightEffect(!bIsAddTrain);
			}
			curSelectedRailLine = Cast<ACxxRailLine>(hitRes.GetActor());
			// 当前如果是rail则高亮
			if(curSelectedRailLine)
			{
				curSelectedRailLine->bIsHighlighted = false;
				curSelectedRailLine->SwitchHightLightEffect(!bIsAddTrain);
				// UE_LOG(LogTemp, Warning, TEXT("rail: %s"), *curSelectedRailLine->GetName());
			}
		}
	}
	else
	{
		if(curSelectedRailLine != nullptr)
		{
			curSelectedRailLine->SwitchHightLightEffect(true);
			curSelectedRailLine = nullptr;
		}
		if(curSelectedStation != nullptr)
		{
			curSelectedStation->SwitchOutlineEffect();
			curSelectedStation = nullptr;
		}
		if (curSelectedTrain != nullptr)
		{
			curSelectedTrain->SwitchHightLightEffect();
			curSelectedTrain = nullptr;
		}
	}
}

void ACxxDispatchManager::BeginIsAddTrain()
{
	if(bIsDrawRailLine) return;
	bIsAddTrain = true;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(0.05f);
	}
	addTrainCanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxDispatchManager::EndIsAddTrain()
{
	if(bIsDrawRailLine) return;
	bIsAddTrain = false;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(1.0f);
	}
	addTrainCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}

FString ACxxDispatchManager::GetRailLineName()
{
	FString name = TEXT("【线路数量超出名称数量上限】");
	for(int i = 1; i < 50; ++i)
	{
		bool bTFindName = false;
		for(auto railLine : railLines)
		{
			// 使用 TEXT() 宏确保正确编码
			if(railLine->railLineName == TEXT("深月轨道交通") + FString::FromInt(i) + TEXT("号线"))
			{
				bTFindName = true;
			}
		}
		if(!bTFindName)
		{
			// 使用 TEXT() 宏确保正确编码
			name = TEXT("深月轨道交通") + FString::FromInt(i) + TEXT("号线");
			break;
		}
	}
	return name;
}


void ACxxDispatchManager::OnEditRailLineButtonClicked()
{
	if (!bRailIsVaild) return;
	if(!bIsEditRail)
	{
		BeginEditRailLine();
	}else
	{
		EndEditRailLine();
	}
}

void ACxxDispatchManager::EditRailFromStation()
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		StationChannel, cParams))
	{
		ACxxStation* station = Cast<ACxxStation>(hitRes.GetActor());

		// 当前station切换才继续逻辑
		if(lastStation == station) return;
		lastStation = station;

		UE_LOG(LogTemp, Warning, TEXT("station: %s"), *station->GetName());

		// 添加新站
		if(station && !curSelectedRailLine->stations.Contains(station))
		{
			// 判断方向
			FString collsionName = hitRes.GetComponent()->GetName();
			if(curSelectedPoint && curSelectedRailLine) curSelectedRailLine->curSelectedPoint = curSelectedPoint;
			if(collsionName.Contains("mDir"))
			{
				curSelectedPoint = curSelectedRailLine->AddStation(station, false);
			}else
			{
				curSelectedPoint = curSelectedRailLine->AddStation(station, true);
			}
			station->isHighlighted = false;
			station->SwitchOutlineEffect();
		}
		// 已有站
		else if(station && curSelectedRailLine->stations.Contains(station))
		{
			auto point = curSelectedRailLine->GetPointFromStation(station);
			// 形成环线
			if(!curSelectedRailLine->bIsRingLine && curSelectedRailLine->stations.Num() > 1 &&
				((!point->preSegment && !curSelectedPoint->nextSegment) ||
				(!point->nextSegment && !curSelectedPoint->preSegment)))
			{
				// 变成环线
				curSelectedPoint = curSelectedRailLine->AddRingLineStation(station, curSelectedPoint, point);
				// 显示环线图标
				ringLineIcon->SetVisibility(ESlateVisibility::Visible);
				ringRingNum--;
			}
			// 删除站
			else
			{
				if(curSelectedRailLine->bIsRingLine && station == curSelectedRailLine->ringLineStation) return;
				if(curSelectedPoint && curSelectedRailLine) curSelectedRailLine->curSelectedPoint = curSelectedPoint;
				curSelectedPoint = curSelectedRailLine->DeleteStation(station);	
			}
		}
	}
	else
	{
		lastStation = nullptr;
	}
}

void ACxxDispatchManager::BeginEditRailLine()
{
	// TODO:可能需要初始化一下用到的变量
	bIsEditRail = true;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(0.05f);
	}
	railEditCanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxDispatchManager::EndEditRailLine()
{
	bIsEditRail = false;
	OnCancelButtonClicked();
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(1.0f);
	}
	railEditCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxDispatchManager::InputProcess()
{
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
		float mouseX, mouseY;
		cxxPC->GetMousePosition(mouseX, mouseY);
	
		FVector wLocation, wDirection;
		cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
		FHitResult hitRes;
		FCollisionQueryParams cParams;
	
		if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
			StationChannel, cParams))
		{
			// 站点
			bIsSelectRail = false;
			bIsCreateRail = true;
			createStation = Cast<ACxxStation>(hitRes.GetActor());
			
			// 获取拉出方向
			FString collsionName = hitRes.GetComponent()->GetName();
			if(collsionName.Contains("mDir"))
			{
				bCreateStationIsArrowDir = true;
			}else
			{
				bCreateStationIsArrowDir = false;
			}
		}
		else if(GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
			ECC_GameTraceChannel2, cParams))
		{
			// 轨道
			curSelectedRailLine = Cast<ACxxRailLine>(hitRes.GetActor());
			bIsSelectRail = true;
			bIsCreateRail = false;
			
			// 设置编辑UI可见性,并设置线路名称
			editCanvasPanel->SetVisibility(ESlateVisibility::Visible);
			if(curSelectedRailLine && railNameTextBlock)
			{
				railNameTextBlock->SetText(FText::FromString(curSelectedRailLine->railLineName));
				if (curSelectedRailLine->bIsRingLine)
				{
					ringLineIcon->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

void ACxxDispatchManager::InitButton()
{
	editCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("RailLineEditCanvasPanel")));
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	deleteButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("DeleteButton")));
	cancelButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("CancelButton")));
	// bind按钮
	BindButton();
}

void ACxxDispatchManager::OnCancelButtonClicked()
{
	// TODO:还需要取消其他数据.
	if (!bRailIsVaild) return;
	bIsSelectRail = false;
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
	ringLineIcon->SetVisibility(ESlateVisibility::Hidden);

	if(curSelectedRailLine)
	{
		curSelectedRailLine->SwitchHightLightEffect(true);
		curSelectedRailLine->curSelectedPoint = nullptr;
		curSelectedRailLine = nullptr;
	}

	curSelectedStation = nullptr;
	curSelectedSegment = nullptr;
}

void ACxxDispatchManager::OnDeleteButtonClicked()
{
	bIsSelectRail = false;
	bRailIsVaild = true;
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
	ringLineIcon->SetVisibility(ESlateVisibility::Hidden);
	
	if(curSelectedRailLine)
	{
		curSelectedRailLine->SwitchHightLightEffect(true);
		railLines.Remove(curSelectedRailLine);
		railNum++;
		if (curSelectedRailLine->bIsRingLine) ringRingNum++;
		curSelectedRailLine->Destroy();
		curSelectedRailLine = nullptr;
	}
}

void ACxxDispatchManager::OnDeletePointButtonClicked()
{
	if (lastSelectedPoint && curSelectedRailLine)
	{
		curSelectedRailLine->controlPoints.Remove(lastSelectedPoint);
		if (lastSelectedPoint->preSegment) curSelectedRailLine->railSegments.Remove(lastSelectedPoint->preSegment);
		lastSelectedPoint->Delete();
	}
	pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
	bIsChanged = true;
}

void ACxxDispatchManager::UpdateButtonAndIcon()
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	if(pointDeleteButton && lastSelectedPoint)
	{
		FVector wLocation = lastSelectedPoint->location;
		FVector2D sPosition;
			
		if (cxxPC && cxxPC->ProjectWorldLocationToScreen(wLocation, sPosition))
		{
			if (UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(pointDeleteButton->Slot))
			{
				slot->SetPosition(sPosition);
			}
		}	
	}

	if(curSelectedRailLine && curSelectedRailLine->ringLineStation && ringLineIcon &&
		ringLineIcon->GetVisibility() == ESlateVisibility::Visible)
	{
		FVector stationLoc = curSelectedRailLine->ringLineStation->GetActorLocation();
		FVector2D stationScreenLoc;
			
		if (cxxPC && cxxPC->ProjectWorldLocationToScreen(stationLoc, stationScreenLoc))
		{
			if (UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(ringLineIcon->Slot))
			{
				slot->SetPosition(stationScreenLoc);
			}
		}
	}
}

void ACxxDispatchManager::BindButton()
{
	if(deleteButton)
	{
		deleteButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnDeleteButtonClicked);
	}
	if(cancelButton)
	{
		cancelButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnCancelButtonClicked);
	}
}

void ACxxDispatchManager::EnableEditView()
{
	if(!curSelectedRailLine)
	{
		UE_LOG(LogTemp, Warning, TEXT("【ERROR】curSelectedRailLine is nullptr"));
		return;
	}

	// TODO:这里加上某个滤镜
	// 绘制视图状态的轨道(目前就是线)
	bIsEditView = true;
}

void ACxxDispatchManager::DisableEditView()
{
	bIsEditView = false;
}

void ACxxDispatchManager::ModifyInputProcess()
{
	// 按住编辑该对象
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		curSelectedStation = nullptr;
		curSelectedSegment = nullptr;
		curSelectedRailLine->curSelectedPoint = nullptr;
		lastStation = nullptr;
		GetRailLineObject();
		pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if(GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if(curSelectedRailLine->stations.Num() != 1){
			if(curSelectedPoint && curSelectedRailLine) curSelectedRailLine->curSelectedPoint = curSelectedPoint;
			curSelectedRailLine->FinishedEditRailLineFromStation(); // 结束编辑生成轨道实例
			curSelectedRailLine->curSelectedPoint = nullptr;
			// 准备进行线路合规性检测
			bIsChanged = true;
		}else
		{
			curSelectedRailLine->Destroy();
			curSelectedRailLine->SwitchHightLightEffect(true);
			if (railLines.Contains(curSelectedRailLine))
			{
				railLines.Remove(curSelectedRailLine);
				if (curSelectedRailLine->bIsRingLine) ringRingNum++;
				railNum++;
			}
			curSelectedRailLine = nullptr;
			OnCancelButtonClicked();
			bIsSelectRail = false;
			ringLineIcon->SetVisibility(ESlateVisibility::Hidden);
			pointDeleteButton->SetVisibility(ESlateVisibility::Hidden);
			curSelectedPoint = nullptr;
		}
		curSelectedStation = nullptr;
		if(curSelectedPoint) curSelectedPoint->ResetTangentOperatorParam();
		lastSelectedPoint = nullptr;
		// 设置删除节点按钮
		if (curSelectedPoint && curSelectedPoint->GetPointType() == PointType::ManualPoint &&
			curSelectedPoint->preSegment && curSelectedPoint->nextSegment)
		{
			lastSelectedPoint = curSelectedPoint;
			pointDeleteButton->SetVisibility(ESlateVisibility::Visible);
		}
		curSelectedPoint = nullptr;
		curSelectedSegment = nullptr;
	}
} 

void ACxxDispatchManager::GetRailLineObject()
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel4, cParams))
	{
		// 操作点
		// 获取到当前actor中的ControlPoint对象以及操作的方式
		USceneComponent* hitComponent = hitRes.GetComponent();
		FString componentName = hitComponent->GetName();
		FString pointComponentName;

		int32 underscoreIndex = INDEX_NONE;
		componentName.FindLastChar('_', underscoreIndex);
		if (underscoreIndex != INDEX_NONE)
		{
			pointComponentName = componentName.Left(underscoreIndex);
			operatorName = componentName.Mid(underscoreIndex + 1);
		}
		else
		{
			pointComponentName = componentName;
			operatorName = TEXT("");
		}
		TArray<UActorComponent*> pointComponents = hitComponent->GetOwner()->GetComponentsByClass(UCxxPathControlPoint::StaticClass());
		for(auto comp:pointComponents)
		{
			FString compName = comp->GetName();
			if(compName == pointComponentName)
			{
				curSelectedPoint = Cast<UCxxPathControlPoint>(comp);
				// 输出pointComponentName和operatorName
				UE_LOG(LogTemp, Warning, TEXT("pointComponentName: %s, operatorName: %s"), *pointComponentName, *operatorName);
			}
		}
	}
	else if(GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		StationChannel, cParams))
	{
		// 站点
		if(curSelectedRailLine->stations.Contains(Cast<ACxxStation>(hitRes.GetActor())))
		{
			curSelectedStation = Cast<ACxxStation>(hitRes.GetActor());
		}
		curSelectedRailLine->curSelectedPoint = curSelectedRailLine->GetPointFromStation(curSelectedStation);
	}
	else if(GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel2, cParams))
	{
		// 轨道
		UCxxRailLineSegment* seg = curSelectedRailLine->GetSegmentFromMesh(Cast<USplineMeshComponent>(hitRes.GetComponent()));
		
		if(curSelectedRailLine->railSegments.Contains(seg))
		{
			if(seg->startPoint->type == PointType::StationPoint ||
				seg->endPoint->type == PointType::StationPoint)
			{
				return;
			}
			curSelectedSegment = seg;
		}
	}
}