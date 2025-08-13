// Fill out your copyright notice in the Description page of Project Settings.

#include "CxxPassenger.h"
#include "CxxStation.h"

// Sets default values
ACxxPassenger::ACxxPassenger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACxxPassenger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACxxPassenger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACxxPassenger::IsBoardingRequired(ACxxStation* nextStation)
{
	if (path.Num() <= pathIdx + 1) return false;
	if (nextStation == path[pathIdx + 1])
	{
		FSlateBrush brush; 
		brush.SetResourceObject(textureTA);
		brush.ImageSize = FVector2D(12, 24);
		passagerImage->SetBrush(brush);
		return true;
	}
	return false;
}

bool ACxxPassenger::IsAlightingRequired(ACxxStation* nextStation)
{
	//最后一站了直接下车
	if (path.Num() == pathIdx + 1)
	{
		FSlateBrush brush; 
		brush.SetResourceObject(textureA);
		brush.ImageSize = FVector2D(12, 24);
		passagerImage->SetBrush(brush);
		return true;
	}
	// 导航错误，先下车
	if (path.Num() < pathIdx + 1) return true;
	if (nextStation != path[pathIdx + 1])
	{
		FSlateBrush brush; 
		brush.SetResourceObject(textureA);
		brush.ImageSize = FVector2D(12, 24);
		passagerImage->SetBrush(brush);
		return true;
	}
	// pathIdx++;
	return false;
}

void ACxxPassenger::DestroyPassenger()
{
	passagerImage->RemoveFromParent();
	passagerImage->SetVisibility(ESlateVisibility::Collapsed);
	passagerImage = nullptr;
	Destroy();
}

