// Fill out your copyright notice in the Description page of Project Settings.


#include "CxxPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"

void ACxxPlayerController::BeginPlay()
{
	Super::BeginPlay();
	this->SetMouseVisable();
	dispatchManager = Cast<ACxxDispatchManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACxxDispatchManager::StaticClass()));
	if(dispatchManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Find DispatchManager"));
		bGetDispatchManager = true;
	}
	palyerCamera = GetPawn()->FindComponentByClass<UCameraComponent>();
}

void ACxxPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DeltaSeconds = DeltaSeconds / GetWorld()->GetWorldSettings()->TimeDilation;
	CameraMovementControl(DeltaSeconds);
	if(zoomScale > 0) zoomScale -= DeltaSeconds * 2.f;
	if(zoomScale < 0) zoomScale += DeltaSeconds * 2.f;
	if(zoomScale!= 0 && zoomScale > -0.05 && zoomScale < 0.05) zoomScale = 0;
	CameraForwardControl(DeltaSeconds);
}

void ACxxPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	
	InputComponent->BindAxis("MouseScroll", this, &ACxxPlayerController::SetZoomScale);
}

void ACxxPlayerController::SetMouseVisable()
{
	FInputModeGameAndUI inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	inputMode.SetHideCursorDuringCapture(false);
	this->SetInputMode(inputMode);
	this->bShowMouseCursor = true;
}

void ACxxPlayerController::CameraMovementControl(float DeltaTime)
{
	int32 screenWidth, screenHeight;
	GetViewportSize(screenWidth, screenHeight);
	float mouseX, mouseY;
	GetMousePosition(mouseX, mouseY);
    if(mouseX < 1 || mouseY < 1 || mouseX > screenWidth || mouseY > screenHeight) return;

	// UE_LOG(LogTemp, Warning, TEXT("mouseX: %f, mouseY: %f"), mouseX, mouseY);
	
	float edgeThreshold = 100.0f;
	float speed = 0;

	APawn* playerPawn = GetPawn();
	if (!playerPawn || !palyerCamera) return;
	if(palyerCamera->ProjectionMode == ECameraProjectionMode::Type::Perspective)
	{
		speed = 2000.0f * GetPawn()->GetActorLocation().Z / 1600.f;
	}else
	{
		speed = 4500.0f * GetPawn()->GetActorLocation().Z / 1600.f;
	}
	
	FVector palyerCameraLocation = GetPawn()->GetActorLocation();
    
	if (mouseX < edgeThreshold)
	{
		palyerCameraLocation.X += speed * DeltaTime * (edgeThreshold - mouseX) / edgeThreshold;
		palyerCameraLocation.X = FMath::Clamp(palyerCameraLocation.X, -5000.f, 25000.f);
	}
	else if (mouseX > screenWidth - edgeThreshold)
	{
		palyerCameraLocation.X -= speed * DeltaTime * (edgeThreshold - (screenWidth - mouseX)) / edgeThreshold;
		palyerCameraLocation.X = FMath::Clamp(palyerCameraLocation.X, -5000.f, 25000.f);
	}
	if (mouseY < edgeThreshold)
	{
		palyerCameraLocation.Y += speed * DeltaTime * (edgeThreshold - mouseY) / edgeThreshold;
		palyerCameraLocation.Y = FMath::Clamp(palyerCameraLocation.Y, -5000.f, 25000.f);
	}
	else if (mouseY > screenHeight - edgeThreshold)
	{
		palyerCameraLocation.Y -= speed * DeltaTime * (edgeThreshold - (screenHeight - mouseY)) / edgeThreshold;
		palyerCameraLocation.Y = FMath::Clamp(palyerCameraLocation.Y, -5000.f, 25000.f);
	}

	GetPawn()->SetActorLocation(palyerCameraLocation);
}

void ACxxPlayerController::CameraForwardControl(float DeltaTime)
{
	APawn* playerPawn = GetPawn();
	if (!playerPawn || !palyerCamera) return;
	if(palyerCamera->ProjectionMode == ECameraProjectionMode::Type::Perspective){
		APlayerCameraManager* palyerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		if (!palyerCameraManager) return;

		FVector forward = palyerCameraManager->GetActorForwardVector(); 
		// 1650 15000
		float speed = 5000.0f;
		FVector movement = forward * zoomScale * speed * DeltaTime * playerPawn->GetActorLocation().Z / 2000.f;
		FVector currentLoc = playerPawn->GetActorLocation();
		FVector predictedLoc = currentLoc + movement;          

		float factor = 1.0f;
		if (predictedLoc.Z < 1650.0f)
		{
			float allowedZ = 1650.0f - currentLoc.Z; 
			float plannedZ = movement.Z;
			factor = allowedZ / plannedZ; 
		}
		else if (predictedLoc.Z > 15000.0f)
		{
			float allowedZ = 15000.0f - currentLoc.Z;
			float plannedZ = movement.Z;
			factor = allowedZ / plannedZ;
		}
		movement *= factor;
		playerPawn->AddActorWorldOffset(movement, true);
	}else
	{
		float speed = 7000.0f;
		// 限制缩放
		palyerCamera->OrthoWidth = FMath::Clamp(
			palyerCamera->OrthoWidth - zoomScale * speed * DeltaTime * playerPawn->GetActorLocation().Z / 2000.f,
			6500.0f,
			50000.0f);
		
	}
}

void ACxxPlayerController::SetZoomScale(float value)
{
	if (value == 0.0f) return;
	zoomScale = value;
}