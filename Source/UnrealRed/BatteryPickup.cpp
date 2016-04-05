// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealRed.h"
#include "BatteryPickup.h"

//Set default values
ABatteryPickup::ABatteryPickup()
{
	GetMesh()->SetSimulatePhysics(true);
}

void ABatteryPickup::WasCollected_Implementation()
{
	// Use the base pickup behvaiour
	Super::WasCollected_Implementation();
	// Destroy the battery
	Destroy();
}
