// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxRailLineSegment.h"

#include "CxxScenePCGTool.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCxxRailLineSegment::UCxxRailLineSegment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// static ConstructorHelpers::FObjectFinder<UBlueprint> railLineBaseClassFinder(
	// 	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLineBase.RailLineBase'"));
	// if (railLineBaseClassFinder.Succeeded()) 
	// {
	// 	railLineBaseClass = railLineBaseClassFinder.Object->GeneratedClass;
	// }
	//
	// static ConstructorHelpers::FObjectFinder<UBlueprint> railLinePillarClassFinder(
	// 	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLinePillar.RailLinePillar'"));
	// if (railLinePillarClassFinder.Succeeded())
	// {
	// 	railLinePillarBaseClass = railLinePillarClassFinder.Object->GeneratedClass;
	// }
	//
	// static ConstructorHelpers::FObjectFinder<UBlueprint> railLinePillarRepeatClassFinder(
	// TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLinePillar_2.RailLinePillar_2'"));
	// if (railLinePillarRepeatClassFinder.Succeeded())
	// {
	// 	railLinePillarRepeatClass = railLinePillarRepeatClassFinder.Object->GeneratedClass;
	// }
	// ...
}

void UCxxRailLineSegment::Init(UCxxPathControlPoint* start,
	UCxxPathControlPoint* end, float length, UClass* b, UClass* pb, UClass* pr)
{
	railLineBaseClass = b;
	railLinePillarBaseClass = pb;
	railLinePillarRepeatClass = pr;
	
	startPoint = start;
	endPoint = end;
	pillarDensityLength = length;

	if(!splineMesh)
	{
		splineMesh = NewObject<USplineMeshComponent>(this, railLineBaseClass);
		// splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// splineMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
		splineMesh->RegisterComponent();
		splineMesh->SetRenderCustomDepth(true);
	}

	if (!spline)
	{
		spline = NewObject<USplineComponent>(this);
		spline->RegisterComponent();
	} 

	SetStartPoint(startPoint);
	SetEndPoint(endPoint);
}

void UCxxRailLineSegment::Destroy()
{
	if(splineMesh)
	{
		splineMesh->DestroyComponent();
		splineMesh = nullptr;
	}
	if (spline)
	{
		spline->DestroyComponent();
		spline = nullptr;
	}
	for (auto pillar:pillarMeshBaseArray)
	{
		pillar->DestroyComponent();
	}
	for (auto pillar:pillarMeshRepeatArray)
	{
		pillar->DestroyComponent();
	}
	this->DestroyComponent();
}

void UCxxRailLineSegment::SetStartPoint(UCxxPathControlPoint* point)
{
	startPoint = point;
	if(splineMesh)
	{
		splineMesh->SetStartPosition(startPoint->location + FVector(0, 0, 150));
		splineMesh->SetStartTangent(startPoint->tangent);

		splineMesh->UpdateMesh();
	}
	if (spline)
	{
		spline->SetLocationAtSplinePoint(0, startPoint->location, ESplineCoordinateSpace::World);
		spline->SetTangentAtSplinePoint(0, startPoint->tangent, ESplineCoordinateSpace::World);
	
		spline->UpdateSpline();
	}
	UpdatePillarMesh();
	ComputeCollisionBlock();
}

void UCxxRailLineSegment::SetEndPoint(UCxxPathControlPoint* point)
{
	endPoint = point;
	if(splineMesh)
	{
		splineMesh->SetEndPosition(endPoint->location + FVector(0, 0, 150));
		splineMesh->SetEndTangent(endPoint->tangent);

		splineMesh->UpdateMesh();
	}
	if (spline)
	{
		spline->SetLocationAtSplinePoint(1, endPoint->location, ESplineCoordinateSpace::World);
		spline->SetTangentAtSplinePoint(1, endPoint->tangent, ESplineCoordinateSpace::World);

		spline->UpdateSpline();
	}
	UpdatePillarMesh();
	ComputeCollisionBlock();
}

void UCxxRailLineSegment::UpdatePillarMesh()
{
	if (startPoint->type == StationPoint || endPoint->type == StationPoint) return;
	float preLength = 0;
	if (startPoint->preSegment)
	{
		auto prePoint = startPoint;
		// 遍历计算所有之前段的总长
		while (prePoint->type != PointType::FixedPoint && prePoint->type != PointType::StationPoint && prePoint->preSegment)
		{
			preLength += prePoint->preSegment->spline->GetSplineLength();
			// 输出preLength
			prePoint = prePoint->preSegment->startPoint;
		}
	}
	auto length = pillarDensityLength - (preLength - floor(preLength / pillarDensityLength) * pillarDensityLength);
	int idx = 0;
	for (float currentLength = 0.0; currentLength + length < spline->GetSplineLength();)
	{
		currentLength = currentLength + length;
		UStaticMeshComponent* pillarBase = nullptr;
		UStaticMeshComponent* pillarMeshRepeat = nullptr;
		if (idx < pillarMeshBaseArray.Num())
		{
			pillarBase = pillarMeshBaseArray[idx];
			pillarMeshRepeat = pillarMeshRepeatArray[idx];
		}
		else
		{
			pillarBase = NewObject<UStaticMeshComponent>(this, railLinePillarBaseClass);
			pillarBase->RegisterComponent();
			pillarMeshBaseArray.Add(pillarBase);
			pillarMeshRepeat = NewObject<UStaticMeshComponent>(this, railLinePillarRepeatClass);
			pillarMeshRepeat->RegisterComponent();
			pillarMeshRepeatArray.Add(pillarMeshRepeat);
		}
		FVector splineLoc = spline->GetLocationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World)
		+ FVector(0, 0, 150);
		// 设置repeat参数
		FVector repeatLoc = splineLoc + FVector(0, 0, -62.5);
		pillarMeshRepeat->SetWorldLocation(repeatLoc);

		// 设置base参数，动态检测地面
		float lengthFix = 10.0f;
		FHitResult hitRes;
		FCollisionQueryParams cParams;
		if (GetWorld()->LineTraceSingleByChannel(hitRes, splineLoc + FVector(0, 0, 10000),
			splineLoc + FVector(0, 0, -10000),
			ECC_GameTraceChannel3, cParams))
		{
			FVector hitLoc = hitRes.Location;
			// TODO:这里的hitRes有一些误差
			pillarBase->SetWorldLocation(FVector(repeatLoc.X, repeatLoc.Y, hitLoc.Z - lengthFix));
			float dist = (hitLoc - (splineLoc + FVector(0, 0, -62.5))).Length() + lengthFix;
			pillarBase->SetRelativeScale3D(FVector(1, 1, 0.71875 * dist / 100.0f));
		}else
		{
			pillarBase->SetWorldLocation(splineLoc);
		}
		pillarBase->SetWorldRotation(spline->GetRotationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World));
		pillarMeshRepeat->SetWorldRotation(spline->GetRotationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World));
		idx++;
		length = pillarDensityLength;
	}
	while (idx < pillarMeshBaseArray.Num())
	{
		pillarMeshBaseArray[idx]->DestroyComponent();
		pillarMeshBaseArray.RemoveAt(idx);
		pillarMeshRepeatArray[idx]->DestroyComponent();
		pillarMeshRepeatArray.RemoveAt(idx);
	}
}

// Called when the game starts
void UCxxRailLineSegment::BeginPlay()
{
	Super::BeginPlay();
	
	// 获取scenePCGTool
	TArray<AActor*> foundPCGTool;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxScenePCGTool::StaticClass(), foundPCGTool); 
	if (foundPCGTool.Num() > 0)
	{
		scenePCGTool = Cast<ACxxScenePCGTool>(foundPCGTool[0]);
	}
	
}


// Called every frame
void UCxxRailLineSegment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// MeshBuildTick();
	// ...
}

// Amanatides and Woo算法,本质是不断寻找下一个横线和竖线
void UCxxRailLineSegment::ComputeCollisionBlock()
{
	if(!scenePCGTool) return;
	TArray<FVector2D> newCollisionBlockArray;
	int samplerNum = 10;
	float splineLength = spline->GetSplineLength();
	for (float i = 0.0f; i < samplerNum; i += 1.0f)
	{
		FVector point1 = spline->GetLocationAtDistanceAlongSpline(splineLength * i / static_cast<float>(samplerNum), ESplineCoordinateSpace::World);
		FVector point2 = spline->GetLocationAtDistanceAlongSpline(splineLength * (i + 1) / static_cast<float>(samplerNum), ESplineCoordinateSpace::World);

		FVector2D p0(point1.X, point1.Y);
		FVector2D p1(point2.X, point2.Y);
		FVector2D dir = p1 - p0;

		int x0 = FMath::FloorToInt((p0.X) / scenePCGTool->gridSize);
		int y0 = FMath::FloorToInt((p0.Y) / scenePCGTool->gridSize);
		int x1 = FMath::FloorToInt((p1.X) / scenePCGTool->gridSize);
		int y1 = FMath::FloorToInt((p1.Y) / scenePCGTool->gridSize);

		int stepX = (dir.X >= 0) ? 1 : -1;
		int stepY = (dir.Y >= 0) ? 1 : -1;

		float dx = FMath::Abs(dir.X);
		float dy = FMath::Abs(dir.Y);

		float tDeltaX = (dx == 0) ? BIG_NUMBER : scenePCGTool->gridSize / dx;
		float tDeltaY = (dy == 0) ? BIG_NUMBER : scenePCGTool->gridSize / dy;

		float nextGridX = (x0 + (stepX > 0 ? 1 : 0)) * scenePCGTool->gridSize;
		float nextGridY = (y0 + (stepY > 0 ? 1 : 0)) * scenePCGTool->gridSize;

		// TODO:在dy非常小的时候会有问题
		float tMaxX = (dx == 0) ? BIG_NUMBER : FMath::Abs((nextGridX - p0.X) / dir.X);
		float tMaxY = (dy == 0) ? BIG_NUMBER : FMath::Abs((nextGridY - p0.Y) / dir.Y);

		int x = x0;
		int y = y0;
		newCollisionBlockArray.AddUnique(FVector2D(x, y));

		// while ((dir.X >= 0) ? x < x1 : x > x1 || (dir.Y >= 0) ? y < y1 : y > y1)
		int count = scenePCGTool->gridXNum + scenePCGTool->gridYNum + 10;
		while (x != x1 || y != y1)
		{
			if (tMaxX < tMaxY)
			{
				tMaxX += tDeltaX;
				x += stepX;
			}
			else
			{
				tMaxY += tDeltaY;
				y += stepY;
			}
			if (count-- <= 0) break;
			if (x < scenePCGTool->gridXNum && y < scenePCGTool->gridYNum && x >= 0 && y >= 0)
			{
				newCollisionBlockArray.AddUnique(FVector2D(x, y));
			}
		}
	}
	AddToCollisionBlockArray(newCollisionBlockArray);
}


void UCxxRailLineSegment::AddToCollisionBlockArray(TArray<FVector2D> newCollisionBlockArray)
{
	if(!scenePCGTool) return;
	TArray<FVector2D> addArray;
	TArray<FVector2D> subArray;
	for (auto block: newCollisionBlockArray)
	{
		if (!collisionBlockArray.Contains(block)) addArray.Add(block);
	}

	for (auto block: collisionBlockArray)
	{
		if (!newCollisionBlockArray.Contains(block)) subArray.Add(block);
	}
	collisionBlockArray = newCollisionBlockArray;

	scenePCGTool->UpdateCollisionCount(addArray, subArray);
}

void UCxxRailLineSegment::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if(!scenePCGTool) return;
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	TArray<FVector2D> addArray;
	TArray<FVector2D> subArray = collisionBlockArray;
	scenePCGTool->UpdateCollisionCount(addArray, subArray);
}

void UCxxRailLineSegment::SwitchHightLightEffect(bool bIsHighlighted)
{
	if (splineMesh) splineMesh->SetRenderCustomDepth(bIsHighlighted);
	for (auto mesh:pillarMeshBaseArray)
	{
		if (mesh) mesh->SetRenderCustomDepth(bIsHighlighted);
	}

	for (auto mesh:pillarMeshRepeatArray)
	{
		if (mesh) mesh->SetRenderCustomDepth(bIsHighlighted);
	}
}

