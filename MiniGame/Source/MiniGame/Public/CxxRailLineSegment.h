// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CxxPathControlPoint.h"
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
	// UPROPERTY() USceneComponent* rootComponent;

	// UPROPERTY() UCxxRailLineSegment* nextSegment = nullptr;
	// UPROPERTY() UCxxRailLineSegment* preSegment = nullptr;
	UPROPERTY() UCxxPathControlPoint* startPoint = nullptr;
	UPROPERTY() UCxxPathControlPoint* endPoint = nullptr;
	UPROPERTY() USplineMeshComponent* splineMesh = nullptr;

	void Init(UCxxPathControlPoint* start, UCxxPathControlPoint* end);
	
	void MeshBuildTick();
	void Destroy();

	void SetStartPoint(UCxxPathControlPoint* point);
	void SetEndPoint(UCxxPathControlPoint* point);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
