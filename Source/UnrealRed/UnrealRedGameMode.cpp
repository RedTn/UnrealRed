// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealRed.h"
#include "UnrealRedGameMode.h"
#include "UnrealRedCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"

AUnrealRedGameMode::AUnrealRedGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//base decay rate
	DecayRate = 0.01f;
}

void AUnrealRedGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check that we are using the battery collector character
	AUnrealRedCharacter* MyCharacter = Cast<AUnrealRedCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter) {
		if (MyCharacter->GetCurrentPower() > PowerToWin) {
			SetCurrentState(EBatteryPlayState::EWon);
		}
		//if the character's power is positive
		else if (MyCharacter->GetCurrentPower() > 0) {
			//decrease the character's power using the decay rate
			MyCharacter->UpdatePower(-DeltaTime*DecayRate*(MyCharacter->GetInitialPower()));
		}
		else {
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
	}
}

float AUnrealRedGameMode::GetPowerToWin() const
{
	return PowerToWin;
}

void AUnrealRedGameMode::BeginPlay()
{
	Super::BeginPlay();

	// find all spawn volume actors
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors) {
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor) {
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}

	SetCurrentState(EBatteryPlayState::EPlaying);

	//set score to beat
	AUnrealRedCharacter* MyCharacter = Cast<AUnrealRedCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter) {
		PowerToWin = (MyCharacter->GetInitialPower())*1.25f;
	}

	if (HUDWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}

}

EBatteryPlayState AUnrealRedGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AUnrealRedGameMode::SetCurrentState(EBatteryPlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void AUnrealRedGameMode::HandleNewState(EBatteryPlayState NewState)
{
	switch (NewState) {
	case EBatteryPlayState::EPlaying:
	{
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(true);
		}
	}
	break;
	case EBatteryPlayState::EWon:
	{
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(false);
		}
	}
	break;
	case EBatteryPlayState::EGameOver:
	{
		/* Spawn volumes inactive
		* Block player input
		* Ragdoll character
		*/
		for (ASpawnVolume* Volume : SpawnVolumeActors) {
			Volume->SetSpawningActive(false);
		}
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController) {
			PlayerController->SetCinematicMode(true, false, false, true, true);
		}
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (MyCharacter) {
			MyCharacter->GetMesh()->SetSimulatePhysics(true);
			MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
		}
	}
	break;
	default:
	case EBatteryPlayState::EUnknown:
	{

	}
	break;
	}
}
