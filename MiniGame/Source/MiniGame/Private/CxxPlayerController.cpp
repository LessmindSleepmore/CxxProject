// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxPlayerController.h"

#include "Kismet/GameplayStatics.h"

void ACxxPlayerController::BeginPlay()
{
	Super::BeginPlay();
	this->SetMouseVisable();
	dispatchManager = Cast<ACxxDispatchManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACxxDispatchManager::StaticClass()));
	if(dispatchManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Find DispatchManager"));
		bGetDispatchManager = true;
	}
}

void ACxxPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// if (bDrawRailLine)
	// {
	// 	CheckMouseOnStation();
	// }
}

void ACxxPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	
	// InputComponent->BindAction("CheckMouseInActor", IE_Pressed, this, &ACxxPlayerController::OnLeftMousePressed);
	// InputComponent->BindAction("CheckMouseInActor", IE_Released, this, &ACxxPlayerController::OnLeftMouseReleased);
}

void ACxxPlayerController::SetMouseVisable()
{
	FInputModeGameAndUI inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	inputMode.SetHideCursorDuringCapture(false);
	this->SetInputMode(inputMode);
	this->bShowMouseCursor = true;
}

void ACxxPlayerController::OnLeftMousePressed()
{
	if(bGetDispatchManager)
	{
		// dispatchManager->StartCreateRailLine();

	}
	
	// float mouseX, mouseY;
	// this->GetMousePosition(mouseX, mouseY);
	//
	// FVector wLocation, wDirection;
	// this->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	//
	// FHitResult hitRes;
	// FCollisionQueryParams cParams;
	// cParams.AddIgnoredActor(this);
	//
	// if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
	// 	StationChannel, cParams))
	// {
	// 	// 加入空的stations列表中
	// 	AStation* station = Cast<AStation>(hitRes.GetActor());
	// 	SwapRailLine(station);
	// 	if(!railLine->stations.IsEmpty()) UE_LOG(LogTemp, Warning, TEXT("【ERROR】stations is not empty!"));
	// 	
	// 	railLine->stations.Empty();
	// 	railLine->stations.Add(station);
	// 	bDrawRailLine = true;
	// 	station->SwitchOutlineEffect();
	// }
}

void ACxxPlayerController::OnLeftMouseReleased()
{
	if(bGetDispatchManager)
	{
		// dispatchManager->EndCreateRailLine();
	}
	// if(bDrawRailLine)
	// {
	// 	bDrawRailLine = false;
	//     for(AStation* station : railLine->stations)
	// 	{
	// 		station->SwitchOutlineEffect();
	// 	}
	// 	if(railLine->stations.Num() != 1){
	// 		EndRailLineDraw(true);
	// 	}else
	// 	{
	// 		EndRailLineDraw(false);
	// 	}
	// }
}

// void ACxxPlayerController::CheckMouseOnStation()
// {
// 	float mouseX, mouseY;
// 	this->GetMousePosition(mouseX, mouseY);
// 	
// 	FVector wLocation, wDirection;
// 	this->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
// 	
// 	FHitResult hitRes;
// 	FCollisionQueryParams cParams;
// 	cParams.AddIgnoredActor(this);;
// 	
// 	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
// 		StationChannel, cParams))
// 	{
// 		AStation* station = Cast<AStation>(hitRes.GetActor());
//
// 		// 当前station切换才继续逻辑
// 		if(lastStation == station) return;
// 		lastStation = station;
//
// 		UE_LOG(LogTemp, Warning, TEXT("station: %s"), *station->GetName());
// 		
// 		if(station && !railLine->stations.Contains(station))
// 		{
// 			// 添加新站
// 			railLine->stations.Add(station);
// 			station->SwitchOutlineEffect();
// 			
// 			// 修改最后一个段的状态,并更新为车站的连接方式
// 			railLine->railSegments.Last()->stationB = station;
// 			railLine->railSegments.Last()->bEndIsStation = true;
// 			railLine->UpdataSegment(railLine->railSegments.Last());
//
// 			// 添加一个新段
// 			URailSegment* newSeg = NewObject<URailSegment>(railLine);
// 			newSeg->init(station);
// 			railLine->railSegments.Add(newSeg);
// 		}else if(station && railLine->stations.Contains(station))
// 		{
// 			// 如果是最后一个站非起始站则移除
// 			if(railLine->stations.Num() >= 2 && railLine->stations.Last() == station)
// 			{
// 				AStation* popStation = railLine->stations.Pop();
// 				popStation->SwitchOutlineEffect();
// 				popStation = nullptr;
//
// 				URailSegment* segment = railLine->railSegments.Top();
// 				railLine->railSegments.Pop();
// 				segment->MarkAsGarbage();
// 				segment = nullptr;
// 				
// 				railLine->railSegments.Last()->bEndIsStation = false;
// 				railLine->railSegments.Last()->stationB = nullptr;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		lastStation = nullptr;
// 	}
// }

// void ACxxPlayerController::SwapRailLine(AStation* station)
// {
// 	if(railLine)
// 	{
// 		railLine->Destroy();
// 		railLine = nullptr;
// 	}
//
// 	
// 	railLine = GetWorld()->SpawnActor<ARailLine>(ARailLine::StaticClass());
//
// 	// 设置线路颜色
// 	FColor railColor = GetRailColor();
// 	railLine->SetRailColor(railColor);
// 	railLine->Init();
// 	
// 	// 添加当前车站
// 	railLine->stations.Add(station);
//
// 	// 添加一个新段
// 	URailSegment* newSeg = NewObject<URailSegment>(railLine);
// 	newSeg->init(station);
// 	railLine->railSegments.Add(newSeg);
// }
//
// void ACxxPlayerController::EndRailLineDraw(bool bIsCreate)
// {
// 	if(bIsCreate)
// 	{
// 		if(!railLine->railSegments.Last()->bEndIsStation)
// 		{
// 			URailSegment* segment = railLine->railSegments.Top();
// 			railLine->railSegments.Pop();
// 			segment->MarkAsGarbage();
// 			segment = nullptr;
// 		}
// 		railLines.Add(railLine);
// 		railLine->FinishEdit(); // 结束编辑生成轨道实例
// 		railLine = nullptr;
// 	}
// 	else
// 	{
// 		railLine->Destroy();
// 		railLine = nullptr;
// 	}
// }
//
// FColor ACxxPlayerController::GetRailColor()
// {
// 	FColor resColor = railColors[railColorIdx];
// 	railColorIdx = (railColorIdx + 1) % railColors.Num();
// 	return resColor;
// }
