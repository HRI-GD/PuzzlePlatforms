// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "MenuSystem/MainMenu.h"


UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if(!ensure(MenuBPClass.Class != nullptr)) return;   

    this->MenuClass = MenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
    if (MenuClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Menu Class: %s"), *MenuClass->GetName());
    }
}

void UPuzzlePlatformsGameInstance::LoadMenu()
{
    // MenuClass가 존재하는지 확인
    if(!ensure(MenuClass != nullptr)) return;

    // MenuClass를 사용하여 메뉴 위젯 생성
    this->Menu = CreateWidget<UMainMenu>(this, MenuClass);
    if(!ensure(Menu != nullptr)) return;

    this->Menu->Setup();
    this->Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host()
{
    if (Menu != nullptr)
    {
        this->Menu->Teardown();
    }
    
    // 엔진 가져오기
    UEngine* Engine = GetEngine();
    if(!ensure(Engine != nullptr)) return;

    // 디버그 메시지 출력
    Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hosting"), true, FVector2D(1.5f, 1.5f));

    // 월드 가져오기
    UWorld *World = GetWorld();
    if(!ensure(World != nullptr)) return;

    // 서버 트래블
    World->ServerTravel(FString::Printf(TEXT("/Game/ThirdPerson/Lvl_ThirdPerson?listen")));
}

void UPuzzlePlatformsGameInstance::Join(const FString& Address)
{
    if (Menu != nullptr)
    {
        this->Menu->Teardown();
    }
    
    // 엔진 가져오기
    UEngine *Engine = GetEngine();
    if(!ensure(Engine != nullptr)) return;

    // 디버그 메시지 출력
    Engine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Joining %s"), *Address), true, FVector2D(1.5f, 1.5f));

    // PlayerController 가져오기
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    // Address로 클라이언트 트래블
    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}