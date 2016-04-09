// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "UnrealRedGameMode.generated.h"

UCLASS(minimalapi)
class AUnrealRedGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUnrealRedGameMode();

	virtual void Tick(float DeltaTime) override;

protected:
	//The rate at which the character loses power
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float DecayRate;
};



