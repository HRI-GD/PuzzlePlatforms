// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    NumberOfPlayers++;
    if(NumberOfPlayers >= 2)
    {
        UWorld* World = GetWorld();
        if(!ensure(World != nullptr)) return;
        bUseSeamlessTravel = true;
        World->ServerTravel(FString("/Game/PuzzlePlatforms/Maps/Game?listen"));
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    NumberOfPlayers--;
}