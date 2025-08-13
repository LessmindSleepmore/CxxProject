// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxPathControlPoint.h"

#include "CxxPlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UCxxPathControlPoint::UCxxPathControlPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// static ConstructorHelpers::FObjectFinder<UBlueprint> movementComponentClassFinder(
	// 		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/MovementMesh.MovementMesh_C'"));
	// if (movementComponentClassFinder.Succeeded())
	// {
	// movementComponentClass = movementComponentClassFinder.Object->GeneratedClass;
	// }
	
	// static ConstructorHelpers::FObjectFinder<UBlueprint> tangentComponentAClassFinder(
	// 	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/TangentMeshA.TangentMeshA_C'"));
	// if (tangentComponentAClassFinder.Succeeded())
	// {
	// 	tangentComponentAClass = tangentComponentAClassFinder.Object->GeneratedClass;
	// }
	//
	// static ConstructorHelpers::FObjectFinder<UBlueprint> tangentComponentBClassFinder(
	// TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/TangentMeshB.TangentMeshB_C'"));
	// if (tangentComponentBClassFinder.Succeeded())
	// {
	// 	tangentComponentBClass = tangentComponentBClassFinder.Object->GeneratedClass;
	// }
}

void UCxxPathControlPoint::CreateControlPointVisibleComponents()
{
	if(movementControlComponent || tangentControlAComponent || tangentControlBComponent) return;
	FString name = this->GetName();
	FName movementFName(name + "_movement");
	movementControlComponent = NewObject<UStaticMeshComponent>(this, movementComponentClass, movementFName);
	movementControlComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	movementControlComponent->RegisterComponent();

	if(movementControlComponent->GetAttachParent())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachParent Location: %s"), *movementControlComponent->GetAttachParent()->GetComponentLocation().ToString());
	}

	FName tangentAFName(name + "_tangentA");
	tangentControlAComponent = NewObject<UStaticMeshComponent>(this, tangentComponentAClass, tangentAFName);
	tangentControlAComponent->AttachToComponent(movementControlComponent, FAttachmentTransformRules::KeepRelativeTransform);
	tangentControlAComponent->SetRelativeLocation(FVector(100, 0, 0));
	tangentControlAComponent->RegisterComponent();

	FName tangentBFName(name + "_tangentB");
	tangentControlBComponent = NewObject<UStaticMeshComponent>(this, tangentComponentBClass, tangentBFName);
	tangentControlBComponent->AttachToComponent(movementControlComponent, FAttachmentTransformRules::KeepRelativeTransform);
	tangentControlBComponent->SetRelativeLocation(FVector(-100, 0, 0));
	tangentControlBComponent->RegisterComponent();

	movementControlComponent->SetRenderCustomDepth(true);
	tangentControlAComponent->SetRenderCustomDepth(true);
	tangentControlBComponent->SetRenderCustomDepth(true);
	UpdateMousePointTick();
}

void UCxxPathControlPoint::DeleteControlPointVisibleComponents()
{
	if(movementControlComponent) movementControlComponent->DestroyComponent();
	if(tangentControlAComponent) tangentControlAComponent->DestroyComponent();
	if(tangentControlBComponent) tangentControlBComponent->DestroyComponent();
	movementControlComponent = nullptr;
	tangentControlAComponent = nullptr;
	tangentControlBComponent = nullptr;
}

void UCxxPathControlPoint::Init(FVector loc, FVector tan, PointType t,
                                UCxxRailLineSegment* pre, UCxxRailLineSegment* next)
{
	location = loc;
	tangent = tan;
	type = t;
	preSegment = pre;
	nextSegment = next;
	station = nullptr;
	if(t != ManualPoint)
	{
		DeleteControlPointVisibleComponents();
	}
}

void UCxxPathControlPoint::InitStationPoint(ACxxStation* s, FVector loc, FVector tan, PointType t,
	UCxxRailLineSegment* pre, UCxxRailLineSegment* next)
{
	Init(loc, tan, t, pre, next);
	station = s;
}

void UCxxPathControlPoint::SetSegment(UCxxRailLineSegment* pre, UCxxRailLineSegment* next)
{
	preSegment = pre;
	nextSegment = next;
}

void UCxxPathControlPoint::UpdateMousePointTick()
{
	// 打印tangent
	UE_LOG(LogTemp, Warning, TEXT("tangent: %s"), *tangent.ToString());
	
	// 更新鼠标切线和位置
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel3, cParams))
	{
		location = hitRes.Location;
		if (location.Z < 150.0f) location.Z = 150.0f;
		if(movementControlComponent){
			movementControlComponent->SetWorldLocation(FVector(location.X, location.Y, (location.Z +
				GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().Z) / 2.0f));
			UpdateSegment();
		}
	}else
	{
		if (preSegment && preSegment->startPoint)
		{
			location = preSegment->startPoint->location;
		}else if (nextSegment && nextSegment->endPoint)
		{
			location = nextSegment->startPoint->location;
		}else
		{
			location = FVector(0, 0, 0);
		}
	}
}

void UCxxPathControlPoint::UpdateMouseRotateTick(bool bRevert)
{
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 100000,
		ECC_GameTraceChannel3, cParams))
	{
		FVector direction = hitRes.Location - location;
		if(beginDirectionLength < -0.1f) beginDirectionLength = direction.Length();
		tangentIntensity += (direction.Length() - beginDirectionLength) * 10.0f;
		beginDirectionLength = direction.Length();
		direction = FVector(direction.X, direction.Y, 0);
		direction.Normalize();
		direction = bRevert?-direction:direction;

		tangent = direction * tangentIntensity;
		UE_LOG(LogTemp, Warning, TEXT("tangent bRevert: %s"), *tangent.ToString());
		if (movementControlComponent)
		{
			FRotator rotation = FRotationMatrix::MakeFromX(direction).Rotator();
			movementControlComponent->SetWorldRotation(rotation);
			UpdateSegment();
		}
	}
}

void UCxxPathControlPoint::ResetTangentOperatorParam()
{
	beginDirectionLength = -1;
}

void UCxxPathControlPoint::Init(UClass* m1, UClass* t1, UClass* t2)
{
	movementComponentClass = m1;
	tangentComponentAClass = t1;
	tangentComponentBClass = t2;
}

PointType UCxxPathControlPoint::GetPointType()
{
	return type;
}

void UCxxPathControlPoint::SetPointType(PointType t)
{
	type = t;
	if(t != StationPoint)
	{
		station = nullptr;
	}
	if(type == ManualPoint)
	{
		CreateControlPointVisibleComponents();
	}
}

void UCxxPathControlPoint::UpdateSegment()
{
	if(preSegment)
	{
		preSegment->SetEndPoint(this);
	}
	if(nextSegment)
	{
		nextSegment->SetStartPoint(this);
	}
}

void UCxxPathControlPoint::SetVisible(bool bPointVisible)
{
	if(type == ManualPoint)
	{
		if(movementControlComponent)
		{
			movementControlComponent->SetVisibility(bPointVisible);
		}
		if(tangentControlAComponent)
		{
			tangentControlAComponent->SetVisibility(bPointVisible);
		}
		if(tangentControlBComponent)
		{
			tangentControlBComponent->SetVisibility(bPointVisible);
		}
	}
}

void UCxxPathControlPoint::Destroy()
{
	if(movementControlComponent)
	{
		movementControlComponent->DestroyComponent();
	}
	if(tangentControlAComponent)
	{
		tangentControlAComponent->DestroyComponent();
	}
	if(tangentControlBComponent)
	{
		tangentControlBComponent->DestroyComponent();
	}
	this->DestroyComponent();
}

void UCxxPathControlPoint::Delete()
{
	if(movementControlComponent)
	{
		movementControlComponent->DestroyComponent();
	}
	if(tangentControlAComponent)
	{
		tangentControlAComponent->DestroyComponent();
	}
	if(tangentControlBComponent)
	{
		tangentControlBComponent->DestroyComponent();
	}
	if (preSegment)
	{
		preSegment->startPoint->nextSegment = nextSegment;
		if (nextSegment)
		{
			nextSegment->SetStartPoint(preSegment->startPoint);
		}
		preSegment->Destroy();
	}
	this->DestroyComponent();
}

// void UCxxPathControlPoint::ManualPointVisible()
// {
// 	
// }

// Called when the game starts
void UCxxPathControlPoint::BeginPlay()
{
	Super::BeginPlay();
	if(type == ManualPoint)
	{
		CreateControlPointVisibleComponents();
	}
	playerUsedCamera = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UCameraComponent>();
}


// Called every frame
void UCxxPathControlPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(type == ManualPoint)
	{
		// 动态调整大小
		if (movementControlComponent && playerUsedCamera)  
		{
			float dist = playerUsedCamera->OrthoWidth / 10000.0f;
			movementControlComponent->SetRelativeScale3D(FVector(1.2, 1.2, 1.2) * dist);
		}
		
		if(!movementControlComponent && !tangentControlAComponent && !tangentControlBComponent)
		{
			CreateControlPointVisibleComponents();
		}
	}
}

