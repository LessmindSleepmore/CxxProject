// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxRailLineSegment.h"

// Sets default values for this component's properties
UCxxRailLineSegment::UCxxRailLineSegment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UBlueprint> railLineBaseClassFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Rail/RailLineBase.RailLineBase'"));
	if (railLineBaseClassFinder.Succeeded())
	{
		railLineBaseClass = railLineBaseClassFinder.Object->GeneratedClass;
	}
	// ...
}

void UCxxRailLineSegment::Init(UCxxPathControlPoint* start,
	UCxxPathControlPoint* end)
{
	startPoint = start;
	endPoint = end;

	if(!splineMesh)
	{
		splineMesh = NewObject<USplineMeshComponent>(this, railLineBaseClass);
		splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		splineMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
		// splineMesh->AttachToComponent(rootComponent, FAttachmentTransformRules::KeepWorldTransform);
		splineMesh->RegisterComponent();
		splineMesh->SetRenderCustomDepth(true);
	}

	splineMesh->SetStartPosition(startPoint->location + FVector(0, 0, 50));
	splineMesh->SetStartTangent(startPoint->tangent);
	splineMesh->SetEndPosition(endPoint->location + FVector(0, 0, 50));
	splineMesh->SetEndTangent(endPoint->tangent);
}

// void UCxxRailLineSegment::MeshBuildTick()
// {
// 	if(startPoint->type == PointType::ManualPoint)
// 	{
// 		splineMesh->SetStartPosition(startPoint->location + FVector(0, 0, 50));
// 		splineMesh->SetStartTangent(startPoint->tangent);
// 	}
// 	else if(endPoint->type == PointType::ManualPoint)
// 	{
// 		splineMesh->SetEndPosition(endPoint->location + FVector(0, 0, 50));
// 		splineMesh->SetEndTangent(endPoint->tangent);
// 	}
// }

void UCxxRailLineSegment::Destroy()
{
	if(splineMesh)
	{
		splineMesh->DestroyComponent();
		splineMesh = nullptr;
	}
	this->DestroyComponent();
}

void UCxxRailLineSegment::SetStartPoint(UCxxPathControlPoint* point)
{
	startPoint = point;
	if(splineMesh)
	{
		splineMesh->SetStartPosition(startPoint->location + FVector(0, 0, 50));
		splineMesh->SetStartTangent(startPoint->tangent);
	}
}

void UCxxRailLineSegment::SetEndPoint(UCxxPathControlPoint* point)
{
	endPoint = point;
	if(splineMesh)
	{
		splineMesh->SetEndPosition(endPoint->location + FVector(0, 0, 50));
		splineMesh->SetEndTangent(endPoint->tangent);
		
		// print splineMesh->GetEndTangent();
		// UE_LOG(LogTemp, Warning, TEXT("SetEndPoint: %s"), splineMesh->GetEndTangent().ToString());
	}
}

// Called when the game starts
void UCxxRailLineSegment::BeginPlay()
{
	Super::BeginPlay();
	// if(!rootComponent)
	// {
	// 	rootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// }
	// ...
	
}


// Called every frame
void UCxxRailLineSegment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// MeshBuildTick();
	// ...
}

