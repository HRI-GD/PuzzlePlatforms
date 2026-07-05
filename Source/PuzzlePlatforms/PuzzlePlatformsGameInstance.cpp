// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = FName(TEXT("My Game Session"));

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
    // MainMenu 클래스 찾기
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if(!ensure(MenuBPClass.Class != nullptr)) return;   
    this->MenuClass = MenuBPClass.Class;

    // InGameMenu 클래스 찾기
    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
    if(!ensure(InGameMenuBPClass.Class != nullptr)) return;
    this->InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Online Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
        this->SessionInterface = OnlineSubsystem->GetSessionInterface();
        if(this->SessionInterface.IsValid())
        {
            this->SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            this->SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Online Subsystem not found"));
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

void UPuzzlePlatformsGameInstance::LoadInGameMenu()
{
    // InGameMenuClass가 존재하는지 확인
    if(!ensure(InGameMenuClass != nullptr)) return;

    // InGameMenuClass를 사용하여 인게임 메뉴 위젯 생성
    UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
    if(!ensure(InGameMenu != nullptr)) return;

    InGameMenu->Setup();
    InGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel(TEXT("/Game/PuzzlePlatforms/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);

}

void UPuzzlePlatformsGameInstance::Host()
{
    if (this->SessionInterface.IsValid())
    {
        auto ExistingSession = this->SessionInterface->GetNamedSession(SESSION_NAME);
        if(ExistingSession != nullptr)
        {
            this->SessionInterface->DestroySession(SESSION_NAME);
            // Init에서 OnDestroySessionCompleteDelegates 등록했기 때문에 DestroySession 후 OnDestroySessionComplete 함수가 호출됨
        }
        else
        {
            CreateSession();
        }
    }
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
    if(SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;
        this->SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
        // Init에서 OnCreateSessionCompleteDelegates 등록했기 때문에 CreateSession 후 OnCreateSessionComplete 함수가 호출됨
    }

}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create session"));
        return;
    }

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

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
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