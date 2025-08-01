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
	static ConstructorHelpers::FObjectFinder<UBlueprint> movementComponentClassFinder(
			TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/MovementMesh.MovementMesh'"));
	if (movementComponentClassFinder.Succeeded())
	{
		movementComponentClass = movementComponentClassFinder.Object->GeneratedClass;
	}
	
	static ConstructorHelpers::FObjectFinder<UBlueprint> tangentComponentAClassFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/TangentMeshA.TangentMeshA'"));
	if (tangentComponentAClassFinder.Succeeded())
	{
		tangentComponentAClass = tangentComponentAClassFinder.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> tangentComponentBClassFinder(
	TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/PointTest/TangentMeshB.TangentMeshB'"));
	if (tangentComponentBClassFinder.Succeeded())
	{
		tangentComponentBClass = tangentComponentBClassFinder.Object->GeneratedClass;
	}
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
	// 更新鼠标切线和位置
	ACxxPlayerController* cxxPC = Cast<ACxxPlayerController>(GetWorld()->GetFirstPlayerController());
	float mouseX, mouseY;
	cxxPC->GetMousePosition(mouseX, mouseY);
	
	FVector wLocation, wDirection;
	cxxPC->DeprojectScreenPositionToWorld(mouseX, mouseY, wLocation, wDirection);
	
	FHitResult hitRes;
	FCollisionQueryParams cParams;
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		ECC_GameTraceChannel3, cParams))
	{
		location = hitRes.Location;
		// UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *location.ToString());
		if(movementControlComponent){
			movementControlComponent->SetWorldLocation(location + FVector(0, 0, 75));
			if(movementControlComponent->GetAttachParent())
			{
				// UE_LOG(LogTemp, Warning, TEXT("AttachParent Location: %s"), *movementControlComponent->GetAttachParent()->GetComponentLocation().ToString());
			}
			// UE_LOG(LogTemp, Warning, TEXT("World Location: %s"), *movementControlComponent->GetComponentLocation().ToString());
			UpdateSegment();
		}
	}else
	{
		location = FVector::ZeroVector;
	}

	// 画一个debug sphere
	// DrawDebugSphere(GetWorld(), location, 20.0f, 8, FColor::Red, false, 0.1f);
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
	
	if (GetWorld()->LineTraceSingleByChannel(hitRes, wLocation, wLocation + wDirection * 10000,
		ECC_GameTraceChannel3, cParams))
	{
		FVector direction = hitRes.Location - location;
		if(beginDirectionLength < -0.1f) beginDirectionLength = direction.Length();
		tangentIntensity += (direction.Length() - beginDirectionLength) * 10.0f;
		beginDirectionLength = direction.Length();
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
	// ...
}


// Called every frame
void UCxxPathControlPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(type == ManualPoint)
	{
		// if(movementControlComponent)
		// {
		// 	// 输出世界坐标和时间
		// 	UE_LOG(LogTemp, Warning, TEXT("World Location: %s, Time: %f"), *movementControlComponent->GetComponentLocation().ToString(), DeltaTime);
		// }
		if(!movementControlComponent && !tangentControlAComponent && !tangentControlBComponent)
		{
			CreateControlPointVisibleComponents();
		}
	}else
	{
		DeleteControlPointVisibleComponents();
		// if(type == PointType::ManualPoint)
		// {
		// 	UpdateMousePointTick();
		// }
	}
	// ManualPointVisible();
	// ...
}

