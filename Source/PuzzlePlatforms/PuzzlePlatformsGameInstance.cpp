// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
    UE_LOG(LogTemp, Warning, TEXT("Game Instance Constructor"));
}

void UPuzzlePlatformsGameInstance::Init()
{
    UE_LOG(LogTemp, Warning, TEXT("Game Instance Init"));
}

void UPuzzlePlatformsGameInstance::Host()
{
    UEngine* Engine = GetEngine();
    if(!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hosting"), true, FVector2D(1.5f, 1.5f));

    UWorld *World = GetWorld();
    if(!ensure(World != nullptr)) return;

    World->ServerTravel(FString::Printf(TEXT("/Game/ThirdPerson/Lvl_ThirdPerson?listen")));
}

void UPuzzlePlatformsGameInstance::Join(const FString& Address)
{
    UEngine *Engine = GetEngine();
    if(!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Joining %s"), *Address), true, FVector2D(1.5f, 1.5f));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}