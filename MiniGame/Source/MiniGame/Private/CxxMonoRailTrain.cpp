// Fill out your copyright notice in the Description page of Project Settings.

#include "CxxMonoRailTrain.h"
#include "Components/CanvasPanelSlot.h"

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
	mesh = FindComponentByClass<UStaticMeshComponent>();
}

// Called every frame
void ACxxMonoRailTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bInStation) MovementControl(DeltaTime);

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
	ACxxDispatchManager* tDispatchManager, UCxxRailLineSegment* seg, UCanvasPanel* rootPanel)
{
	this->railLine = line;
	this->bCurIsForward = isForward;
	bInStation = false;
	this->curSegment = seg;
	this->curSegmentMovedDist = 0;
	this->speed = tSpeed;
	// this->offSetRail = tOffSetRail;
	this->dispatchManager = tDispatchManager;

	if (dispatchManager)
	{
		// 创建train的乘客UI
		horizontalBox = NewObject<UHorizontalBox>(dispatchManager->hudWidget);
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(rootPanel->AddChild(horizontalBox));
		if (slot)
		{
			slot->SetOffsets(FMargin(-1000.0, -1000.0, 320, 64));
			slot->SetAlignment(FVector2D(0.5, 0.5));
			slot->SetAutoSize(true);
		}
		
		trainImage = NewObject<UImage>(dispatchManager);
		traintexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/Image/trainHead.trainHead'"));
		if (traintexture && trainImage)
		{
			FSlateBrush brush; 
			brush.SetResourceObject(traintexture);
			brush.ImageSize = FVector2D(21, 24);
			trainImage->SetBrush(brush);
		}	
	}

	horizontalBox->AddChildToHorizontalBox(trainImage);
}

void ACxxMonoRailTrain::MovementControl(float DeltaTime)
{
	FVector nextPosition = GetNextPosition(DeltaTime) + FVector(0,0,150);
	FRotator nextRotation = GetNextRotation();

	const FVector rightDir = FRotationMatrix(nextRotation).GetUnitAxis(EAxis::Y);
	
	// 获取nextRotation的垂直方向
	FVector OffsetDir = bCurIsForward ? rightDir : -rightDir;
	
	FVector nextOffsetPosition = nextPosition + OffsetDir * offSetRail;
	
	SetActorLocation(nextOffsetPosition);
	SetActorRotation(bCurIsForward?nextRotation + FRotator(0.f, 180.f, 0.f):nextRotation);
}

FVector ACxxMonoRailTrain::GetNextPosition(float DeltaTime)
{
	float curFrameCanMovingDist = DeltaTime * speed;
	curFrameCanMovingDist += curSegmentMovedDist;
	float curMovingDist = 0;
	bool bFindNextPosition = false;
	bool bFindIsStation = false;

	while(!bFindIsStation && !bFindNextPosition)
	{
		// 如果下一个位置在当前段中
		if (curMovingDist + curSegment->spline->GetSplineLength() > curFrameCanMovingDist)
		{
			bFindNextPosition = true;
			curSegmentMovedDist = curFrameCanMovingDist - curMovingDist;
			break;
		}
		else
		{
			// 如果是站点，停下来上下车
			if (bCurIsForward?curSegment->endPoint->type == PointType::StationPoint:
				curSegment->startPoint->type == PointType::StationPoint)
			{
				bFindIsStation = true;
				bInStation = true;
				curSegmentMovedDist = 0.f;
				break;
			}
			// 如果是非站点，继续遍历下一个
			else
			{
				curMovingDist += curSegment->spline->GetSplineLength();
				bCurIsForward?curSegment = curSegment->endPoint->nextSegment:curSegment = curSegment->startPoint->preSegment;
			}
		}
	}

	if (bFindIsStation)
	{
		// 端点站
		if (!curSegment->endPoint->nextSegment || !curSegment->startPoint->preSegment)
		{
			bCurIsForward = !bCurIsForward;
		}
		// 继续下一段
		else
		{
			bCurIsForward?curSegment = curSegment->endPoint->nextSegment:curSegment = curSegment->startPoint->preSegment;
		}
		return bCurIsForward?curSegment->startPoint->location:curSegment->endPoint->location;
	}

	return curSegment->spline->GetLocationAtDistanceAlongSpline(bCurIsForward?curSegmentMovedDist:
		curSegment->spline->GetSplineLength() - curSegmentMovedDist, ESplineCoordinateSpace::World);
}

FRotator ACxxMonoRailTrain::GetNextRotation()
{
	return curSegment->spline->GetRotationAtDistanceAlongSpline(bCurIsForward?curSegmentMovedDist:
		curSegment->spline->GetSplineLength() - curSegmentMovedDist, ESplineCoordinateSpace::World);
}

void ACxxMonoRailTrain::SwitchHightLightEffect()
{
	bIsHighlighted = !bIsHighlighted;
	if (mesh) mesh->SetRenderCustomDepth(bIsHighlighted);
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
	bool bBoardingAndAlighting = false;
	UCxxPathControlPoint* point = bCurIsForward?curSegment->startPoint:curSegment->endPoint;
	// 获取下一站
	ACxxStation* nextStation = nullptr;
	// 这里不能默认从站点出发
	ACxxStation* curStation = bCurIsForward?curSegment->startPoint->station:curSegment->endPoint->station;
	
	// 直接下客
	if (bMarkedForRemoval)
	{
		if (passengers.Num() > 0)
		{
			if (passengers[0]->targetStationType == curStation->GetStationType())
			{
				passengers[0]->DestroyPassenger();
				passengers.RemoveAt(0);
				// 加分
				if(dispatchManager)
				{
					dispatchManager->AddScore(10);
				}
			}else
			{
				curStation->AddPassenger(passengers[0]);
				passengers.RemoveAt(0);
			}
			return;
		}else
		{
			DestroyTrain();
			return;
		}
	}
	
	if (bCurIsForward)
	{
		while(point->nextSegment)
		{
			point = point->nextSegment->endPoint;
			if(point->GetPointType() == PointType::StationPoint)
			{
				nextStation = point->station;
				break;
			}
		}
	}else
	{
		while(point->preSegment)
		{
			point = point->preSegment->startPoint;
			if(point->GetPointType() == PointType::StationPoint)
			{
				nextStation = point->station;
				break;
			}
		}
	}

	// 下车阶段
	for(int i = passengers.Num() - 1; i >= 0; i--)
	{
		if(passengers[i]->IsAlightingRequired(nextStation))
		{
			if (passengers[i]->targetStationType == curStation->GetStationType())
			{
				passengers[i]->DestroyPassenger();
				passengers.RemoveAt(i);
				// 加分
				if(dispatchManager)
				{
					dispatchManager->AddScore(10);
				}
			}else
			{
				curStation->AddPassenger(passengers[i]);
				passengers.RemoveAt(i);
			}
			bBoardingAndAlighting = true;
			break;
		}
	}

	// 上车阶段
	if (bBoardingAndAlighting) return;
	for(int i = 0; i < curStation->passengers.Num(); i++)
	{
		if(passengers.Num() >= capacity)
		{
			break;	
		}
		// 判断是否上车
		if(curStation->passengers[i]->IsBoardingRequired(nextStation))
		{
			AddPassenger(curStation->passengers[i]);
			curStation->passengers.RemoveAt(i);
			bBoardingAndAlighting = true;
			break;
		}
	}

	if(bBoardingAndAlighting) return;
	for(auto passenger:passengers)
	{
		passenger->pathIdx++;
	}
	bInStation = false;
	inStationTime = 0;
}

void ACxxMonoRailTrain::AddPassenger(ACxxPassenger* passenger)
{
	passengers.Add(passenger);

	// UI修改
	passenger->passagerImage->RemoveFromParent();
	horizontalBox->AddChildToHorizontalBox(passenger->passagerImage);
}

void ACxxMonoRailTrain::MarkTrainAsDelete()
{
	bMarkedForRemoval = true;
}

void ACxxMonoRailTrain::DestroyTrain()
{
	for(auto passenger : passengers)
	{
		passenger->Destroy();
	}
	if (dispatchManager)
	{
		dispatchManager->trainNum++;
		dispatchManager->trains.Remove(this);
	}
	if (railLine) railLine->trains.Remove(this);
	if (horizontalBox) horizontalBox->SetVisibility(ESlateVisibility::Hidden);
	if (trainImage) trainImage->SetVisibility(ESlateVisibility::Hidden);
	horizontalBox = nullptr;
	trainImage = nullptr;
	passengers.Empty();
	this->Destroy();
}

