// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxMonoRailTrain.h"

// Sets default values
ACxxMonoRailTrain::ACxxMonoRailTrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACxxMonoRailTrain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACxxMonoRailTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bInStation) MovementControl(DeltaTime);
	ShowPassengerStatus();

	// 每隔inStationTime后进行一次上下车
	if(bInStation)
	{
		inStationTime += DeltaTime;
		if(inStationTime > 0.25)
		{
			inStationTime = 0;
			BoardingAndAlighting();
		}
	}
}

void ACxxMonoRailTrain::Init(ACxxRailLine* line, bool isForward, float tSpeed, float tOffSetRail,
	ACxxDispatchManager* tDispatchManager)
{
	this->railLine = line;
	this->bCurIsForward = isForward;
	this->curSegmentIdx = 1;
	this->curPointIdx = 0;
	this->curBlockMovedDist = 0;
	this->speed = tSpeed;
	this->offSetRail = tOffSetRail;
	this->dispatchManager = tDispatchManager;
}

void ACxxMonoRailTrain::MovementControl(float DeltaTime)
{
	FVector nextPosition = GetNextPosition(DeltaTime) + FVector(0,0,50);
	// UE_LOG(LogTemp, Warning, TEXT("nextPosition: %s"), *nextPosition.ToString());
	FVector nextRotation = GetNextRotation();

	// 获取nextRotation的垂直方向
	FVector offsetDirt = FVector(-nextRotation.Y,nextRotation.X,0);
	
	nextPosition = bCurIsForward?nextPosition + offsetDirt.GetSafeNormal() * offSetRail:
								nextPosition - offsetDirt.GetSafeNormal() * offSetRail;
	
	SetActorLocation(nextPosition);
	SetActorRotation(nextRotation.Rotation());
}

FVector ACxxMonoRailTrain::GetNextPosition(float DeltaTime)
{
	// float curFrameCanMovingDist = DeltaTime * speed;
	// curFrameCanMovingDist += curBlockMovedDist;
	// float curMovingDist = 0;
	// bool bFindNextPoint = false;
	// bool bFindIsStation = false;
	// while(!bFindIsStation && !bFindNextPoint)
	// {
	// 	for(int i = curPointIdx;
	// 		bCurIsForward?i < railLine->railSegments[curSegmentIdx]->points.Num() - 1:i > 0;
	// 		bCurIsForward?i++:i--)
	// 	{
	// 		FVector prePoint = railLine->railSegments[curSegmentIdx]->points[i];
	// 		FVector nextPoint = railLine->railSegments[curSegmentIdx]->points[bCurIsForward?i+1:i-1];
	// 		float curBlockLengthSum = (prePoint - nextPoint).Size();
	// 		// 如果当前Block的移动距离会导致已移动大于可移动距离
	// 		if(curMovingDist + curBlockLengthSum > curFrameCanMovingDist)
	// 		{
	// 			curPointIdx = i;
	// 			curBlockMovedDist = curFrameCanMovingDist - curMovingDist;
	// 			bFindNextPoint = true;
	// 			break;
	// 		}
	// 		curMovingDist += curBlockLengthSum;
	// 	}
	// 	// 说明到站点了
	// 	if(!bFindNextPoint)
	// 	{
	// 		// 判断是否是终点站,终点站就反转
	// 		if((bCurIsForward && curSegmentIdx == railLine->railSegments.Num() - 2) ||
	// 			(!bCurIsForward && curSegmentIdx == 1))
	// 		{
	// 			curSegmentIdx = bCurIsForward?railLine->railSegments.Num() - 2:1;
	// 			curPointIdx = bCurIsForward?railLine->railSegments[curSegmentIdx]->points.Num() - 1:0;
	// 			bCurIsForward = !bCurIsForward;
	// 			curBlockMovedDist = 0;
	// 			bInStation = true;
	// 		}
	// 		// 反之继续走到下一站
	// 		else
	// 		{
	// 			bCurIsForward?curSegmentIdx++:curSegmentIdx--;
	// 			bCurIsForward?curPointIdx = 0:curPointIdx = railLine->railSegments[curSegmentIdx]->points.Num() - 1;
	// 			curBlockMovedDist = 0;
	// 			bInStation = true;
	// 		}
	// 	}
	// }
	//
	// if (bFindIsStation)
	// {
	// 	return railLine->railSegments[curSegmentIdx]->points[curPointIdx];
	// }
	// if (bFindNextPoint)
	// {
	// 	FVector blockVec = railLine->railSegments[curSegmentIdx]->points[bCurIsForward?curPointIdx + 1:curPointIdx - 1]
	// 				- railLine->railSegments[curSegmentIdx]->points[curPointIdx];
	// 	return railLine->railSegments[curSegmentIdx]->points[curPointIdx] + blockVec * (curBlockMovedDist / blockVec.Size());
	// }
	return FVector(0,0,0);
}

FVector ACxxMonoRailTrain::GetNextRotation()
{
	// 插值当前的旋转
	// FVector tangentA = railLine->railSegments[curSegmentIdx]->tangents[curPointIdx];
	// FVector tangentB = railLine->railSegments[curSegmentIdx]->tangents[bCurIsForward?curPointIdx + 1:curPointIdx - 1];
	// float blockSize = (railLine->railSegments[curSegmentIdx]->points[bCurIsForward?curPointIdx + 1:curPointIdx - 1]
	// 				- railLine->railSegments[curSegmentIdx]->points[curPointIdx]).Size();
	// return bCurIsForward?FMath::Lerp(tangentA, tangentB, curBlockMovedDist / blockSize):
	// 					FMath::Lerp(tangentA, tangentB, curBlockMovedDist / blockSize);

	return FVector(0,0,0);
}

void ACxxMonoRailTrain::ShowPassengerStatus()
{
	// 根据乘客的targetStationType间隔一定距离绘制Deug圆形
	for (int i = 0; i < passengers.Num(); i++)
	{
		FVector location = this->GetActorLocation();
		location.Z += 200;
		location.Y -= 50;
		location.X += 70 * i;
		switch (passengers[i]->targetStationType)
		{
		case EStationType::BusinessDistrict:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Yellow, false, 0.0f);
			break;
		case EStationType::ResidentialArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Blue, false, 0.0f);
			break;
		case EStationType::CulturalArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Red, false, 0.0f);
			break;
		case EStationType::RecreationalArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Green, false, 0.0f);
			break;
		default:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Black, false, 0.0f);
			break;
		}
	}
}

void ACxxMonoRailTrain::BoardingAndAlighting()
{
	// // ACxxStation* curStation = bCurIsForward?railLine->railSegments[curSegmentIdx]->stationA:
	// // 									railLine->railSegments[curSegmentIdx]->stationB;
	// bool bBoardingAndAlighting = false;
	// // 先下车
	// for(int i = passengers.Num() - 1; i >= 0; i--)
	// {
	// 	if(passengers[i]->targetStationType == curStation->GetStationType())
	// 	{
	// 		passengers.RemoveAt(i);
	// 		bBoardingAndAlighting = true;
	// 		// 加分
	// 		if(dispatchManager)
	// 		{
	// 			dispatchManager->AddScore(10);
	// 		}
	// 		break;
	// 	}
	// }
	//
	// if(bBoardingAndAlighting) return;
	//
	// // 再上车
	// if(passengers.Num() < capacity)
	// {
	// 	for(int i = 0; i < curStation->passengers.Num(); i++)
	// 	{
	// 		TArray<EStationType> path;
	// 		// 生成后续的站点类型
	// 		for(int j = curSegmentIdx;
	// 			bCurIsForward?j < railLine->railSegments.Num() - 1:j > 0;
	// 			bCurIsForward?j++:j--)
	// 		{
	// 			// path.Add(bCurIsForward?railLine->railSegments[j]->stationB->GetStationType():
	// 			// 						railLine->railSegments[j]->stationA->GetStationType());
	// 		}
	// 		
	// 		if(curStation->passengers[i]->IsBoardingRequired(path))
	// 		{
	// 			passengers.Add(curStation->passengers[i]);
	// 			curStation->passengers.RemoveAt(i);
	// 			bBoardingAndAlighting = true;
	// 			break;
	// 		}
	// 	}
	// }
	//
	// if(bBoardingAndAlighting) return;
	// bInStation = false;
	// inStationTime = 0;
}

