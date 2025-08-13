// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxScenePCGTool.h"

// Sets default values
ACxxScenePCGTool::ACxxScenePCGTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    // ConstructorHelpers::FObjectFinder<UBlueprint>defaultBuildAClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/DefaultBuild.DefaultBuild'"));
    // if (defaultBuildAClassFinder.Succeeded())
    // {
    //     defaultBuildAClass = defaultBuildAClassFinder.Object->GeneratedClass; 
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>defaultBuildBClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/DefaultBuild_2.DefaultBuild_2'"));
    // if (defaultBuildBClassFinder.Succeeded())
    // {
    //     defaultBuildBClass = defaultBuildBClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaSYQBuildAClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/SYQ_A.SYQ_A'"));
    // if (areaSYQBuildAClassFinder.Succeeded())
    // {
    //     areaSYQBuildAClass = areaSYQBuildAClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaSYQBuildBClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/SYQ_B.SYQ_B'"));
    // if (areaSYQBuildBClassFinder.Succeeded())
    // {
    //     areaSYQBuildBBClass = areaSYQBuildBClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaSYQBuildCCClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/SYQ_B_2.SYQ_B_2'"));
    // if (areaSYQBuildCCClassFinder.Succeeded())
    // {
    //     areaSYQBuildCCClass = areaSYQBuildCCClassFinder.Object->GeneratedClass;
    // } 
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaWHQBuildAClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/WHQ_A.WHQ_A'"));
    // if (areaWHQBuildAClassFinder.Succeeded())
    // {
    //     areaWHQBuildAClass = areaWHQBuildAClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaZZQBuildBBClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/ZZQ_A.ZZQ_A'"));
    // if (areaZZQBuildBBClassFinder.Succeeded())
    // {
    //     areaZZQBuildBBClass = areaZZQBuildBBClassFinder.Object->GeneratedClass;
    // }
    //
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaWHQBuildBBClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/WHQ_A_2.WHQ_A_2'"));
    // if (areaWHQBuildBBClassFinder.Succeeded())
    // {
    //     areaWHQBuildBBClass = areaWHQBuildBBClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaWHQBuildCCClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/WHQ_A_3.WHQ_A_3'"));
    // if (areaWHQBuildCCClassFinder.Succeeded())
    // {
    //     areaWHQBuildCCClass = areaWHQBuildCCClassFinder.Object->GeneratedClass;
    // }
    //
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaXXQBuildAClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/XXQ_A.XXQ_A'"));
    // if (areaXXQBuildAClassFinder.Succeeded())
    // {
    //     areaXXQBuildAClass = areaXXQBuildAClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaXXQBuildBBClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/XXQ_A_2.XXQ_A_2'"));
    // if (areaXXQBuildBBClassFinder.Succeeded())
    // { 
    //     areaXXQBuildBBClass = areaXXQBuildBBClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaXXQBuildCCClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/XXQ_A_3.XXQ_A_3'"));
    // if (areaXXQBuildCCClassFinder.Succeeded())
    // {
    //     areaXXQBuildCCClass = areaXXQBuildCCClassFinder.Object->GeneratedClass;
    // }
    //
    // ConstructorHelpers::FObjectFinder<UBlueprint>areaZZQBuildAClassFinder(
    // TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Bulid/ZZQ_A_2.ZZQ_A_2'"));
    // if (areaZZQBuildAClassFinder.Succeeded())
    // {
    //     areaZZQBuildAClass = areaZZQBuildAClassFinder.Object->GeneratedClass;
    // }
}

// Called when the game starts or when spawned
void ACxxScenePCGTool::BeginPlay()
{
	Super::BeginPlay();
    buildMaterialA = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Materials/Station/StationTempMat.StationTempMat'"));
    buildMaterialB = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Materials/Station/StationTempMat_2.StationTempMat_2'"));
    buildMaterialC = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Materials/Station/StationTempMat_3.StationTempMat_3'"));
    buildMaterialD = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Materials/Station/StationTempMat_4.StationTempMat_4'"));
    buildMaterialE = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Materials/Station/StationTempMat_5.StationTempMat_5'"));

    SceneGenerate();
    SceneCreate();
    bBeginPlay = true;
}

// Called every frame
void ACxxScenePCGTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACxxScenePCGTool::SceneGenerate()
{
    InitArray();
    TArray<int> areaBlockSum;
    TArray<int> areaBlockNum;
    TQueue<FVector2D> queue;
    for (int i = 0; i < areaGeneratePoint.Num(); ++i)
    {
        FVector2D xy = GetXYFromXYLocation(areaGeneratePoint[i]);
        if(FMath::FloorToInt(xy.X) >= gridXNum || FMath::FloorToInt(xy.Y) >= gridYNum ||
            FMath::FloorToInt(xy.X) < 0 || FMath::FloorToInt(xy.Y) < 0)
        {
            areaVaild.Add(false);
            continue;
        }
        gridAreaFlags[FMath::FloorToInt(xy.X)][FMath::FloorToInt(xy.Y)] = i + 1;
        areaBlockSum.Add(FMath::RandRange(FMath::FloorToInt(areaGenerateBlockNumRange[i].X), FMath::FloorToInt(areaGenerateBlockNumRange[i].Y)));
        areaBlockNum.Add(0);
        TArray<FVector2D> tArray;
        queue.Enqueue(FVector2D(xy.X, xy.Y));
        tArray.Add(xy);
        areaVaild.Add(true);
        areaBlocksArray.Add(tArray);
    }
    
    while (!queue.IsEmpty())
    {
        FVector2D curPoint;
        queue.Dequeue(curPoint);

        int curX = FMath::FloorToInt(curPoint.X);
        int curY = FMath::FloorToInt(curPoint.Y);
        
            
        Algo::RandomShuffle(directions);
        bool bfirstVaildDir = false;
        for (const FVector2D& direction : directions)
        {
            if(bfirstVaildDir && FMath::RandRange(0, 100) > 20) continue;
            int x = FMath::FloorToInt(curPoint.X + direction.X);
            int y = FMath::FloorToInt(curPoint.Y + direction.Y);

            // 射线检测是否合法地形，通过高度判断
            FVector groundLoc = GetBlockLoction(x, y);
            if (groundLoc.Z < -5000) continue;

            if (x >= 0 && x < gridXNum && y >= 0 && y < gridYNum && gridAreaFlags[x][y] == 0 &&
                areaBlockNum[gridAreaFlags[curX][curY] - 1] < areaBlockSum[gridAreaFlags[curX][curY] - 1])
            {
                gridAreaFlags[x][y] = gridAreaFlags[curX][curY];
                queue.Enqueue(FVector2D(x, y));
                areaBlocksArray[gridAreaFlags[curX][curY] - 1].Add(FVector2D(x, y));
                areaBlockNum[gridAreaFlags[curX][curY] - 1]++;
                bfirstVaildDir = true;
            }
        }
    }

    // 生成区域种类
    SceneTypeGenerate();
}

void ACxxScenePCGTool::SceneTypeGenerate()
{
    TArray<TArray<int>> areaType;
    // 遍历每一个区
    TQueue<FVector2D> queue;
    TArray<int> areaBlockSum;
    TArray<int> areaBlockNum;
    
    int areaNum = areaGeneratePoint.Num();
    TArray<int> areaTypeRemainIdx;

    areaType.SetNum(areaNum);
    for (int typeIndex = 0; typeIndex < typeAreaNum.Num(); ++typeIndex)
    {
        int count = typeAreaNum[typeIndex];
        int baseNum = count / areaNum; 
        int remainNum = count % areaNum;

        // 先平均分配
        for (int areaIdx = 0; areaIdx < areaNum; ++areaIdx)
        {
            for (int i = 0; i < baseNum; ++i)
            {
                areaType[areaIdx].Add(typeIndex + 1);
            }
        }
        
        for (int i = 0; i < remainNum; ++i)
        {
            areaTypeRemainIdx.Add(typeIndex + 1);
        }
    }

    // 余数轮流随机分配
    TArray<int> shuffleIdx;
    for (int i = 0; i < areaTypeRemainIdx.Num(); ++i)
    {
        shuffleIdx.Add(i);
    }
    Algo::RandomShuffle(shuffleIdx);
    for (int i = 0; i < shuffleIdx.Num(); ++i)
    {
        int targetAreaIdx = i % areaNum;
        int typeVal = areaTypeRemainIdx[shuffleIdx[i]];
        areaType[targetAreaIdx].Add(typeVal);
    }


    int areaIDCount = 1;
    for (int i = 0; i < areaType.Num(); ++i)
    {
        for (int j = 0; j < areaType[i].Num(); ++j)
        {
            int randomIdx = FMath::RandRange(0, areaBlocksArray[i].Num() - 1);

            // 避免死循环
            int count = gridXNum * gridYNum;
            while(true)
            {
                // 随机生成一个在当前areaIdx内且和其他已有站点有一定距离的点位置（不在周围
                if(areaID[FMath::FloorToInt(areaBlocksArray[i][randomIdx].X)]
                    [FMath::FloorToInt(areaBlocksArray[i][randomIdx].Y)] == 0)
                {
                    if (count < 0) break;
                    FVector2D loc = FVector2D(areaBlocksArray[i][randomIdx].X, areaBlocksArray[i][randomIdx].Y);
                    bool bTooClose = false;
                    for (auto sLoc: stationLoc)
                    {
                        if (FMath::Abs(loc.X - sLoc.X) <= 1 || FMath::Abs(loc.Y - sLoc.Y) <= 1)
                        {
                            bTooClose = true;
                            break;
                        }
                    }
                    if (!bTooClose) break;
                }
                randomIdx = FMath::RandRange(0, areaBlocksArray[i].Num() - 1);
                if (count-- < - gridXNum * gridYNum) break;
            }
            gridTypeFlags[FMath::FloorToInt(areaBlocksArray[i][randomIdx].X)][FMath::FloorToInt(areaBlocksArray[i][randomIdx].Y)] = j;
            areaID[FMath::FloorToInt(areaBlocksArray[i][randomIdx].X)][FMath::FloorToInt(areaBlocksArray[i][randomIdx].Y)] = areaIDCount;
            stationLoc.Add(FVector2D(areaBlocksArray[i][randomIdx].X, areaBlocksArray[i][randomIdx].Y));
            stationType.Add(areaType[i][j]);
                
            areaBlockSum.Add(FMath::RandRange(5, 15));
            areaBlockNum.Add(0);
            queue.Enqueue(FVector2D(areaBlocksArray[i][randomIdx].X, areaBlocksArray[i][randomIdx].Y));
            areaIDCount++;
        }
    }
    
    while (!queue.IsEmpty())
    {
        FVector2D curPoint;
        queue.Dequeue(curPoint);

        int curX = FMath::FloorToInt(curPoint.X);
        int curY = FMath::FloorToInt(curPoint.Y);
                    
        for (const FVector2D& direction : directions)
        {
            int x = FMath::FloorToInt(curPoint.X + direction.X);
            int y = FMath::FloorToInt(curPoint.Y + direction.Y);

            if (x >= 0 && x < gridXNum && y >= 0 && y < gridYNum &&
                gridTypeFlags[x][y] == 0 &&
                areaBlockNum[areaID[curX][curY] - 1] < areaBlockSum[areaID[curX][curY] - 1])
            {
                gridTypeFlags[x][y] = gridTypeFlags[curX][curY];
                areaID[x][y] = areaID[curX][curY];
                queue.Enqueue(FVector2D(x, y));
                areaBlockNum[areaID[curX][curY] - 1]++;
            }
        }
    }
}

void ACxxScenePCGTool::SceneCreate()
{
    for (int i = 0; i < gridXNum; ++i)
    {
        for (int j = 0; j < gridYNum; ++j)
        {
            if (gridAreaFlags[i][j] != 0)
            {
                // 修正一下位置，避免浮空
                FVector location = GetBlockLoction(i, j) + FVector(0, 0, -10.0);
                UStaticMeshComponent* build = nullptr;

                UMaterial* mat = nullptr;
                int32 tRandValue = FMath::RandRange(1, 10);
                switch (gridTypeFlags[i][j])
                {
                case 0:
                    if (tRandValue > 7)
                    {
                        if(areaZZQBuildAClass) build = NewObject<UStaticMeshComponent>(this, areaZZQBuildAClass);
                    }else
                    {
                        if(areaZZQBuildBBClass) build = NewObject<UStaticMeshComponent>(this, areaZZQBuildBBClass);
                    }
                    break;
                case 1:
                    if (tRandValue > 5)
                    {
                        if(areaZZQBuildAClass) build = NewObject<UStaticMeshComponent>(this, areaZZQBuildAClass);
                    }else
                    {
                        if(areaZZQBuildBBClass) build = NewObject<UStaticMeshComponent>(this, areaZZQBuildBBClass);
                    }
                    break;
                case 2:
                    if (tRandValue > 6)
                    {
                        if(areaWHQBuildAClass) build = NewObject<UStaticMeshComponent>(this, areaWHQBuildAClass);
                    }else if (tRandValue > 2)
                    {
                        if(areaWHQBuildBBClass) build = NewObject<UStaticMeshComponent>(this, areaWHQBuildBBClass);
                    }else
                    {
                        if(areaWHQBuildCCClass) build = NewObject<UStaticMeshComponent>(this, areaWHQBuildCCClass);
                    }
                    break;
                case 3:
                    if (tRandValue > 9)
                    {
                        if(areaXXQBuildAClass) build = NewObject<UStaticMeshComponent>(this, areaXXQBuildAClass);
                    }else if (tRandValue > 4)
                    {
                        if(areaXXQBuildBBClass) build = NewObject<UStaticMeshComponent>(this, areaXXQBuildBBClass);
                    }else
                    {
                        if(areaXXQBuildCCClass) build = NewObject<UStaticMeshComponent>(this, areaXXQBuildCCClass);
                    }
                    break;
                case 4:
                    if (tRandValue > 8)
                    {
                        if(areaSYQBuildBBClass) build = NewObject<UStaticMeshComponent>(this, areaSYQBuildAClass);
                    }else if (tRandValue > 6)
                    {
                        if(areaSYQBuildCCClass) build = NewObject<UStaticMeshComponent>(this, areaSYQBuildBBClass);
                    }else
                    {
                        if(areaSYQBuildAClass) build = NewObject<UStaticMeshComponent>(this, areaSYQBuildCCClass);
                    }
                    break;
                default:
                    break;
                }
                
                if(build)
                {
                    build->RegisterComponent();
                    build->AddToRoot();
                    if (gridTypeFlags[i][j] != 3)
                    {
                        float randomX = FMath::FRandRange(-150.0, 150.0);
                        float randomY = FMath::FRandRange(-150.0, 150.0);
                        build->SetWorldLocation(location + FVector(randomX, randomY, 0.0f));
                        // float randomRotation = FMath::FRandRange(-20.0, 20.0);
                        // build->SetWorldRotation(FRotator(0.0f, randomRotation, 0.0f)); 
                    }
                    else
                    {
                        build->SetWorldLocation(location);
                    }
                    build->SetWorldScale3D(FVector(0.11, 0.11, 0.11));
                    build->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                    if(mat) build->SetMaterial(0, mat);
                    builds[i][j] = build;
                }
            }
        }
    }

    for (auto array:builds)
    {
        UE_LOG(LogTemp, Warning, TEXT("mouseX, mouseY"));
        for (auto build:array)
        {
            UE_LOG(LogTemp, Warning, TEXT("mouseX, mouseY"));
        }
    }
}

FVector ACxxScenePCGTool::GetLocationFromXY(int x, int y)
{
    return FVector(x * gridSize, y * gridSize, 0);
}

FVector2D ACxxScenePCGTool::GetXYFromXYLocation(FVector2D loc)
{
    return FVector2D(FMath::FloorToInt(loc.X / gridSize), FMath::FloorToInt(loc.Y / gridSize));
}

FVector ACxxScenePCGTool::GetBlockLoction(int x, int y)
{
    FHitResult hitRes;
    FCollisionQueryParams cParams;
    FVector loc = GetLocationFromXY(x, y);
    FVector newloc = FVector(loc.X + gridSize / 2.0f, loc.Y + gridSize / 2.0f, 5000);
    if (GetWorld()->LineTraceSingleByChannel(hitRes, newloc,  newloc + FVector(0, 0, -10000),
        ECC_GameTraceChannel3, cParams))
    {
        FVector hitLoc = hitRes.Location;
        if(hitLoc.Z <= horizontalRange.Y && hitLoc.Z >= horizontalRange.X) return FVector(hitLoc);
    }
    
    return FVector(0, 0, -10000);
}

void ACxxScenePCGTool::InitArray()
{
    gridAreaFlags.SetNumZeroed(gridXNum);
    for (int i = 0; i < gridXNum; ++i)
    {
        gridAreaFlags[i].SetNumZeroed(gridYNum);
    }

    gridTypeFlags.SetNumZeroed(gridXNum);
    for(int i = 0; i < gridXNum; ++i)
    {
        gridTypeFlags[i].SetNumZeroed(gridYNum);
    }

    areaID.SetNumZeroed(gridXNum);
    for(int i = 0; i < gridXNum; ++i)
    {
        areaID[i].SetNumZeroed(gridYNum);
    }
    
    for(int i = 0; i < gridXNum; ++i)
    {
        TArray<UStaticMeshComponent*> buildArray;
        for(int j = 0; j < gridYNum; ++j)
        {
            buildArray.Add(nullptr);
        }
       builds.Add(buildArray);
    }

    buildCollisionCount.SetNumZeroed(gridXNum);
    for(int i = 0; i < gridXNum; ++i)
    {
        buildCollisionCount[i].SetNumZeroed(gridYNum);
    }
}

void ACxxScenePCGTool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    Super::Destroyed();
    for (auto array:ACxxScenePCGTool::builds)
    {
        for (auto build:array)
        {
            if (IsValid(build))
            {
                build->RemoveFromRoot();
            }
        }
    }
}

void ACxxScenePCGTool::SetBuildVisible(FVector2D block, bool visible)
{
    int ix = FMath::FloorToInt(block.X);
    int iy = FMath::FloorToInt(block.Y);
    auto* build = builds[ix][iy];
    if(IsValid(builds[ix][iy]))
    {
        builds[ix][iy]->SetVisibility(visible);
        builds[ix][iy]->SetComponentTickEnabled(visible);
    }
}

void ACxxScenePCGTool::UpdateCollisionCount(TArray<FVector2D> addCount, TArray<FVector2D> subCount)
{
    for (auto block : addCount)
    {
        int ix = FMath::FloorToInt(block.X);
        int iy = FMath::FloorToInt(block.Y);
        if (ix >= 0 && ix < gridXNum && iy >= 0 && iy < gridYNum)
        {
            if (buildCollisionCount[ix][iy] == 0)
            {
                SetBuildVisible(FVector2D(ix, iy), false);
            }
            buildCollisionCount[ix][iy]++;
        }
    }

    for (auto block : subCount)
    {
        int ix = FMath::FloorToInt(block.X);
        int iy = FMath::FloorToInt(block.Y);
        if (ix >= 0 && ix < gridXNum && iy >= 0 && iy < gridYNum)
        {
            buildCollisionCount[ix][iy]--;
            if (buildCollisionCount[ix][iy] == 0)
            {
                SetBuildVisible(FVector2D(ix, iy), true);
            }
        }
    }
}
