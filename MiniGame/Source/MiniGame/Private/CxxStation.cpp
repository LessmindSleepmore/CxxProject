// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxStation.h"

// Sets default values
ACxxStation::ACxxStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACxxStation::BeginPlay()
{
	Super::BeginPlay();

	isHighlighted = false;
}

// Called every frame
void ACxxStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShowPassengerStatus();
}

void ACxxStation::AddPassenger(ACxxPassenger* passenger)
{
	passengers.Add(passenger);
}

void ACxxStation::RemovePassenger(ACxxPassenger* passenger)
{
	passengers.Remove(passenger);
}

void ACxxStation::SwitchOutlineEffect()
{
	UStaticMeshComponent* comp = Cast<UStaticMeshComponent>(this->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (comp)
	{
		isHighlighted = !isHighlighted;
		comp->SetRenderCustomDepth(isHighlighted);
	}
	if(this->GetName() == "BP_CxxResidentialArea_C_3")
	{
		UE_LOG(LogTemp, Warning, TEXT("Switch Name: %s"), *this->GetName());
	}
}

EStationType ACxxStation::GetStationType()
{
	return stationType;
}

void ACxxStation::ShowPassengerStatus()
{
	// 根据乘客的targetStationType间隔一定距离绘制Deug圆形
	for (int i = 0; i < passengers.Num(); i++)
	{
		FVector location = this->GetActorLocation();
		location.Z += 200;
		location.X += 70 * i;
		switch (passengers[i]->targetStationType)
		{
		case EStationType::BusinessDistrict:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Yellow, false, 0.0f);
			break;
		case EStationType::ResidentialArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Blue, false, 0.0f);
			break;
		case EStationType::CulturalArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Red, false, 0.0f);
			break;
		case EStationType::RecreationalArea:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Green, false, 0.0f);
			break;
		default:
			DrawDebugSphere(GetWorld(), location, 20, 4, FColor::Black, false, 0.0f);
			break;
		}
	}
}

