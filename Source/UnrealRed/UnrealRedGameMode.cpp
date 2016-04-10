// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UnrealRed.h"
#include "UnrealRedGameMode.h"
#include "UnrealRedCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

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
		//if the character's power is positive
		if (MyCharacter->GetCurrentPower() > 0) {
			//decrease the character's power using the decay rate
			MyCharacter->UpdatePower(-DeltaTime*DecayRate*(MyCharacter->GetInitialPower()));
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
