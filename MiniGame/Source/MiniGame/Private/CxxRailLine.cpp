// Fill out your copyright notice in the Description page of Project Settings.

#include "CxxMonoRailTrain.h"
#include "CxxRailLine.h"

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
}

ACxxRailLine::ACxxRailLine(FColor color)
{
	railLineColor = color;
}

void ACxxRailLine::Init()
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

void ACxxRailLine::ProceeEndWithStation(UCxxRailSegment* seg, int segIdx)
{
	if(seg->stationA && seg->stationB)
	{
		UActorComponent* compA = seg->stationA->GetComponentByClass(UArrowComponent::StaticClass());
		UActorComponent* compB = seg->stationB->GetComponentByClass(UArrowComponent::StaticClass());

		if(!compA || !compB) return; // TODO:为什么会获取不到呢?
		
		UArrowComponent* arrowCompA = Cast<UArrowComponent>(compA);
		UArrowComponent* arrowCompB = Cast<UArrowComponent>(compB);
		if(!arrowCompA || !arrowCompB) return;

		// FVector station2station = seg->stationB->GetActorLocation() - seg->stationA->GetActorLocation();
		// station2station.Normalize();

		// stationA方向，如果有上一段，则不能复用同一个方向.
		if(bStationArrowDircArray[segIdx - 1])
		{
			seg->points.Add(seg->stationA->GetActorLocation());
			seg->points.Add(seg->stationA->GetActorLocation() - arrowCompA->GetForwardVector() * railOutLength);

			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
		}else
		{
			seg->points.Add(seg->stationA->GetActorLocation());
			seg->points.Add(seg->stationA->GetActorLocation() + arrowCompA->GetForwardVector() * railOutLength);

			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
			seg->tangents.Add((seg->points[1] - seg->points[0]).GetSafeNormal());
		}

		// stationB方向
		if(bStationArrowDircArray[segIdx])
		{
			seg->points.Add(seg->stationB->GetActorLocation() + arrowCompB->GetForwardVector() * railOutLength);
			seg->points.Add(seg->stationB->GetActorLocation());

			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
		}else
		{
			seg->points.Add(seg->stationB->GetActorLocation() - arrowCompB->GetForwardVector() * railOutLength);
			seg->points.Add(seg->stationB->GetActorLocation());

			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
			seg->tangents.Add((seg->points[2] - seg->points[3]).GetSafeNormal());
		}
		
		// 贝塞尔曲线插值
		GenerateInterPoint(seg, 1, seg->points[1] - seg->points[0],
			2, seg->points[2] - seg->points[3]);
	}
}

void ACxxRailLine::GenerateInterPoint(UCxxRailSegment* seg, int startPointIdx, FVector startPointTangent, 
	int endPointIdx, FVector endPointTangent)
{
	FVector P0 = seg->points[startPointIdx];
	FVector P3 = seg->points[endPointIdx];

	FVector P1 = P0 + startPointTangent.GetSafeNormal() * bizerTangentIntensity;
	FVector P2 = P3 + endPointTangent.GetSafeNormal() * bizerTangentIntensity;
	
	P1.Z = P0.Z;
	P2.Z = P3.Z;

	TArray<FVector> interpolatedPoints;
	TArray<FVector> tangentDirections;
	
	for (int i = 0; i <= insertRailPointNum; ++i)
	{
		float t = i / (float)insertRailPointNum;
		FVector point = FMath::Pow(1 - t, 3) * P0 + 3 * FMath::Pow(1 - t, 2) * t * P1 +
						3 * (1 - t) * FMath::Pow(t, 2) * P2 + FMath::Pow(t, 3) * P3;
		FVector tangent = 3 * FMath::Pow(1 - t, 2) * (P1 - P0) + 6 * (1 - t) * t * (P2 - P1) +
						  3 * FMath::Pow(t, 2) * (P3 - P2);
		tangent.Normalize();
		interpolatedPoints.Add(point);
		tangentDirections.Add(tangent);
	}

	interpolatedPoints.RemoveAt(0);
	tangentDirections.RemoveAt(0);
	interpolatedPoints.RemoveAt(interpolatedPoints.Num() - 1);
	tangentDirections.RemoveAt(tangentDirections.Num() - 1);
	
	seg->points.Insert(interpolatedPoints, startPointIdx + 1);
	seg->tangents.Insert(tangentDirections, startPointIdx + 1);
	
	// TODO:这里的逻辑其实是生成的有点复杂了，完全可以简化，不过考虑到可能不用Bizer，所以先不优化.
	seg->tangents[seg->points.Num() - 2] = -seg->tangents[seg->points.Num() - 2];
	seg->tangents[seg->points.Num() - 1] = -seg->tangents[seg->points.Num() - 1];
}

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

void ACxxRailLine::FinishEdit()
{
	// isEdit = false;

	// 遍历segments，在每个segments的两点之间生成一个spline mesh component
	if(!railLineBaseClass || !railLineBaseMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("railLineBaseClass or railLineBaseMesh is nullptr"));
		return;
	}
	
	for(auto seg : railSegments)
	{
		for(int i = 0; i < seg->points.Num(); i++)
		{
			if(i == seg->points.Num() - 1)
			{
				break;
			}
			USplineMeshComponent* spMeshComp = NewObject<USplineMeshComponent>(this, railLineBaseClass);
			// spMeshComp->SetStaticMesh(railLineBaseMesh);
			spMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			
			// 设置参数
			// TODO:这里是直接设置了一个高度
			spMeshComp->SetStartPosition(seg->points[i] + FVector(0, 0, 50));
			spMeshComp->SetStartTangent(seg->tangents[i]);
			spMeshComp->SetEndPosition(seg->points[i + 1] + FVector(0, 0, 50));
			spMeshComp->SetEndTangent(seg->tangents[i + 1]);
			spMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
			
			spMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			spMeshComp->RegisterComponent();

			meshCompName2Seg.Add(spMeshComp, seg);
			railLineMeshComponents.Add(spMeshComp);
		}
	}

	// 记录距离决定当前point是否放置支撑柱
	float distance = 0.f;
	// 每个点生成一个支撑柱
	for(auto seg : railSegments)
	{
		for(int i = 1; i < seg->points.Num() - 1; i++)
		{
			distance += (seg->points[i] - seg->points[i - 1]).Size();
			if(distance <= pillarIntervalDist)
			{
				continue;
			}
			
			UStaticMeshComponent* pillarMeshComp = NewObject<UStaticMeshComponent>(this, railLinePillarClass);
			// pillarMeshComp->SetStaticMesh(railLinePillarMesh);
			pillarMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			pillarMeshComp->SetRelativeLocation(seg->points[i]);
			pillarMeshComp->SetRelativeScale3D(FVector(0.5, 0.5, 0.37));
			pillarMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
			
			// 设置支撑住按切线旋转
			pillarMeshComp->SetWorldRotation(seg->tangents[i].Rotation());
			pillarMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			pillarMeshComp->RegisterComponent();

			meshCompName2Seg.Add(pillarMeshComp, seg);
			railLineMeshComponents.Add(pillarMeshComp);
			distance = 0;
		}
	}
	
	// Debug point和tangent
	// VisiablePointAndTangent();
}

void ACxxRailLine::VisiablePointAndTangent()
{
	// 遍历所有的URailSegment，遍历points绘制dubug球，tangents绘制debug方向
	for(auto seg : railSegments)
	{
		for(int i = 0; i < seg->points.Num(); i++)
		{
			DrawDebugSphere(GetWorld(), seg->points[i], 5, 4, FColor::Red, true, 100.f,
				0, 4.f);
			DrawDebugDirectionalArrow(GetWorld(), seg->points[i], seg->points[i] + seg->tangents[i] * 15,
				4, FColor::Green, true, 100.f, 0, 4.f);
		}
	}
}

void ACxxRailLine::DestroyLine()
{
	
}

void ACxxRailLine::AddStation(ACxxStation* station, int idx, bool bIsArrowDir)
{
	// 更新段
	if(idx != 0 && idx != stations.Num())
	{
		UCxxRailSegment* curSeg = railSegments[idx - 1];
		curSeg->stationB = station;
		UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
		newSeg->init(station, stations[idx]);
		railSegments.Insert(newSeg, idx);
	}
	else if(idx == 0)
	{
		if(!stations.IsEmpty())
		{
			UCxxRailSegment* curSeg = railSegments[idx];
			curSeg->stationB = station;
			UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
			newSeg->init(station, stations[idx]);
			railSegments.Insert(newSeg, idx + 1);			
		}
	}
	else if(idx == stations.Num())
	{
		UCxxRailSegment* curSeg = railSegments[idx];
		curSeg->stationB = station;
		UCxxRailSegment* newSeg = NewObject<UCxxRailSegment>(this);
		newSeg->init(station, nullptr);
		railSegments.Add(newSeg);
	}
	
	// 插入站点
	if(idx == stations.Num())
	{
		stations.Add(station);
		bStationArrowDircArray.Add(bIsArrowDir);
	}else
	{
		stations.Insert(station, idx);
		bStationArrowDircArray.Insert(bIsArrowDir, idx);
	}
}

int ACxxRailLine::DeleteStation(ACxxStation* station)
{
	int idx = stations.Find(station);
	if(idx != INDEX_NONE)
	{
		// 检查是否有合法的idx +1和idx - 1
		if(idx + 1 < railSegments.Num())
		{
			railSegments[idx + 1]->stationA = railSegments[idx]->stationA;
		}
		stations.RemoveAt(idx);
		railSegments.RemoveAt(idx);
		bStationArrowDircArray.RemoveAt(idx);
	}
	return idx;
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

void ACxxRailLine::UpdateRailLine()
{
	// 遍历Segment并更新
	for(int i = 0; i < railSegments.Num(); i++)
	{
		UCxxRailSegment* seg = railSegments[i];
		if(!seg->points.IsEmpty())
		{
			seg->points.Empty();
		}
		if(!seg->tangents.IsEmpty())
		{
			seg->tangents.Empty();
		}
		if(!seg->pointType.IsEmpty())
		{
			seg->pointType.Empty();
		}
		
		if(seg->stationA && seg->stationB)
		{
			ProceeEndWithStation(seg, i);
		}
	}
	
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
}

void ACxxRailLine::UpdateRailLineSegment(UCxxRailSegment* seg, FVector mouseLocation)
{
	if(curEditSegment && curEditSegment != seg){
		
	}
	curEditSegment = seg;
	
	// 更新当前段
}

void ACxxRailLine::SetRailColor(FColor color)
{
	railLineColor = color;
}

void ACxxRailLine::SwitchHightLightEffect()
{
	bIsHighlighted = !bIsHighlighted;
	for(auto comp : railLineMeshComponents)
	{
		comp->SetRenderCustomDepth(bIsHighlighted);
	}
	for(auto station:stations)
	{
		station->SwitchOutlineEffect();
	}
}

