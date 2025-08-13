// Fill out your copyright notice in the Description page of Project Settings.

#include "CxxRailLine.h" 
#include "CxxMonoRailTrain.h"
#include "CxxRailLineSegment.h"

void ACxxRailLine::Init(UClass* m1, UClass* t1, UClass* t2, UClass* b, UClass* pb, UClass* pr)
{
	movementComponentClass = m1;
	tangentComponentAClass = t1;
	tangentComponentBClass = t2;
	railLineBaseClass = b;
	railLinePillarBaseClass = pb;
	railLinePillarRepeatClass = pr;
}

// Sets default values
ACxxRailLine::ACxxRailLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SetRootComponent(root);
}


void ACxxRailLine::Init(FString name)
{
	// isEdit = true;
	UStaticMesh* splineMesh = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Script/Engine.StaticMesh'/Game/Assets/Models/MonoRail/MonoRail_BaseRail_Split.MonoRail_BaseRail_Split'"));
	UStaticMesh* pillarMesh = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Script/Engine.StaticMesh'/Game/Assets/Models/MonoRail/MonoRail_Pillar.MonoRail_Pillar'"));
	
	if (splineMesh)
	{
		railLineBaseMesh = splineMesh;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find SplineMesh"));
	}

	if(pillarMesh)
	{
		railLinePillarMesh = pillarMesh;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find PillarMesh"));
	}

	railLineName = name;
}

void ACxxRailLine::FinishedEditRailLineFromStation()
{
	if(!curSelectedPoint) return;
	// 如果当前选择点不是左右都有站点的点，则一并删除
	if(!curSelectedPoint->nextSegment)
	{
		while(true)
		{
			if(curSelectedPoint->GetPointType() != StationPoint)
			{
				controlPoints.Remove(curSelectedPoint);
				curSelectedPoint->Destroy();
				railSegments.Remove(curSelectedPoint->preSegment);
				curSelectedPoint->preSegment->Destroy();
				curSelectedPoint = curSelectedPoint->preSegment->startPoint;
			}
			else
			{
				curSelectedPoint->nextSegment = nullptr;
				break;
			}
		}
	}else if(!curSelectedPoint->preSegment)
	{
		while(true)
		{
			if(curSelectedPoint->GetPointType() != StationPoint)
			{
				controlPoints.Remove(curSelectedPoint);
				curSelectedPoint->Destroy();
				railSegments.Remove(curSelectedPoint->nextSegment);
				curSelectedPoint->nextSegment->Destroy();
				curSelectedPoint = curSelectedPoint->nextSegment->endPoint;
			}
			else
			{
				curSelectedPoint->preSegment = nullptr;
				break;
			}
		}
	}

	curSelectedPoint = nullptr;
}

TArray<UCxxRailLineSegment*> ACxxRailLine::IsCurSelectedPointNeighbor(ACxxStation* station)
{
	TArray<UCxxRailLineSegment*> neighborSegments;
	// 遍历各侧的第一个站点，看是否是station
	UCxxPathControlPoint* pPoint = curSelectedPoint;
	while(true)
	{
		if(pPoint->GetPointType() == StationPoint)
		{
			if(pPoint->station == station)
			{
				if(pPoint->preSegment) neighborSegments.Insert(pPoint->preSegment, 0);
				return neighborSegments;
			}
			else
			{
				if(!neighborSegments.IsEmpty()) neighborSegments.Empty();
				break;
			}
		}
		if(pPoint->preSegment && pPoint->preSegment->startPoint)
		{
			neighborSegments.Insert(pPoint->preSegment, 0);
			pPoint = pPoint->preSegment->startPoint;
		}
		else
		{
			if(!neighborSegments.IsEmpty()) neighborSegments.Empty();
			break;
		}
	}

	UCxxPathControlPoint* nPoint = curSelectedPoint;
	while(true)
	{
		if(nPoint->GetPointType() == StationPoint)
		{
			if(nPoint->station == station)
			{
				if(nPoint->nextSegment) neighborSegments.Add(nPoint->nextSegment);
				return neighborSegments;
			}
			else
			{
				if(!neighborSegments.IsEmpty()) neighborSegments.Empty();
				break;
			}
		}
		if(nPoint->nextSegment && nPoint->nextSegment->endPoint)
		{
			neighborSegments.Add(nPoint->nextSegment);
			nPoint = nPoint->nextSegment->endPoint;
		}
		else
		{
			if(!neighborSegments.IsEmpty()) neighborSegments.Empty();
			break;
		}
	}

	return neighborSegments;
}

UCxxPathControlPoint* ACxxRailLine::GetPointFromStation(ACxxStation* station)
{
	for(auto point : controlPoints)
	{
		if(point->GetPointType() == PointType::StationPoint && point->station == station)
		{
			// 缺了一个最后的站点
			return point;
		}
	}
	return nullptr;
}

UCxxRailLineSegment* ACxxRailLine::GetSegmentFromMesh(USplineMeshComponent* mesh)
{
	for (auto seg : railSegments)
	{
		if(seg->splineMesh == mesh)
		{
			return seg;
		}
	}
	return nullptr;
}

UCxxPathControlPoint* ACxxRailLine::AddStation(ACxxStation* station, bool bIsArrowDir)
{
	UArrowComponent* comp = Cast<UArrowComponent>(station->GetComponentByClass(UArrowComponent::StaticClass()));
	// 生成点并附着段  0 —— 1 —— 3
	if(stations.IsEmpty())
	{
		UCxxPathControlPoint* controlPoint1 = NewObject<UCxxPathControlPoint>(this);
		controlPoint1->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint1->RegisterComponent();
		UCxxPathControlPoint* controlPoint2 = NewObject<UCxxPathControlPoint>(this);
		controlPoint2->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint2->RegisterComponent();
		UCxxPathControlPoint* controlPoint3 = NewObject<UCxxPathControlPoint>(this);
		controlPoint3->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint3->RegisterComponent();
		
		if(bIsArrowDir){
			controlPoint1->InitStationPoint(station, station->GetActorLocation(), -comp->GetForwardVector(), PointType::StationPoint,
				nullptr, nullptr);
			controlPoint2->Init(station->GetActorLocation() - comp->GetForwardVector() * railOutLength,
				-comp->GetForwardVector() * tangentIntensity, FixedPoint, nullptr, nullptr);
			controlPoint3->Init(FVector(0, 0, 0), -comp->GetForwardVector()  * tangentIntensity, PointType::ManualPoint,
			nullptr, nullptr);
		}else
		{
			controlPoint1->InitStationPoint(station, station->GetActorLocation(),  comp->GetForwardVector(), PointType::StationPoint,
				nullptr, nullptr);
			controlPoint2->Init(station->GetActorLocation() + comp->GetForwardVector() * railOutLength,
				comp->GetForwardVector()  * tangentIntensity, FixedPoint, nullptr, nullptr);
			controlPoint3->Init(FVector(0, 0, 0), comp->GetForwardVector()  * tangentIntensity, PointType::ManualPoint,
			nullptr, nullptr);
		}
		controlPoints.Add(controlPoint1);
		controlPoints.Add(controlPoint2);
		controlPoints.Add(controlPoint3);

		UCxxRailLineSegment* seg1 = NewObject<UCxxRailLineSegment>(this);
		seg1->RegisterComponent();
		UCxxRailLineSegment* seg2 = NewObject<UCxxRailLineSegment>(this);
		seg2->RegisterComponent();

		controlPoint1->SetSegment(nullptr, seg1);
		controlPoint2->SetSegment(seg1, seg2);
		controlPoint3->SetSegment(seg2, nullptr);
		
		seg1->Init(controlPoint1, controlPoint2, pillarIntervalDist, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		seg2->Init(controlPoint2, controlPoint3, pillarIntervalDist, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		
		railSegments.Add(seg1);
		railSegments.Add(seg2);

		curSelectedPoint = controlPoint3;
	}
	// 直接添加三个站点
	else
	{
		if(!curSelectedPoint) return nullptr;
		UCxxPathControlPoint* controlPoint1 = NewObject<UCxxPathControlPoint>(this);
		controlPoint1->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint1->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
		controlPoint1->RegisterComponent();
		UCxxPathControlPoint* controlPoint2 = NewObject<UCxxPathControlPoint>(this);
		controlPoint2->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint2->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
		controlPoint2->RegisterComponent();
		UCxxPathControlPoint* controlPoint3 = NewObject<UCxxPathControlPoint>(this);
		controlPoint3->Init(movementComponentClass, tangentComponentAClass, tangentComponentBClass);
		controlPoint3->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
		controlPoint3->RegisterComponent();
		
		controlPoints.Add(controlPoint1);
		controlPoints.Add(controlPoint2);
		controlPoints.Add(controlPoint3);

		bool bIsForward = false;;
		if(!curSelectedPoint->preSegment)
		{
			// 说明是向前添加
			bIsForward = true;
		}
		
		UCxxRailLineSegment* insertSeg = curSelectedPoint->nextSegment;
		if(insertSeg)
		{
			UE_LOG(LogTemp, Warning, TEXT("insertSeg is not nullptr"));
		}
		
		if(bIsArrowDir ^ bIsForward)
		{
			controlPoint1->Init(station->GetActorLocation() + comp->GetForwardVector() * railOutLength,
				-comp->GetForwardVector() * tangentIntensity, FixedPoint, nullptr, nullptr);
			controlPoint2->InitStationPoint(station, station->GetActorLocation(), -comp->GetForwardVector() * tangentIntensity,
				PointType::StationPoint, nullptr, nullptr);
			controlPoint3->Init(station->GetActorLocation() - comp->GetForwardVector() * railOutLength,
				- comp->GetForwardVector() * tangentIntensity, FixedPoint, nullptr, nullptr);
			curSelectedPoint->Init(FVector(0, 0, 0), - comp->GetForwardVector(), PointType::ManualPoint,
				curSelectedPoint->preSegment, curSelectedPoint->nextSegment);
		}else
		{
			controlPoint1->Init(station->GetActorLocation() - comp->GetForwardVector() * railOutLength,
				comp->GetForwardVector() * tangentIntensity, FixedPoint, nullptr, nullptr);
			controlPoint2->InitStationPoint(station, station->GetActorLocation(), comp->GetForwardVector() * tangentIntensity,
				PointType::StationPoint, curSelectedPoint->preSegment, curSelectedPoint->nextSegment);
			controlPoint3->Init(station->GetActorLocation() + comp->GetForwardVector() * railOutLength,
				comp->GetForwardVector() * tangentIntensity, FixedPoint, nullptr, nullptr);
			curSelectedPoint->Init(FVector(0, 0, 0), comp->GetForwardVector() * tangentIntensity,
				PointType::ManualPoint, curSelectedPoint->preSegment, curSelectedPoint->nextSegment);
		}

		UCxxRailLineSegment* seg1 = NewObject<UCxxRailLineSegment>(this);
		UCxxRailLineSegment* seg2 = NewObject<UCxxRailLineSegment>(this);
		UCxxRailLineSegment* seg3 = NewObject<UCxxRailLineSegment>(this);
		seg1->RegisterComponent();
		seg2->RegisterComponent();
		seg3->RegisterComponent();
		
		bIsForward?curSelectedPoint->nextSegment->Init(controlPoint3 ,curSelectedPoint->nextSegment->endPoint, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass):
		curSelectedPoint->preSegment->Init(curSelectedPoint->preSegment->startPoint, controlPoint1, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);

		bIsForward?controlPoint1->SetSegment(seg1, seg2):controlPoint1->SetSegment(curSelectedPoint->preSegment, seg1);
		bIsForward?controlPoint2->SetSegment(seg2, seg3):controlPoint2->SetSegment(seg1, seg2);
		bIsForward?controlPoint3->SetSegment(seg3, curSelectedPoint->nextSegment):controlPoint3->SetSegment(seg2, seg3);
		bIsForward?curSelectedPoint->SetSegment(curSelectedPoint->preSegment, seg1):curSelectedPoint->SetSegment(seg3, curSelectedPoint->nextSegment);
		
		bIsForward?seg1->Init(curSelectedPoint, controlPoint1, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass):seg1->Init(controlPoint1, controlPoint2, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		bIsForward?seg2->Init(controlPoint1, controlPoint2, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass):seg2->Init(controlPoint2, controlPoint3, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		bIsForward?seg3->Init(controlPoint2, controlPoint3, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass):seg3->Init(controlPoint3, curSelectedPoint, pillarIntervalDist,
			railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);

		if(insertSeg && !bIsForward)
		{
			insertSeg->SetStartPoint(curSelectedPoint);
			// curSelectedPoint->nextSegment = insertSeg;
		}
		
		railSegments.Add(seg1);
		railSegments.Add(seg2);
		railSegments.Add(seg3);
	}
	stations.Add(station);
	return curSelectedPoint;
}

// point1鼠标点, point2站点
UCxxPathControlPoint* ACxxRailLine::AddRingLineStation(ACxxStation* station, UCxxPathControlPoint* point1, UCxxPathControlPoint* point2)
{
	UArrowComponent* comp = Cast<UArrowComponent>(station->GetComponentByClass(UArrowComponent::StaticClass()));
	if(!point2->preSegment)
	{
		// 计算comp->GetForwardVector()是否和point2->nextSegment->endPoint->location - point2->location同方向
		bool bSameDir = FVector::DotProduct(comp->GetForwardVector(), point2->nextSegment->endPoint->location - point2->location) > 0;
		if(bSameDir)
		{
			point1->Init(station->GetActorLocation() - comp->GetForwardVector() * railOutLength,
				comp->GetForwardVector() * tangentIntensity, FixedPoint, point1->preSegment, point1->nextSegment);
		}else
		{
			point1->Init(station->GetActorLocation() + comp->GetForwardVector() * railOutLength,
				-comp->GetForwardVector() * tangentIntensity, FixedPoint, point1->preSegment, point1->nextSegment);
		}
		UCxxRailLineSegment* seg = NewObject<UCxxRailLineSegment>(this);
		seg->RegisterComponent();
		point1->SetSegment(point1->preSegment,seg);
		point2->SetSegment(seg, point2->nextSegment);
		seg->Init(point1, point2, pillarIntervalDist,railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		point1->preSegment->SetEndPoint(point1);
		railSegments.Add(seg);
		ringLineSegment = seg;
	}else
	{
		bool bSameDir = FVector::DotProduct(comp->GetForwardVector(), point2->preSegment->startPoint->location - point2->location) > 0;
		if(bSameDir)
		{
			point1->Init(station->GetActorLocation() - comp->GetForwardVector() * railOutLength,
				-comp->GetForwardVector() * tangentIntensity, FixedPoint, point1->preSegment, point1->nextSegment);
		}else
		{
			point1->Init(station->GetActorLocation() + comp->GetForwardVector() * railOutLength,
				comp->GetForwardVector() * tangentIntensity, FixedPoint, point1->preSegment, point1->nextSegment);
		}
		UCxxRailLineSegment* seg = NewObject<UCxxRailLineSegment>(this);
		seg->RegisterComponent();
		point1->SetSegment(seg, point1->nextSegment);
		point2->SetSegment(point2->preSegment, seg);
		seg->Init(point2, point1, pillarIntervalDist, railLineBaseClass, railLinePillarBaseClass, railLinePillarRepeatClass);
		point1->nextSegment->SetStartPoint(point1);
		railSegments.Add(seg);
		ringLineSegment = seg;
	}
	ringLinePoint = point1;
	bIsRingLine = true;
	ringLineStation = station;
	return point1;
}

UCxxPathControlPoint* ACxxRailLine::DeleteRingLineStation(ACxxStation* station)
{
	ringLineSegment->startPoint->nextSegment = nullptr;
	ringLineSegment->endPoint->preSegment = nullptr;
	ringLineSegment->Destroy();
	railSegments.Remove(ringLineSegment);
	ringLineSegment = nullptr;
	
	ringLinePoint->Init(station->GetActorLocation(),
				FVector::Zero(), PointType::ManualPoint, ringLinePoint->preSegment, ringLinePoint->nextSegment);
	
	bIsRingLine = false;
	ringLineStation = nullptr;
	return ringLinePoint;
}

UCxxPathControlPoint* ACxxRailLine::DeleteStation(ACxxStation* station)
{
	if(!curSelectedPoint) return nullptr;
	// 是从修改-从站点修改过来的
	if(curSelectedPoint->GetPointType() == StationPoint)
	{
		if(curSelectedPoint->station == station)
		{
			// 删除该站点并修改为手动控制点
			stations.Remove(station);
			station->isHighlighted = true;
			station->SwitchOutlineEffect();

			// 修改当前点的属性
			curSelectedPoint->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
			curSelectedPoint->SetPointType(PointType::ManualPoint);

			// 删除左右的点和段
			if(auto nSeg = curSelectedPoint->nextSegment)
			{
				if(auto nPoint = curSelectedPoint->nextSegment->endPoint)
				{
					if(curSelectedPoint->nextSegment->endPoint->nextSegment)
					{
						curSelectedPoint->nextSegment->endPoint->nextSegment->startPoint = curSelectedPoint;
						curSelectedPoint->nextSegment = curSelectedPoint->nextSegment->endPoint->nextSegment;
					}else
					{
						curSelectedPoint->nextSegment = nullptr;
					}
					nPoint->DestroyComponent();
					controlPoints.Remove(nPoint);
				}
				nSeg->Destroy();
				railSegments.Remove(nSeg);
			}
			if(auto pSeg = curSelectedPoint->preSegment)
			{
				if(auto pPoint = curSelectedPoint->preSegment->startPoint)
				{
					if(curSelectedPoint->preSegment->startPoint->preSegment)
					{
						curSelectedPoint->preSegment->startPoint->preSegment->endPoint = curSelectedPoint;
						curSelectedPoint->preSegment = curSelectedPoint->preSegment->startPoint->preSegment;
					}else
					{
						curSelectedPoint->preSegment = nullptr;
					}
					pPoint->DestroyComponent();
					controlPoints.Remove(pPoint);
				}
				pSeg->Destroy();
				railSegments.Remove(pSeg);
			}
		}
		return curSelectedPoint;
	}

	// 鼠标点修改过来的
	TArray<UCxxRailLineSegment*> neighborSegments = IsCurSelectedPointNeighbor(station);
	if(stations.Num() > 1 && stations.Contains(station) && !neighborSegments.IsEmpty())
	{
		stations.Remove(station);
		station->isHighlighted = true;
		station->SwitchOutlineEffect();

		UCxxPathControlPoint* mousePoint = neighborSegments[0]->startPoint;
		mousePoint->SetPointType(ManualPoint);
		curSelectedPoint = mousePoint;
		
		for(auto seg : neighborSegments)
		{
			if(seg == neighborSegments.Last())
			{
				mousePoint->nextSegment = seg->endPoint->nextSegment;
				if(seg->endPoint->nextSegment)
				{
					seg->endPoint->nextSegment->SetStartPoint(mousePoint);
				}
				else
				{
					mousePoint->nextSegment = nullptr;
				}
			}
			controlPoints.Remove(seg->endPoint);
			seg->endPoint->Destroy();
			railSegments.Remove(seg);
			seg->Destroy();
		}
	}
	return curSelectedPoint;
}


TArray<ACxxStation*> ACxxRailLine::GetNeighborStations(ACxxStation* station)
{
	TArray<ACxxStation*> res;
	UCxxPathControlPoint* point = GetPointFromStation(station);
	if(!point) return res;
	// 检查next的站
	UCxxPathControlPoint* tPoint = point;
	while(tPoint->nextSegment)
	{
		tPoint = tPoint->nextSegment->endPoint;
		if(tPoint->GetPointType() == PointType::StationPoint)
		{
			res.Add(tPoint->station);
			break;
		}
	}

	tPoint = point;
	while(tPoint->preSegment)
	{
		tPoint = tPoint->preSegment->startPoint;
		if(tPoint->GetPointType() == PointType::StationPoint)
		{
		 res.Add(tPoint->station);
		 break;
		}
	}

	return res;
}

// Called when the game starts or when spawned
void ACxxRailLine::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACxxRailLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACxxRailLine::SwitchHightLightEffect(bool bVisiblePoint)
{
	bIsHighlighted = !bIsHighlighted;
	for(auto seg : railSegments)
	{
		seg->SwitchHightLightEffect(bIsHighlighted);
	}
	for(auto station:stations)
	{
		station->SwitchOutlineEffect();
	}
	if (bVisiblePoint)
	{
		for(auto point:controlPoints)
		{
			point->SetVisible(bIsHighlighted);
		}	
	}
}
