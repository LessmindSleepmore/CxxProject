// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxDispatchManager.h"
#include "CxxPlayerController.h"
#include "CxxMonoRailTrain.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACxxDispatchManager::ACxxDispatchManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 获取列车Actor
	static ConstructorHelpers::FObjectFinder<UBlueprint> trainActorFinder(
	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Train/BP_CxxTrain.BP_CxxTrain'"));
	if (trainActorFinder.Succeeded())
	{
		trainClass = trainActorFinder.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> HubFinder(
	TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Assets/HUD/GameWinHUD.GameWinHUD'"));
	if (HubFinder.Succeeded())
	{
		HubClass = HubFinder.Object->GeneratedClass;
	}
}

void ACxxDispatchManager::AddScore(int s)
{
	score += s;
	scoreText->SetText(FText::FromString("Score: " + FString::FromInt(score)));
}

// Called when the game starts or when spawned
void ACxxDispatchManager::BeginPlay()
{
	Super::BeginPlay();
	TestFuncGetAllStation();
	CreateHUD();
}

// Called every frame
void ACxxDispatchManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(bIsDrawRailLine)
	// {
	// 	UpdataTickOperate();
	// }
	CreateCxxPassenger(DeltaTime);
	// if(bIsSelectRail)
	// {
	// 	UpdateCurCollisionRail();
	// 	MonitorEditRailLineInput();
	// }
	
	if(bIsEditRail)
	{
		UpdataMouseLocation();
		// 创建
		if(bIsCreateRail)
		{
			if(!bInitRailLine)
			{
				SwapRailLineActor(createStation, bCreateStationIsArrowDir);
				lastStation = createStation; // 避免直接被删除掉
			}
			EditRailFromStation();
			CreateInputProcess();
		}
		else if(bIsSelectRail)
		{
			UpdataButtonLocation();
			// ModifyInputProcess();
			// // 如果选择站点
			// if(curSelectedStation)
			// {
			// 	// // 初始化一下当前段
			// 	// stationIdx = curSelectedRailLine->stations.Find(curSelectedStation);
			// 	// int idx = curSelectedRailLine->stations.Find(curSelectedStation);
			// 	// curEditSegment = idx == 0?curSelectedRailLine->railSegments[0]:curSelectedRailLine->railSegments[idx - 1];
			// 	EditRailFromStation();
			// 	
			// }
			// // 如果为轨道
			// // 选择已有顶点还是添加新顶点
			// // 拉到的位置为站点，添加新站点
			// // 拉到的位置不为站点，则修改当前轨道
			// if(curSelectedSegment)
			// {
			// 	
			// }
		}
		// 选择阶段
		else
		{
			HighLightHoverObject(true, true);
			InputProcess();
		}

		// 更新当前段
		if(curSelectedRailLine && curSelectedSegment && mouseLocation != FVector::ZeroVector)
		{
			curSelectedRailLine->UpdateRailLineSegment(curSelectedSegment, mouseLocation);
			// curSelectedRailLine->UpdateRailLine();
			// curSelectedSegment name
			// UE_LOG(LogTemp, Warning, TEXT("curSelectedSegment: %s"), *curSelectedSegment->GetName());
		}
		ShowEditViewRailLine();
	}
	
	if(bIsAddTrain)
	{
		HighLightHoverObject(true, false);
		// 处理按键
		MonitorAddTrainInput();
	}

	/////////////////////////////////////////////////////// DEBUG ///////////////////////////////////////////////////////
	// 输出bIsAddTrain状态
	// UE_LOG(LogTemp, Warning, TEXT("bIsAddTrain: %d"), bIsAddTrain);
}


// void ACxxDispatchManager::StartCreateRailLine()
// {
// 	if(bIsAddTrain) return;
// 	
// 	// 获取PlayerController
// 	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
// 	float mouseX, mouseY;
// 	cxxPC->GetMousePosition(mouseX, mouseY);
// 	
// 	FVector wLocation, wDirection;
// 	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
// 	
// 	FHitResult hitRes;
// 	FCollisionQueryParams cParams;
// 	cParams.AddIgnoredActor(this);
// 	
// 	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
// 		StationChannel, cParams))
// 	{
// 		// 加入空的stations列表中
// 		ACxxStation* station = Cast<ACxxStation>(hitRes.GetActor());
// 		SwapRailLineActor(station);
// 		if(!curGeneRailLine->stations.IsEmpty()) UE_LOG(LogTemp, Warning, TEXT("【ERROR】stations is not empty!"));
// 		
// 		curGeneRailLine->stations.Empty();
// 		curGeneRailLine->stations.Add(station);
// 		bIsDrawRailLine = true;
// 		station->SwitchOutlineEffect();
// 	}
// }

// void ACxxDispatchManager::EndCreateRailLine()
// {
// 	if(bIsAddTrain) return;
// 	
// 	if(bIsDrawRailLine)
// 	{
// 		bIsDrawRailLine = false;
// 		for(ACxxStation* station : curGeneRailLine->stations)
// 		{
// 			station->SwitchOutlineEffect();
// 		}
// 		// 创建成功
// 		if(curGeneRailLine->stations.Num() != 1){
// 			if(!curGeneRailLine->railSegments.Last()->bEndIsStation)
// 			{
// 				UCxxRailSegment* segment = curGeneRailLine->railSegments.Top();
// 				curGeneRailLine->railSegments.Pop();
// 				segment->MarkAsGarbage();
// 				segment = nullptr;
// 			}
// 			railLines.Add(curGeneRailLine);
// 			curGeneRailLine->FinishEdit(); // 结束编辑生成轨道实例
// 			CreateTrainInRailLine(curGeneRailLine);  // 创建列车
// 			curGeneRailLine = nullptr;
// 		}else
// 		{
// 			curGeneRailLine->Destroy();
// 			curGeneRailLine = nullptr;
// 		}
// 	}
// }

void ACxxDispatchManager::SwapRailLineActor(ACxxStation* station, bool bIsArrowDir)
{
	if(curSelectedRailLine)
	{
		curSelectedRailLine->Destroy();
		curSelectedRailLine = nullptr;
	}
	
	curSelectedRailLine = GetWorld()->SpawnActor<ACxxRailLine>(ACxxRailLine::StaticClass());

	// 设置线路颜色
	FColor railColor = GetRailColor();
	curSelectedRailLine->SetRailColor(railColor);
	curSelectedRailLine->Init();

	// 添加新段
	UCxxRailSegment* newSeg1 = NewObject<UCxxRailSegment>(curSelectedRailLine);
	UCxxRailSegment* newSeg2 = NewObject<UCxxRailSegment>(curSelectedRailLine);
	newSeg1->init(nullptr, station);
	newSeg2->init(station, nullptr);
	curSelectedRailLine->railSegments.Add(newSeg1);
	curSelectedRailLine->railSegments.Add(newSeg2);
	curSelectedSegment = newSeg2;

	// 添加当前车站
	curSelectedRailLine->AddStation(station, 0, bIsArrowDir);
	// station->SwitchOutlineEffect();

	bInitRailLine = true;
}

// void ACxxDispatchManager::UpdataTickOperate()
// {
// 	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
// 	float mouseX, mouseY;
// 	cxxPC->GetMousePosition(mouseX, mouseY);
// 	
// 	FVector wLocation, wDirection;
// 	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
// 	
// 	FHitResult hitRes;
// 	FCollisionQueryParams cParams;
// 	
// 	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
// 		StationChannel, cParams))
// 	{
// 		ACxxStation* station = Cast<ACxxStation>(hitRes.GetActor());
//
// 		// 当前station切换才继续逻辑
// 		if(lastStation == station) return;
// 		lastStation = station;
//
// 		UE_LOG(LogTemp, Warning, TEXT("station: %s"), *station->GetName());
// 		
// 		if(station && !curGeneRailLine->stations.Contains(station))
// 		{
// 			// 添加新站
// 			curGeneRailLine->stations.Add(station);
// 			station->SwitchOutlineEffect();
// 			
// 			// 修改最后一个段的状态,并更新为车站的连接方式
// 			curGeneRailLine->railSegments.Last()->stationB = station;
// 			curGeneRailLine->railSegments.Last()->bEndIsStation = true;
// 			curGeneRailLine->UpdataSegment(curGeneRailLine->railSegments.Last());
//
// 			// 添加一个新段
// 			UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(curGeneRailLine);
// 			newSeg->init(station);
// 			curGeneRailLine->railSegments.Add(newSeg);
// 		}else if(station && curGeneRailLine->stations.Contains(station))
// 		{
// 			// 如果是最后一个站非起始站则移除
// 			if(curGeneRailLine->stations.Num() >= 2 && curGeneRailLine->stations.Last() == station)
// 			{
// 				ACxxStation* popStation = curGeneRailLine->stations.Pop();
// 				popStation->SwitchOutlineEffect();
// 				popStation = nullptr;
//
// 				UCxxRailSegment* segment = curGeneRailLine->railSegments.Top();
// 				curGeneRailLine->railSegments.Pop();
// 				segment->MarkAsGarbage();
// 				segment = nullptr;
// 				
// 				curGeneRailLine->railSegments.Last()->bEndIsStation = false;
// 				curGeneRailLine->railSegments.Last()->stationB = nullptr;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		lastStation = nullptr;
// 	}
// }

void ACxxDispatchManager::CreateTrainInRailLine(ACxxRailLine* railLine)
{
	// 创建一个列车
	if(trainClass)
	{
		FActorSpawnParameters spawnParams;
		ACxxMonoRailTrain* train = Cast<ACxxMonoRailTrain>(GetWorld()->SpawnActor<AActor>(trainClass,
			FVector::ZeroVector, FRotator::ZeroRotator, spawnParams));
		if(train)
		{
			train->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			train->Init(railLine, true, trainSpeed, trainOffSetRail, this);
			railLine->trains.Add(train);
			trains.Add(train);
		}
	}
}

void ACxxDispatchManager::CreateCxxPassenger(float DeltaTime)
{
	// 每隔一段1/60s随机生成一次乘客
	passengerTimer += DeltaTime;
	if(passengerTimer < 1/60.f) return;
	passengerTimer = 0.f;
	
	// 每个车站随机生成乘客
	for(ACxxStation* station : stations)
	{
		int randNum = FMath::RandRange(0, 999);
		if(randNum > 996)
		{
			// 随机生成一个不是当前站EStationType的EStationType
			EStationType randType = static_cast<EStationType>(FMath::RandRange(0, 3));
			while(randType == station->GetStationType())
			{
				randType = static_cast<EStationType>(FMath::RandRange(0, 3));
			}
			// 创建一个CxxPassenger对象
			ACxxPassenger* passenger = GetWorld()->SpawnActor<ACxxPassenger>(ACxxPassenger::StaticClass());
			if(passenger)
			{
				passenger->AttachToActor(station, FAttachmentTransformRules::KeepRelativeTransform);
				passenger->targetStationType = randType;
				station->AddPassenger(passenger);
			}
		}
	}
}

void ACxxDispatchManager::TestFuncGetAllStation()
{
	TArray<AActor*> allStation;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxStation::StaticClass(), allStation);
	for(AActor* station : allStation)
	{
		stations.Add(Cast<ACxxStation>(station));
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
			UButton* trainCreateButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("TrainButton")));
			if (trainCreateButton)
			{
				trainCreateButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnTrainCreateButtonClicked);
				// trainCreateButton->OnReleased.AddDynamic(this, &ACxxDispatchManager::OnTrainCreateButtonReleased);
			}
			
			UButton* railLineEditButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("LineEditButton")));
			if (railLineEditButton)
			{
				railLineEditButton->OnClicked.AddDynamic(this, &ACxxDispatchManager::OnEditRailLineButtonClicked);
			}

			addTrainCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("AddTrainCanvasPanel")));
			railEditCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("RailEditCanvasPanel")));
			scoreText = Cast<UTextBlock>(hudWidget->GetWidgetFromName(TEXT("Score")));
			// // 设置UMG和GameMode的输入模式，否则按键的时候做不了射线检测 TODO:还是没能解决，按下按钮的时候，UMG会吃掉射线检测.
			// ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
			// FInputModeGameAndUI InputMode;
			// cxxPC->SetInputMode(InputMode);
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

// void ACxxDispatchManager::OnTrainCreateButtonReleased()
// {
// 	EndIsAddTrain();
// }

void ACxxDispatchManager::MonitorAddTrainInput()
{
	// 鼠标左键放置
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if(curSelectedRailLine)
		{
			CreateTrainInRailLine(curSelectedRailLine);
		}
		else
		{
			// 提示不能添加列车
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

	if(bStation && GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		StationChannel, cParams))
	{
		if(curSelectedRailLine != nullptr)
		{
			curSelectedRailLine->SwitchHightLightEffect();
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
	else if (bRailLine && GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		ECC_GameTraceChannel2, cParams))
	{
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
				curSelectedRailLine->SwitchHightLightEffect();
			}
			curSelectedRailLine = Cast<ACxxRailLine>(hitRes.GetActor());
			// 当前如果是rail则高亮
			if(curSelectedRailLine)
			{
				curSelectedRailLine->SwitchHightLightEffect();
				// UE_LOG(LogTemp, Warning, TEXT("rail: %s"), *curSelectedRailLine->GetName());
			}
		}
	}else
	{
		if(curSelectedRailLine != nullptr)
		{
			curSelectedRailLine->SwitchHightLightEffect();
			curSelectedRailLine = nullptr;
		}
		if(curSelectedStation != nullptr)
		{
			curSelectedStation->SwitchOutlineEffect();
			curSelectedStation = nullptr;
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
		World->GetWorldSettings()->SetTimeDilation(0.2f);
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

void ACxxDispatchManager::CreateInputProcess()
{
	if(GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if(bIsCreateRail)
		{
			for(ACxxStation* station : curSelectedRailLine->stations)
			{
				station->SwitchOutlineEffect();
			}
			// 创建成功
			if(curSelectedRailLine->stations.Num() != 1){
				railLines.Add(curSelectedRailLine);
				curSelectedRailLine->FinishEdit(); // 结束编辑生成轨道实例
				// CreateTrainInRailLine(curSelectedRailLine);  // 创建列车
			}else
			{
				curSelectedRailLine->Destroy();
			};
			curSelectedRailLine = nullptr;
			curSelectedStation = nullptr;
			createStation = nullptr;
			bIsCreateRail = false;
			bInitRailLine = false;
		}
	}
}

void ACxxDispatchManager::OnEditRailLineButtonClicked()
{
	if(!bIsEditRail)
	{
		BeginEditRailLine();
	}else
	{
		EndEditRailLine();
	}
}

void ACxxDispatchManager::MonitorEditRailLineInput()
{
	// 再次按下按钮关闭
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::C))
	{
		EndEditRailLine();
		// TODO
	}
	// 鼠标左键开始编辑
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		if(curSelectedRailLine)
		{
			// 停止选择
			// EndSelectRailLine();
			EnableEditView();
		}
		else
		{
			// TODO:提示未选中轨道
		}
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
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
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
			int idx = curSelectedRailLine->railSegments.Find(curSelectedSegment);
			if(idx == -1)
			{
				return;
			}

			// 判断方向
			FString collsionName = hitRes.GetComponent()->GetName();
			if(collsionName.Contains("mDir"))
			{
				curSelectedRailLine->AddStation(station, idx, false);
			}else
			{
				curSelectedRailLine->AddStation(station, idx, true);
			}
			// 写一个函数处理，这样在修改前都会updata一下
			// curSelectedRailLine->UpdateRailLine();
			curSelectedSegment = curSelectedRailLine->railSegments[idx + 1];
			station->SwitchOutlineEffect();
		}
		// 如果当前大于一个站且为当前段的左右段才可以移除
		else if(station && curSelectedRailLine->stations.Contains(station))
		{
			if( (curSelectedSegment->stationA == station || curSelectedSegment->stationB == station) &&
				curSelectedRailLine->stations.Num() >= 2)
			{
				int idx = curSelectedRailLine->DeleteStation(station);
				// curSelectedRailLine->UpdateRailLine();
				curSelectedSegment = curSelectedRailLine->railSegments[idx];
				station->SwitchOutlineEffect();
			}
		}
	}
	else
	{
		lastStation = nullptr;
	}
}

void ACxxDispatchManager::UpdataMouseLocation()
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		ECC_GameTraceChannel3, cParams))
	{
		mouseLocation = hitRes.Location;
	}else
	{
		mouseLocation = FVector::ZeroVector;
	}

	// 画一个debug sphere
	// DrawDebugSphere(GetWorld(), mouseLocation, 20.0f, 8, FColor::Red, false, 0.1f);
}

void ACxxDispatchManager::CreateRailFromStation(ACxxStation* station)
{
	// 创建一个railline
	// SwapRailLineActor(station, );
}

void ACxxDispatchManager::BeginEditRailLine()
{
	// TODO:可能需要初始化一下用到的变量
	bIsEditRail = true;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(0.2f);
	}
	railEditCanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void ACxxDispatchManager::EndEditRailLine()
{
	bIsEditRail = false;
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetWorldSettings()->SetTimeDilation(1.0f);
	}
	railEditCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}

// void ACxxDispatchManager::BeginSelectRailLine()
// {
// 	bIsSelectRail = true;
// }
//
// void ACxxDispatchManager::EndSelectRailLine()
// {
// 	bIsSelectRail = false;
// }

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
	
		if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
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
		else if(GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
			ECC_GameTraceChannel2, cParams))
		{
			// 轨道
			curSelectedRailLine = Cast<ACxxRailLine>(hitRes.GetActor());
			bIsSelectRail = true;
			bIsCreateRail = false;
			
			// 设置编辑UI可见性
			editCanvasPanel->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ACxxDispatchManager::UpdataButtonLocation()
{
	if(curSelectedRailLine && editCanvasPanel)
	{
		ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	
		FVector railLineLocation = curSelectedRailLine->stations[0]->GetActorLocation();

		FVector2D screenPos;
		cxxPC->ProjectWorldLocationToScreen(railLineLocation, screenPos);

		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(editCanvasPanel->Slot);
		if(slot)
		{
			// 向上偏移整个组件的高度，向左偏移半个组件的长度
			screenPos.Y -= slot->GetSize().Y + 100.0;
			screenPos.X -= slot->GetSize().X / 2.0;
			
			slot->SetPosition(screenPos);
			// UE_LOG(LogTemp, Warning, TEXT("screenPos: %s"), *screenPos.ToString());
		}
	}
}

void ACxxDispatchManager::InitButton()
{
	editCanvasPanel = Cast<UCanvasPanel>(hudWidget->GetWidgetFromName(TEXT("RailLineEditCanvasPanel")));
	deleteButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("DeleteButton")));
	cancelButton = Cast<UButton>(hudWidget->GetWidgetFromName(TEXT("CancelButton")));
	// bind按钮
	BindButton();
}

void ACxxDispatchManager::OnCancelButtonClicked()
{
	// TODO:还需要取消其他数据.
	bIsSelectRail = false;
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);

	if(curSelectedRailLine)
	{
		curSelectedRailLine->SwitchHightLightEffect();
		curSelectedRailLine = nullptr;
	}
}

void ACxxDispatchManager::OnDeleteButtonClicked()
{
	bIsSelectRail = false;
	editCanvasPanel->SetVisibility(ESlateVisibility::Hidden);
	if(curSelectedRailLine)
	{
		curSelectedRailLine->SwitchHightLightEffect();
		curSelectedRailLine->Destroy();
		curSelectedRailLine = nullptr;
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

void ACxxDispatchManager::ShowEditViewRailLine()
{
	if(curSelectedRailLine)
	{
		for(auto seg : curSelectedRailLine->railSegments)
		{
			for(int i = 0; i < seg->points.Num(); i++)
			{
				if(i == seg->points.Num() - 1)
				{
					break;
				}
				DrawDebugLine(GetWorld(), seg->points[i], seg->points[i + 1], FColor::Green, false, 0.f, 0, 10.f);
				// if(seg->pointType[i] == 1)
				// {
				// 	DrawDebugBox(GetWorld(), seg->points[i], FVector(10, 10, 10), FColor::Red, false, 0.f, 0, 10.f);
				// }
			}
		}
	}
}

void ACxxDispatchManager::ModifyInputProcess()
{
	// 如果按下鼠标左键
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		curSelectedStation = nullptr;
		curSelectedSegment = nullptr;
		GetRailLineStationOrSegment();
	}
	
	if(GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		curSelectedStation = nullptr;
		curSelectedSegment = nullptr;
	}
}

void ACxxDispatchManager::GetRailLineStationOrSegment()
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		StationChannel, cParams))
	{
		// 站点
		if(curSelectedRailLine->stations.Contains(Cast<ACxxStation>(hitRes.GetActor())))
		{
			curSelectedStation = Cast<ACxxStation>(hitRes.GetActor());
		}
	}
	else if(GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		ECC_GameTraceChannel2, cParams))
	{
		// 轨道
		// if( Cast<ACxxRailLine>(hitRes.GetActor()) == curSelectedRailLine &&
		// 	curSelectedRailLine->meshCompName2Seg.Contains(Cast<UStaticMeshComponent>(hitRes.GetComponent())))
		// {
		// 	curSelectedSegment = curSelectedRailLine->meshCompName2Seg[hitRes.GetComponent()->GetName()];
		// }
	}
}
