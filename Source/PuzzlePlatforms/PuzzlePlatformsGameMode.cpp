// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"

APuzzlePlatformsGameMode::APuzzlePlatformsGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// UE5.5+ ThirdPerson 템플릿은 IMC_Default를 PlayerController에서 추가함
	// PlayerControllerClass를 지정하지 않으면 입력 매핑이 안 걸려 캐릭터가 안 움직임
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController"));
	if (PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
