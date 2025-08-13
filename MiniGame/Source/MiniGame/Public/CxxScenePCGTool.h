// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Algo/RandomShuffle.h"
#include "CxxScenePCGTool.generated.h"

UCLASS()
class MINIGAME_API ACxxScenePCGTool : public AActor
{
	GENERATED_BODY()
	// 1:区域1 2:区域2
	TArray<TArray<int>> gridAreaFlags;
	// 1:住宅区 2:文化区 3:休闲区 4:商业区
	TArray<TArray<int>> gridTypeFlags;
	TArray<TArray<int>> areaID;

	// 建筑模型
	UPROPERTY(EditAnywhere) UClass* defaultBuildAClass;
	UPROPERTY(EditAnywhere) UClass* defaultBuildBClass;
	
	UPROPERTY(EditAnywhere) UClass* areaSYQBuildAClass;
	UPROPERTY(EditAnywhere) UClass* areaSYQBuildBBClass;
	UPROPERTY(EditAnywhere) UClass* areaSYQBuildCCClass;
	UPROPERTY(EditAnywhere) UClass* areaWHQBuildAClass;
	UPROPERTY(EditAnywhere) UClass* areaWHQBuildBBClass;
	UPROPERTY(EditAnywhere) UClass* areaWHQBuildCCClass;
	UPROPERTY(EditAnywhere) UClass* areaXXQBuildAClass;
	UPROPERTY(EditAnywhere) UClass* areaXXQBuildBBClass;
	UPROPERTY(EditAnywhere) UClass* areaXXQBuildCCClass;
	UPROPERTY(EditAnywhere) UClass* areaZZQBuildAClass;
	UPROPERTY(EditAnywhere) UClass* areaZZQBuildBBClass;

	// 材质
	UPROPERTY() UMaterial* buildMaterialA;
	UPROPERTY() UMaterial* buildMaterialB;
	UPROPERTY() UMaterial* buildMaterialC;
	UPROPERTY() UMaterial* buildMaterialD;
	UPROPERTY() UMaterial* buildMaterialE;

	UPROPERTY() TArray<FVector2D> directions = {
		FVector2D(1, 0),
		FVector2D(-1, 0),
		FVector2D(0, 1),
		FVector2D(0, -1)
	};
	
	
public:	
	// Sets default values for this actor's properties
	ACxxScenePCGTool();

	UPROPERTY(EditAnywhere) int gridXNum = 36;  // 地形网格数量
	UPROPERTY(EditAnywhere) int gridYNum = 36;
	UPROPERTY(EditAnywhere) int gridSize = 200;  // 地形大小
	UPROPERTY(EditAnywhere) FVector2D horizontalRange;  // 水平面范围
	UPROPERTY(EditAnywhere) TArray<FVector2D> areaGeneratePoint;  // 生成初始点
	UPROPERTY(EditAnywhere) TArray<FVector2D> areaGenerateBlockNumRange;
	UPROPERTY(EditAnywhere) TArray<int> typeAreaNum = {4, 3, 3, 3};
	UPROPERTY() TArray<bool> areaVaild;
	TArray<TArray<FVector2D>> areaBlocksArray;
	
	// 生成站点用
	TArray<TArray<UStaticMeshComponent*>> builds;
	TArray<TArray<int>> buildCollisionCount; //计数碰撞到的轨道数量
	UPROPERTY() TArray<FVector2D> stationLoc;
	UPROPERTY() TArray<int> stationType;
	bool bBeginPlay = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SceneGenerate();
	void SceneTypeGenerate();
	void SceneCreate();
	FVector GetLocationFromXY(int x, int y);
	FVector2D GetXYFromXYLocation(FVector2D loc);
	FVector GetBlockLoction(int x, int y);
	void InitArray();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void SetBuildVisible(FVector2D block, bool visible);

	// seg的碰撞调用函数
	void UpdateCollisionCount(TArray<FVector2D> addCount, TArray<FVector2D> subCount);
};
