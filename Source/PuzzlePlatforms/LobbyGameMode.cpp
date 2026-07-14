// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    NumberOfPlayers++;
    if(NumberOfPlayers >= 3)
    {
        GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 10.f);
    }
}

void ALobbyGameMode::StartGame()
{
    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if(!ensure(GameInstance != nullptr)) return;
    GameInstance->StartSession();

    bUseSeamlessTravel = true;

    World->ServerTravel(FString("/Game/PuzzlePlatforms/Maps/Game?listen"));
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    NumberOfPlayers--;
}