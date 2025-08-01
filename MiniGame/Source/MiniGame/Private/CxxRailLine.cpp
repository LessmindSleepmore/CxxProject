// Fill out your copyright notice in the Description page of Project Settings.

#include "CxxRailLine.h" 
#include "CxxMonoRailTrain.h"
#include "CxxRailLineSegment.h"

// Sets default values
ACxxRailLine::ACxxRailLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UBlueprint> railLineBaseClassFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLineBase.RailLineBase'"));
	if (railLineBaseClassFinder.Succeeded())
	{
		railLineBaseClass = railLineBaseClassFinder.Object->GeneratedClass;
	}
	
	static ConstructorHelpers::FObjectFinder<UBlueprint> railLinePillarClassFinder(
	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLinePillar.RailLinePillar'"));
	if (railLinePillarClassFinder.Succeeded())
	{
		railLinePillarClass = railLinePillarClassFinder.Object->GeneratedClass;
	}

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SetRootComponent(root);
}

// ACxxRailLine::ACxxRailLine(FColor color)
// {
// 	railLineColor = color;
// }

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
	}else
	{
		curSelectedPoint->SetPointType(ManualPoint);
	}

	curSelectedPoint = nullptr;
}

// void ACxxRailLine::ProceeEndWithMouse(UCxxRailSegment* seg, UCxxRailSegment* preSeg)
// {
// 	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
// 	if (!playerController) return;
// 		
// 	float mouseX, mouseY;
// 	playerController->GetMousePosition(mouseX, mouseY);
//
// 	FVector wLocation, wDirection;
// 	if (playerController->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection))
// 	{
// 		FHitResult hitRes;
// 		FCollisionQueryParams cParams;
// 			
// 		UArrowComponent* arrowComp = Cast<UArrowComponent>(seg->stationA->GetComponentByClass(UArrowComponent::StaticClass()));
// 		if (!arrowComp)
// 		{
// 			return;
// 		}
//             
// 		if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000.0f,
// 			ECC_Visibility, cParams))
// 		{
// 			FVector mouseWP = hitRes.Location;
// 			if(preSeg && preSeg->stationBUsedDir != FVector::ZeroVector)
// 			{
// 				seg->points.Add(seg->stationA->GetActorLocation());
// 				seg->points.Add(seg->stationA->GetActorLocation() + preSeg->stationBUsedDir * railOutLength);
// 				seg->points.Add(mouseWP);
//
// 				seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 				seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 				seg->tangents.Add((seg->points[0] - seg->points[2]).GetSafeNormal());
// 				// // 输出stationBInDir
// 				// UE_LOG(LogTemp, Warning, TEXT("[ProceeEndWithMouse]stationBUsedDir: %s"), *preSeg->stationBUsedDir.ToString());
// 			}
// 			else
// 			{
// 				FVector station2Mouse = mouseWP - seg->stationA->GetActorLocation();
// 				station2Mouse.Normalize();
// 				FVector stationForward = arrowComp->GetForwardVector();
// 				float DotProduct = FVector::DotProduct(station2Mouse, stationForward);
// 				if (DotProduct > 0)
// 				{
// 					seg->points.Add(seg->stationA->GetActorLocation());
// 					seg->points.Add(seg->stationA->GetActorLocation() + stationForward * railOutLength);
// 					seg->points.Add(mouseWP);
//
// 					seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 					seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 					seg->tangents.Add((seg->points[0] - seg->points[2]).GetSafeNormal());
// 				}
// 				else
// 				{
// 					seg->points.Add(seg->stationA->GetActorLocation());
// 					seg->points.Add(seg->stationA->GetActorLocation() - stationForward * railOutLength);
// 					seg->points.Add(mouseWP);
//
// 					seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 					seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 					seg->tangents.Add((seg->points[0] - seg->points[2]).GetSafeNormal());
// 				}	
// 			}
// 		}
// 		
// 		// 贝塞尔曲线插值
// 		GenerateInterPoint(seg, 1, seg->points[1] - seg->points[0],
// 			2, seg->points[0] - seg->points[2]);
// 	}
// }

// void ACxxRailLine::ProceeEndWithStation(UCxxRailSegment* seg, int segIdx)
// {
// 	if(seg->stationA && seg->stationB)
// 	{
// 		UActorComponent* compA = seg->stationA->GetComponentByClass(UArrowComponent::StaticClass());
// 		UActorComponent* compB = seg->stationB->GetComponentByClass(UArrowComponent::StaticClass());
//
// 		if(!compA || !compB) return; // TODO:为什么会获取不到呢?
// 		
// 		UArrowComponent* arrowCompA = Cast<UArrowComponent>(compA);
// 		UArrowComponent* arrowCompB = Cast<UArrowComponent>(compB);
// 		if(!arrowCompA || !arrowCompB) return;
//
// 		// FVector station2station = seg->stationB->GetActorLocation() - seg->stationA->GetActorLocation();
// 		// station2station.Normalize();
//
// 		// stationA方向，如果有上一段，则不能复用同一个方向.
// 		if(bStationArrowDircArray[segIdx - 1])
// 		{
// 			seg->points.Add(seg->stationA->GetActorLocation());
// 			seg->points.Add(seg->stationA->GetActorLocation() - arrowCompA->GetForwardVector() * railOutLength);
//
// 			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 		}else
// 		{
// 			seg->points.Add(seg->stationA->GetActorLocation());
// 			seg->points.Add(seg->stationA->GetActorLocation() + arrowCompA->GetForwardVector() * railOutLength);
//
// 			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
// 		}
//
// 		// stationB方向
// 		if(bStationArrowDircArray[segIdx])
// 		{
// 			seg->points.Add(seg->stationB->GetActorLocation() + arrowCompB->GetForwardVector() * railOutLength);
// 			seg->points.Add(seg->stationB->GetActorLocation());
//
// 			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
// 			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
// 		}else
// 		{
// 			seg->points.Add(seg->stationB->GetActorLocation() - arrowCompB->GetForwardVector() * railOutLength);
// 			seg->points.Add(seg->stationB->GetActorLocation());
//
// 			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
// 			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
// 		}
// 		
// 		// 贝塞尔曲线插值
// 		GenerateInterPoint(seg, 1, seg->points[1] - seg->points[0],
// 			2, seg->points[2] - seg->points[3]);
// 	}
// }
//
// void ACxxRailLine::GenerateInterPoint(UCxxRailSegment* seg, int startPointIdx, FVector startPointTangent, 
// 	int endPointIdx, FVector endPointTangent)
// {
// 	FVector P0 = seg->points[startPointIdx];
// 	FVector P3 = seg->points[endPointIdx];
//
// 	FVector P1 = P0 + startPointTangent.GetSafeNormal() * tangentIntensity;
// 	FVector P2 = P3 + endPointTangent.GetSafeNormal() * tangentIntensity;
// 	
// 	P1.Z = P0.Z;
// 	P2.Z = P3.Z;
//
// 	TArray<FVector> interpolatedPoints;
// 	TArray<FVector> tangentDirections;
// 	
// 	for (int i = 0; i <= insertRailPointNum; ++i)
// 	{
// 		float t = i / (float)insertRailPointNum;
// 		FVector point = FMath::Pow(1 - t, 3) * P0 + 3 * FMath::Pow(1 - t, 2) * t * P1 +
// 						3 * (1 - t) * FMath::Pow(t, 2) * P2 + FMath::Pow(t, 3) * P3;
// 		FVector tangent = 3 * FMath::Pow(1 - t, 2) * (P1 - P0) + 6 * (1 - t) * t * (P2 - P1) +
// 						  3 * FMath::Pow(t, 2) * (P3 - P2);
// 		tangent.Normalize();
// 		interpolatedPoints.Add(point);
// 		tangentDirections.Add(tangent);
// 	}
//
// 	interpolatedPoints.RemoveAt(0);
// 	tangentDirections.RemoveAt(0);
// 	interpolatedPoints.RemoveAt(interpolatedPoints.Num() - 1);
// 	tangentDirections.RemoveAt(tangentDirections.Num() - 1);
// 	
// 	seg->points.Insert(interpolatedPoints, startPointIdx + 1);
// 	seg->tangents.Insert(tangentDirections, startPointIdx + 1);
// 	
// 	// TODO:这里的逻辑其实是生成的有点复杂了，完全可以简化，不过考虑到可能不用Bizer，所以先不优化.
// 	seg->tangents[seg->points.Num() - 2] = -seg->tangents[seg->points.Num() - 2];
// 	seg->tangents[seg->points.Num() - 1] = -seg->tangents[seg->points.Num() - 1];
// }

// void ACxxRailLine::UpdataSegment(UCxxRailSegment* seg)
// {
// 	if(!seg->points.IsEmpty())
// 	{
// 		seg->points.Empty();;
// 	}
// 	if(!seg->tangents.IsEmpty())
// 	{
// 		seg->tangents.Empty();
// 	}
// 	// ProceeEndWithStation(seg, railSegments.Num() > 1 ? railSegments[railSegments.Num() - 2] : nullptr);
// }

// void ACxxRailLine::FinishEdit()
// {
// 	// isEdit = false;
//
// 	// 遍历segments，在每个segments的两点之间生成一个spline mesh component
// 	if(!railLineBaseClass || !railLineBaseMesh)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("railLineBaseClass or railLineBaseMesh is nullptr"));
// 		return;
// 	}
// 	
// 	for(auto seg : railSegments)
// 	{
// 		for(int i = 0; i < seg->points.Num(); i++)
// 		{
// 			if(i == seg->points.Num() - 1)
// 			{
// 				break;
// 			}
// 			USplineMeshComponent* spMeshComp = NewObject<USplineMeshComponent>(this, railLineBaseClass);
// 			// spMeshComp->SetStaticMesh(railLineBaseMesh);
// 			spMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 			
// 			// 设置参数
// 			// TODO:这里是直接设置了一个高度
// 			spMeshComp->SetStartPosition(seg->points[i] + FVector(0, 0, 50));
// 			spMeshComp->SetStartTangent(seg->tangents[i]);
// 			spMeshComp->SetEndPosition(seg->points[i + 1] + FVector(0, 0, 50));
// 			spMeshComp->SetEndTangent(seg->tangents[i + 1]);
// 			spMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
// 			
			// spMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
// 			spMeshComp->RegisterComponent();
//
// 			meshCompName2Seg.Add(spMeshComp, seg);
// 			railLineMeshComponents.Add(spMeshComp);
// 		}
// 	}
//
// 	// 记录距离决定当前point是否放置支撑柱
// 	float distance = 0.f;
// 	// 每个点生成一个支撑柱
// 	for(auto seg : railSegments)
// 	{
// 		for(int i = 1; i < seg->points.Num() - 1; i++)
// 		{
// 			distance += (seg->points[i] - seg->points[i - 1]).Size();
// 			if(distance <= pillarIntervalDist)
// 			{
// 				continue;
// 			}
// 			
// 			UStaticMeshComponent* pillarMeshComp = NewObject<UStaticMeshComponent>(this, railLinePillarClass);
// 			// pillarMeshComp->SetStaticMesh(railLinePillarMesh);
// 			pillarMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 			pillarMeshComp->SetRelativeLocation(seg->points[i]);
// 			pillarMeshComp->SetRelativeScale3D(FVector(0.5, 0.5, 0.37));
// 			pillarMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
// 			
// 			// 设置支撑住按切线旋转
// 			pillarMeshComp->SetWorldRotation(seg->tangents[i].Rotation());
// 			pillarMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
// 			pillarMeshComp->RegisterComponent();
//
// 			meshCompName2Seg.Add(pillarMeshComp, seg);
// 			railLineMeshComponents.Add(pillarMeshComp);
// 			distance = 0;
// 		}
// 	}
// 	
// 	// Debug point和tangent
// 	// VisiablePointAndTangent();
// }

// void ACxxRailLine::VisiablePointAndTangent()
// {
// 	// 遍历所有的URailSegment，遍历points绘制dubug球，tangents绘制debug方向
// 	for(auto seg : railSegments)
// 	{
// 		for(int i = 0; i < seg->points.Num(); i++)
// 		{
// 			DrawDebugSphere(GetWorld(), seg->points[i], 5, 4, FColor::Red, true, 100.f,
// 				0, 4.f);
// 			DrawDebugDirectionalArrow(GetWorld(), seg->points[i], seg->points[i] + seg->tangents[i] * 15,
// 				4, FColor::Green, true, 100.f, 0, 4.f);
// 		}
// 	}
// }

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
		controlPoint1->RegisterComponent();
		UCxxPathControlPoint* controlPoint2 = NewObject<UCxxPathControlPoint>(this);
		controlPoint2->RegisterComponent();
		UCxxPathControlPoint* controlPoint3 = NewObject<UCxxPathControlPoint>(this);
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
		
		seg1->Init(controlPoint1, controlPoint2);
		seg2->Init(controlPoint2, controlPoint3);
		
		railSegments.Add(seg1);
		railSegments.Add(seg2);

		curSelectedPoint = controlPoint3;
	}
	// 直接添加三个站点
	else
	{
		if(!curSelectedPoint) return nullptr;
		UCxxPathControlPoint* controlPoint1 = NewObject<UCxxPathControlPoint>(this);
		controlPoint1->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
		controlPoint1->RegisterComponent();
		UCxxPathControlPoint* controlPoint2 = NewObject<UCxxPathControlPoint>(this);
		controlPoint2->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
		controlPoint2->RegisterComponent();
		UCxxPathControlPoint* controlPoint3 = NewObject<UCxxPathControlPoint>(this);
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
		
		bIsForward?curSelectedPoint->nextSegment->Init(controlPoint3 ,curSelectedPoint->nextSegment->endPoint):
		curSelectedPoint->preSegment->Init(curSelectedPoint->preSegment->startPoint, controlPoint1);
		// curSelectedPoint->preSegment->SetEndPoint(controlPoint1);
		bIsForward?controlPoint1->SetSegment(seg1, seg2):controlPoint1->SetSegment(curSelectedPoint->preSegment, seg1);
		bIsForward?controlPoint2->SetSegment(seg2, seg3):controlPoint2->SetSegment(seg1, seg2);
		bIsForward?controlPoint3->SetSegment(seg3, curSelectedPoint->nextSegment):controlPoint3->SetSegment(seg2, seg3);
		bIsForward?curSelectedPoint->SetSegment(curSelectedPoint->preSegment, seg1):curSelectedPoint->SetSegment(seg3, curSelectedPoint->nextSegment);
		
		bIsForward?seg1->Init(curSelectedPoint, controlPoint1):seg1->Init(controlPoint1, controlPoint2);
		bIsForward?seg2->Init(controlPoint1, controlPoint2):seg2->Init(controlPoint2, controlPoint3);
		bIsForward?seg3->Init(controlPoint2, controlPoint3):seg3->Init(controlPoint3, curSelectedPoint);

		// curSelectedPoint = controlPoint3;

		if(insertSeg && !bIsForward)
		{
			insertSeg->SetStartPoint(curSelectedPoint);
			// curSelectedPoint->nextSegment = insertSeg;
		}
		
		railSegments.Add(seg1);
		railSegments.Add(seg2);
		railSegments.Add(seg3);
	}
	
	// 更新段
	// if(idx != 0 && idx != stations.Num())
	// {
	// 	UCxxRailSegment* curSeg = railSegments[idx - 1];
	// 	curSeg->stationB = station;
	// 	UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
	// 	newSeg->init(station, stations[idx]);
	// 	railSegments.Insert(newSeg, idx);
	// }
	// else if(idx == 0)
	// {
	// 	if(!stations.IsEmpty())
	// 	{
	// 		UCxxRailSegment* curSeg = railSegments[idx];
	// 		curSeg->stationB = station;
	// 		UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
	// 		newSeg->init(station, stations[idx]);
	// 		railSegments.Insert(newSeg, idx + 1);			
	// 	}
	// }
	// else if(idx == stations.Num())
	// {
	// 	UCxxRailSegment* curSeg = railSegments[idx];
	// 	curSeg->stationB = station;
	// 	UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
	// 	newSeg->init(station, nullptr);
	// 	railSegments.Add(newSeg);
	// }

	
	// 插入站点
	// if(idx == stations.Num())
	// {
	// 	stations.Add(station);de
	// 	// bStationArrowDircArray.Add(bIsArrowDir);
	// }else
	// {
	// 	stations.Insert(station, idx);
	// 	// bStationArrowDircArray.Insert(bIsArrowDir, idx);
	// }
	stations.Add(station);
	return curSelectedPoint;
}

UCxxPathControlPoint* ACxxRailLine::DeleteStation(ACxxStation* station)
{
	if(debugCount >= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("debugCount = 1"));
		debugCount = 0;
	}
	debugCount++;
	if(!curSelectedPoint) return nullptr;
	// 是从修改-从站点修改过来的
	if(curSelectedPoint->GetPointType() == StationPoint)
	{
		if(curSelectedPoint->station == station)
		{
			// 删除该站点并修改为手动控制点
			stations.Remove(station);
			station->SwitchOutlineEffect();

			// 修改当前点的属性
			curSelectedPoint->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
			curSelectedPoint->SetPointType(PointType::ManualPoint);
			// curSelectedPoint->SetVisible(true);

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
		
		// while(true)
		// {
		// 	if(!curSelectedPoint) break;
		// 	if(curSelectedPoint->GetPointType() != StationPoint)
		// 	{
		// 		controlPoints.Remove(curSelectedPoint);
		// 		curSelectedPoint->DestroyComponent();
		// 		railSegments.Remove(curSelectedPoint->preSegment);
		// 		curSelectedPoint->preSegment->Destroy();
		// 		curSelectedPoint = curSelectedPoint->preSegment->startPoint;
		// 	}
		// 	else
		// 	{
		// 		controlPoints.Remove(curSelectedPoint);
		// 		curSelectedPoint->DestroyComponent();
		// 		railSegments.Remove(curSelectedPoint->preSegment);
		// 		curSelectedPoint->preSegment->Destroy();
		// 		curSelectedPoint = curSelectedPoint->preSegment->startPoint;
		// 		curSelectedPoint->SetPointType(ManualPoint);
		// 		break;
		// 	}
		// }
	}
	return curSelectedPoint;
}

// int ACxxRailLine::DeleteStation(ACxxStation* station)
// {
// 	int idx = stations.Find(station);
// 	if(idx != INDEX_NONE)
// 	{
// 		// 检查是否有合法的idx +1和idx - 1
// 		if(idx + 1 < railSegments.Num())
// 		{
// 			railSegments[idx + 1]->stationA = railSegments[idx]->stationA;
// 		}
// 		stations.RemoveAt(idx);
// 		railSegments.RemoveAt(idx);
// 		bStationArrowDircArray.RemoveAt(idx);
// 	}
// 	return idx;
// }


// Called when the game starts or when spawned
void ACxxRailLine::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACxxRailLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(isEdit)
	// {
	// 	UpdateRailLine();
	// 	DrawVisableLine();	
	// }
}

// void ACxxRailLine::DrawVisableLine()
// {
// 	for(auto seg : railSegments)
// 	{
// 		for(int i = 0; i < seg->points.Num(); i++)
// 		{
// 			if(i == seg->points.Num() - 1)
// 			{
// 				break;
// 			}
// 			DrawDebugLine(GetWorld(), seg->points[i], seg->points[i + 1], railLineColor, false, 0.f, 0, 10.f);
// 		}
// 	}
// }

// void ACxxRailLine::UpdateRailLine()
// {
// 	// 遍历Segment并更新
// 	for(int i = 0; i < railSegments.Num(); i++)
// 	{
// 		UCxxRailSegment* seg = railSegments[i];
// 		if(!seg->points.IsEmpty())
// 		{
// 			seg->points.Empty();
// 		}
// 		if(!seg->tangents.IsEmpty())
// 		{
// 			seg->tangents.Empty();
// 		}
// 		if(!seg->pointType.IsEmpty())
// 		{
// 			seg->pointType.Empty();
// 		}
// 		
// 		if(seg->stationA && seg->stationB)
// 		{
// 			ProceeEndWithStation(seg, i);
// 		}
// 	}
	
	// if(!segment->points.IsEmpty())
	// {
	// 	segment->points.Empty();
	// }
	// if(!segment->tangents.IsEmpty())
	// {
	// 	segment->tangents.Empty();
	// }
	// if(!segment->pointType.IsEmpty())
	// {
	// 	segment->pointType.Empty();
	// }
	//
	// if(segment->stationA && segment->stationB)
	// {
	// 	ProceeEndWithStation(segment,
	// 		railSegments.Find(segment) > 0 ? railSegments[railSegments.Find(segment) - 1] : nullptr,
	// 		railSegments.Find(segment) < railSegments.Num() - 1 ? railSegments[railSegments.Find(segment) + 1] : nullptr);
	// }
	
	// if(!railSegments.IsEmpty())
	// {
	// 	UCxxRailSegment* endSeg = railSegments[railSegments.Num() - 1];
	// 	if(!endSeg->points.IsEmpty())
	// 	{
	// 		endSeg->points.Empty();
	// 	}
	// 	if(!endSeg->tangents.IsEmpty())
	// 	{
	// 		endSeg->tangents.Empty();
	// 	}
	// 	
	// 	if(!endSeg)
	// 	{
	// 		ProceeEndWithMouse(endSeg, railSegments.Num() > 1 ? railSegments[railSegments.Num() - 2] : nullptr);
	// 	}else
	// 	{
	// 		ProceeEndWithStation(endSeg, railSegments.Num() > 1 ? railSegments[railSegments.Num() - 2] : nullptr);
	// 	}
	// }
// }
//
// void ACxxRailLine::UpdateRailLineSegmentWithMouseLocation(UCxxRailSegment* seg, FVector mouseLocation)
// {
// 	if(curEditSegment && curEditSegment != seg){
// 		
// 	}
// 	curEditSegment = seg;
// 	
// 	// 更新当前段
// }
//
// void ACxxRailLine::SetRailColor(FColor color)
// {
// 	railLineColor = color;
// }

void ACxxRailLine::SwitchHightLightEffect()
{
	bIsHighlighted = !bIsHighlighted;
	for(auto seg : railSegments)
	{
		if(seg->splineMesh)
		{
			seg->splineMesh->SetRenderCustomDepth(bIsHighlighted);
		}
		// comp->SetRenderCustomDepth(bIsHighlighted);
	}
	for(auto station:stations)
	{
		station->SwitchOutlineEffect();
	}
	for(auto point:controlPoints)
	{
		point->SetVisible(bIsHighlighted);
	}
}

