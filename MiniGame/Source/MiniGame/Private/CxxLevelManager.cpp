// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxLevelManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACxxLevelManager::ACxxLevelManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<UBlueprint> businessActorFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Station/BP_CxxBusinessDistrict.BP_CxxBusinessDistrict'"));
	if (businessActorFinder.Succeeded())
	{
		business = businessActorFinder.Object->GeneratedClass; 
	}

	ConstructorHelpers::FObjectFinder<UBlueprint> culturalActorFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Station/BP_CxxCulturalArea.BP_CxxCulturalArea'"));
	if (culturalActorFinder.Succeeded())
	{
		cultural = culturalActorFinder.Object->GeneratedClass;
	}

	ConstructorHelpers::FObjectFinder<UBlueprint> recreationalActorFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Station/BP_CxxRecreationalArea.BP_CxxRecreationalArea'"));
	if (recreationalActorFinder.Succeeded())
	{
		recreational = recreationalActorFinder.Object->GeneratedClass;
	}

	ConstructorHelpers::FObjectFinder<UBlueprint> residentialActorFinder(
		TEXT("/Script/Engine.Blueprint'/Game/Assets/Actor/Station/BP_CxxResidentialArea.BP_CxxResidentialArea'"));
	if (residentialActorFinder.Succeeded())
	{
		residential = residentialActorFinder.Object->GeneratedClass;
	}
}

void ACxxLevelManager::OnNextDayButtonClicked()
{
	if(dispatchManager) dispatchManager->EndEditRailLine();
	SwitchGameStage();
}

void ACxxLevelManager::HiddenUI()
{
	if(dispatchManager) dispatchManager->HiddenUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Hidden);
 }

void ACxxLevelManager::ShowEditStageUI()
{
	HiddenUI();
	if(dispatchManager) dispatchManager->ShowEditStageUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Visible);
}

void ACxxLevelManager::ShowGameStageUI()
{
	HiddenUI();
	if(dispatchManager) dispatchManager->ShowGameStageUI();
	if(nextDayButton) nextDayButton->SetVisibility(ESlateVisibility::Hidden);
}

void ACxxLevelManager::GetAndSetUIComponent()
{
	// 获取场景中的dispatchManager和UI组件
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACxxDispatchManager::StaticClass(), foundActors);
	if (foundActors.Num() > 0)
	{
		dispatchManager = Cast<ACxxDispatchManager>(foundActors[0]);
		if (dispatchManager)
		{
			stageTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("StageText")));
			timeTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("DayTime")));
			dayTextBlock = Cast<UTextBlock>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("DayText")));
			nextDayButton = Cast<UButton>(dispatchManager->hudWidget->GetWidgetFromName(TEXT("NextDayButton")));
			
			// 显示编辑UI
			ShowEditStageUI();
		}
	}
	if(nextDayButton)
	{
		nextDayButton->OnClicked.AddDynamic(this, &ACxxLevelManager::OnNextDayButtonClicked);
	}
}

void ACxxLevelManager::TestFunc_SetStations()
{
	stationNameArray.Add(TEXT("residential"));
	stationNameArray.Add(TEXT("cultural"));
	stationNameArray.Add(TEXT("business"));
	stationNameArray.Add(TEXT("residential"));
	stationNameArray.Add(TEXT("recreational"));

	stationsLocationArray.Add(FVector(1460.0, -1140.0, 0));
	stationsLocationArray.Add(FVector(1500.0, 110.0, 0));
	stationsLocationArray.Add(FVector(1070.0, -650.0, 0));
	stationsLocationArray.Add(FVector(810.0, 380.0, 0));
	stationsLocationArray.Add(FVector(700.0, -180.0, 0));
}

// Called when the game starts or when spawned
void ACxxLevelManager::BeginPlay()
{
	Super::BeginPlay();
	TestFunc_SetStations();
	InitBeginStation();
}

// Called every frame
void ACxxLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!dispatchManager)
	{
		GetAndSetUIComponent();
		return;
	}
	if(bIsGameStage){
		FString timeStr = TimeControl(DeltaTime);
		UpdataTimeTextBlock(timeStr);
		CreateCxxPassenger(DeltaTime);
	}
}

// 当天结束和开始运营都会切换状态
void ACxxLevelManager::SwitchGameStage()
{
	bIsGameStage = !bIsGameStage;
	if(!bIsGameStage)
	{
		gameDay++;
		gameDayTime = 0.0f;
		CreateStaion(stationNameArray[gameDay], stationsLocationArray[gameDay]);
		stageTextBlock->SetText(FText::FromString(TEXT("编辑阶段")));
		dayTextBlock->SetText(FText::FromString(TEXT("第" + FString::FromInt(gameDay) + TEXT("天"))));
		ShowEditStageUI();
	}else
	{
		stageTextBlock->SetText(FText::FromString(TEXT("运营阶段")));
		ShowGameStageUI();
	}
}

void ACxxLevelManager::InitBeginStation()
{
	//TODO:这里先简单生成前两个
	CreateStaion(stationNameArray[0], stationsLocationArray[0]);
	CreateStaion(stationNameArray[1], stationsLocationArray[1]);
}

void ACxxLevelManager::CreateStaion(FString stationName, FVector location)
{
	FActorSpawnParameters spawnParams;
	ACxxStation* station = nullptr;
	
	FRotator randomRotation = FRotator(
	0.f,
	FMath::RandRange(0.f, 360.f),
	0.f
	);

	if (stationName == TEXT("residential"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(residential, location, randomRotation, spawnParams));
		stationsArray.Add(station);
	}
	else if (stationName == TEXT("cultural"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(cultural, location, randomRotation, spawnParams));
		stationsArray.Add(station);
	}
	else if (stationName == TEXT("business"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(business, location, randomRotation, spawnParams));
		stationsArray.Add(station);
	}
	else if (stationName == TEXT("recreational"))
	{
		station = Cast<ACxxStation>(GetWorld()->SpawnActor<AActor>(recreational, location, randomRotation, spawnParams));
		stationsArray.Add(station);
	}
}

FString ACxxLevelManager::TimeControl(float DeltaTime) 
{
	gameDayTime += DeltaTime;
	if (gameDayTime > oneDayInRealTime)
	{
		SwitchGameStage();
	}

	float realTime = 6.0f + (gameDayTime * (24.0f - 6.0f)) / oneDayInRealTime;
	int hour = FMath::FloorToInt(realTime);
	int minute = FMath::FloorToInt((realTime - hour) * 60);
	return FString::Printf(TEXT("%02d:%02d"), hour, minute);
}

void ACxxLevelManager::UpdataTimeTextBlock(FString time)
{
	timeTextBlock->SetText(FText::FromString(time));
}

void ACxxLevelManager::CreateCxxPassenger(float DeltaTime)
{
	// 每隔一段1/60s随机生成一次乘客
	passengerTimer += DeltaTime;
	if(passengerTimer < 1/60.f) return;
	passengerTimer = 0.f;
	
	// 每个车站随机生成乘客
	for(ACxxStation* station : stationsArray)
	{
		int randNum = FMath::RandRange(0, 999);
		if(randNum > 996)
		{
			// 随机生成一个不是当前站EStationType的EStationType
			EStationType randType = static_cast<EStationType>(FMath::RandRange(0, 3));
			while(randType == station->GetStationType())
			{
				randType = static_cast<EStationType>(FMath::RandRange(0, 3));
			}
			// 创建一个CxxPassenger对象
			ACxxPassenger* passenger = GetWorld()->SpawnActor<ACxxPassenger>(ACxxPassenger::StaticClass());
			if(passenger)
			{
				passenger->AttachToActor(station, FAttachmentTransformRules::KeepRelativeTransform);
				passenger->targetStationType = randType;
				station->AddPassenger(passenger);
			}
		}
	}
}