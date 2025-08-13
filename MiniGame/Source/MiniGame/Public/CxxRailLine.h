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

UCLASS()
class MINIGAME_API ACxxRailLine : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) FString railLineName;
	UPROPERTY(EditAnywhere) TArray<ACxxStation*> stations;
	UPROPERTY(EditAnywhere) TArray<ACxxMonoRailTrain*> trains;

	UPROPERTY(EditAnywhere) TArray<UCxxRailLineSegment*> railSegments;
	UPROPERTY(EditAnywhere) UCxxPathControlPoint* curSelectedPoint = nullptr;
	UPROPERTY(EditAnywhere) float railOutLength = 1000.;
	
	UPROPERTY(EditAnywhere) float tangentIntensity = 900;

	UPROPERTY(EditAnywhere) UStaticMesh* railLineBaseMesh;
	UPROPERTY(EditAnywhere) UStaticMesh* railLinePillarMesh;
	UPROPERTY(EditAnywhere) float pillarIntervalDist = 1300.;

	UPROPERTY() USceneComponent* root = nullptr;

	// 环线
	UPROPERTY(EditAnywhere) ACxxStation* ringLineStation = nullptr;
	UPROPERTY(EditAnywhere) UCxxPathControlPoint* ringLinePoint = nullptr;
	UPROPERTY(EditAnywhere) UCxxRailLineSegment* ringLineSegment = nullptr;

	// point
	UPROPERTY(EditAnywhere) UClass* movementComponentClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* tangentComponentAClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* tangentComponentBClass = nullptr;
	UPROPERTY(EditAnywhere) UClass* railLineBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarRepeatClass;
	void Init(UClass* m1, UClass* t1, UClass* t2, UClass* b, UClass* pb, UClass* pr);

	// 编辑模式
	UPROPERTY(EditAnywhere) bool bIsEdit = false;
	bool bIsHighlighted = false;
	
	// Sets default values for this actor's properties
	ACxxRailLine();

	void Init(FString name);

	void FinishedEditRailLineFromStation();
	
	TArray<UCxxRailLineSegment*> IsCurSelectedPointNeighbor(ACxxStation* station);
	UCxxPathControlPoint* GetPointFromStation(ACxxStation* station);
	UCxxRailLineSegment* GetSegmentFromMesh(USplineMeshComponent* mesh);

	// 线路编辑
	UCxxPathControlPoint* AddStation(ACxxStation* station, bool bIsArrowDir);
	UCxxPathControlPoint* AddRingLineStation(ACxxStation* station, UCxxPathControlPoint* point1, UCxxPathControlPoint* point2);
	UCxxPathControlPoint* DeleteRingLineStation(ACxxStation* station);
	UCxxPathControlPoint* DeleteStation(ACxxStation* station);

	// recode
	UPROPERTY(VisibleAnywhere) TArray<UCxxPathControlPoint*> controlPoints;

	// 生成路线图
	TArray<ACxxStation*> GetNeighborStations(ACxxStation* station);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SwitchHightLightEffect(bool bVisiblePoint);

	UPROPERTY(EditAnywhere) bool bIsRingLine = false;
};