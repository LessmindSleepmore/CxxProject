// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxPathControlPoint.h"
#include "CxxRailLineSegment.h"
#include "CxxStation.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "CxxRailLine.generated.h"

#ifndef RailLineChannel
#define RailLineChannel ECC_GameTraceChannel2
#endif

class ACxxMonoRailTrain;

static int64 componentsCount = 0;
static int GetComponentsCount() { return componentsCount++; }

// UCLASS()
// class UCxxRailSegment : public UObject
// {
// public:
// 	GENERATED_BODY()
// 	UPROPERTY() TArray<FVector> points;
// 	UPROPERTY() TArray<FVector> tangents;
// 	UPROPERTY() TArray<int> pointType; // 0:起点段和终点段（不可编辑） 1:手动插入的点（可编辑） 3:自动生成的点(不可编辑)
// 	// UPROPERTY() bool bEndIsStation;
// 	UPROPERTY() ACxxStation* stationA;
// 	UPROPERTY() ACxxStation* stationB;
// 	// UPROPERTY() FVector stationBUsedDir = FVector::ZeroVector;
// 	// UPROPERTY() FVector stationAUsedDir = FVector::ZeroVector;
// 	
//
// 	void init(ACxxStation* station)
// 	{
// 		points.Empty();
// 		stationA = station;
// 		stationB = nullptr;
// 		// bEndIsStation = false;
// 	}
//
// 	void init(ACxxStation* tStationA, ACxxStation* tStationB)
// 	{
// 		points.Empty();
// 		stationA = tStationA;
// 		stationB = tStationB;
// 		// bEndIsStation = false;
// 	}
// };


UCLASS()
class MINIGAME_API ACxxRailLine : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) FString railLineName;
	UPROPERTY(EditAnywhere) TArray<ACxxStation*> stations;
	UPROPERTY(EditAnywhere) TArray<ACxxMonoRailTrain*> trains;
	// UPROPERTY(EditAnywhere) TArray<bool> bStationArrowDircArray;
	// UPROPERTY(EditAnywhere) FVector mousePos;
	UPROPERTY(EditAnywhere) TArray<UCxxRailLineSegment*> railSegments;
	UPROPERTY(EditAnywhere) UCxxPathControlPoint* curSelectedPoint = nullptr;
	UPROPERTY(EditAnywhere) float railOutLength = 200.;
	// UPROPERTY(EditAnywhere) FColor railLineColor = FColor::Green;
	// UPROPERTY(EditAnywhere) int insertRailPointNum = 20; // TODO:优化为动态计算插值数量
	UPROPERTY(EditAnywhere) float tangentIntensity = 300;
	// UPROPERTY(EditAnywhere) bool isEdit = false;
	UPROPERTY(EditAnywhere) UStaticMesh* railLineBaseMesh;
	UPROPERTY(EditAnywhere) UStaticMesh* railLinePillarMesh;
	UPROPERTY(EditAnywhere) float pillarIntervalDist = 180.;

	UPROPERTY() USceneComponent* root = nullptr;
	// 轨道铁轨 轨道支撑柱
	UPROPERTY(EditAnywhere) UClass* railLineBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarClass;

	// 编辑模式
	UPROPERTY(EditAnywhere) bool bIsEdit = false;
	// UPROPERTY(EditAnywhere) UCxxRailSegment* curEditSegment = nullptr;
	// UPROPERTY(EditAnywhere) FVector mousePosition;
	// UPROPERTY(EditAnywhere) UCxxRailSegment* curEditSegment = nullptr;
	// mesh名称获取Seg对象
	// UPROPERTY() TMap<UStaticMeshComponent*, UCxxRailSegment*> meshCompName2Seg;

	// 轨道段模型
	// UPROPERTY(EditAnywhere) TArray<UStaticMeshComponent*> railLineMeshComponents;
	bool bIsHighlighted = false;
	
	// UPROPERTY(EditAnywhere) UClass* trainClass;
	
	// Sets default values for this actor's properties
	ACxxRailLine();

	void Init(FString name);
	// void ProceeEndWithMouse(UCxxRailSegment* seg, UCxxRailSegment* preSeg);
	// void ProceeEndWithStation(UCxxRailSegment* seg, int segIdx);
	// void GenerateInterPoint(UCxxRailSegment* seg, int startPointIdx, FVector startPointTangent,
		// int endPointIdx, FVector endPointTangent);
	// void UpdataSegment(UCxxRailSegment* seg);
	// void FinishEdit();
	void FinishedEditRailLineFromStation();
	// void VisiablePointAndTangent();
	// void DestroyLine();
	// bool IsCurSelectedPointValid();
	TArray<UCxxRailLineSegment*> IsCurSelectedPointNeighbor(ACxxStation* station);
	UCxxPathControlPoint* GetPointFromStation(ACxxStation* station);
	UCxxRailLineSegment* GetSegmentFromMesh(USplineMeshComponent* mesh);

	// 线路编辑
	UCxxPathControlPoint* AddStation(ACxxStation* station, bool bIsArrowDir);
	UCxxPathControlPoint* DeleteStation(ACxxStation* station);
	// int DeleteStation(ACxxStation* station);
	
	int debugCount = 0;

	// recode
	UPROPERTY(VisibleAnywhere) TArray<UCxxPathControlPoint*> controlPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// void DrawVisableLine();
	void UpdateRailLine();
	// void UpdateRailLineSegmentWithMouseLocation(UCxxRailSegment* seg, FVector mouseLocation);
	void SetRailColor(FColor color);
	void SwitchHightLightEffect();
};