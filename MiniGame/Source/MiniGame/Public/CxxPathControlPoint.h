// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxStation.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "CxxPathControlPoint.generated.h"

class UCxxRailLineSegment;
// 端点 固定点 手动点 鼠标点

UENUM()
enum PointType
{
	StationPoint,
	FixedPoint,
	ManualPoint,
	// MousePoint
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIGAME_API UCxxPathControlPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCxxPathControlPoint();

	UPROPERTY() UCxxRailLineSegment* nextSegment = nullptr;
	UPROPERTY() UCxxRailLineSegment* preSegment = nullptr;
	UPROPERTY() FVector location;
	UPROPERTY() FVector tangent;
	UPROPERTY() float tangentIntensity = 300.0f; 
	UPROPERTY() float beginDirectionLength = -1.0f;
	UPROPERTY() UCameraComponent* playerUsedCamera;
	// ManualPoint使用的圆球
	// UPROPERTY() UStaticMeshComponent* operatorComponent;
	PointType type;

	// Manual控制组件
	UPROPERTY() UClass* movementComponentClass = nullptr;
	UPROPERTY() UClass* tangentComponentAClass = nullptr;
	UPROPERTY() UClass* tangentComponentBClass = nullptr;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* movementControlComponent = nullptr;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* tangentControlAComponent = nullptr;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* tangentControlBComponent = nullptr;
	void CreateControlPointVisibleComponents();
	void DeleteControlPointVisibleComponents();
	void UpdateMousePointTick();
	void UpdateMouseRotateTick(bool bRevert);
	void ResetTangentOperatorParam();
	void Init(UClass* m1, UClass* t1, UClass* t2);

	UPROPERTY() ACxxStation* station = nullptr;
	void Init(FVector loc, FVector tan, PointType t, UCxxRailLineSegment* pre, UCxxRailLineSegment* next);
	void InitStationPoint(ACxxStation* s, FVector loc,
		FVector tan, PointType t, UCxxRailLineSegment* pre, UCxxRailLineSegment* next);
	void SetSegment(UCxxRailLineSegment* pre, UCxxRailLineSegment* next);
	// void SetType(PointType t, ACxxStation* s = nullptr);
	PointType GetPointType();
	void SetPointType(PointType t); 
	// void ManualPointVisible();

	void UpdateSegment();
	void SetVisible(bool bPointVisible);
	// Destroy:直接删除 Delete:删除操作点后连接剩下的部分
	void Destroy();
	void Delete();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
