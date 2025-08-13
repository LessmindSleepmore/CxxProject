// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxPathControlPoint.h"
#include "CxxScenePCGTool.h"
#include "Components/SplineComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SplineMeshComponent.h"
#include "CxxRailLineSegment.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIGAME_API UCxxRailLineSegment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCxxRailLineSegment();
	UPROPERTY(EditAnywhere) UClass* railLineBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarBaseClass;
	UPROPERTY(EditAnywhere) UClass* railLinePillarRepeatClass;
	// UPROPERTY() USceneComponent* rootComponent;

	// UPROPERTY() UCxxRailLineSegment* nextSegment = nullptr;
	// UPROPERTY() UCxxRailLineSegment* preSegment = nullptr;
	UPROPERTY(EditAnywhere) UCxxPathControlPoint* startPoint = nullptr;
	UPROPERTY(EditAnywhere) UCxxPathControlPoint* endPoint = nullptr;

	UPROPERTY(EditAnywhere) float pillarDensityLength = 1300.;
	UPROPERTY(EditAnywhere) USplineComponent* spline = nullptr;
	UPROPERTY(EditAnywhere) USplineMeshComponent* splineMesh = nullptr;
	UPROPERTY(EditAnywhere) TArray<UStaticMeshComponent*> pillarMeshBaseArray;
		UPROPERTY() TArray<UStaticMeshComponent*> pillarMeshRepeatArray;

	UPROPERTY(EditAnywhere) ACxxScenePCGTool* scenePCGTool = nullptr;

	void Init(UCxxPathControlPoint* start, UCxxPathControlPoint* end, float length, UClass* b, UClass* pb, UClass* pr);
	
	// void MeshBuildTick();
	void Destroy();

	void SetStartPoint(UCxxPathControlPoint* point);
	void SetEndPoint(UCxxPathControlPoint* point);

	void UpdatePillarMesh();

	// 场景碰撞计算
	TArray<FVector2D> collisionBlockArray;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ComputeCollisionBlock();
	void AddToCollisionBlockArray(TArray<FVector2D> newCollisionBlockArray);
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	void SwitchHightLightEffect(bool bIsHighlighted);
};
