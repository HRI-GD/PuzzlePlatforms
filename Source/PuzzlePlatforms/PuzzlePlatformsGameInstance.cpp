// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = FName(TEXT("GameSession"));
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

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
            this->SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
            this->SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Online Subsystem not found"));
    }

    if(GEngine)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
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

void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
    this->DesiredServerName = ServerName;
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

        // NULL 서브시스템(-nosteam)이면 LAN 모드, Steam이면 온라인 모드
        const bool bIsNull = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
        SessionSettings.bIsLANMatch = bIsNull;
        SessionSettings.bUsesPresence = !bIsNull;
        //UE5 Steam: bUseLobbiesIfAvailable은 bUsesPresence와 같은 값이어야 함
        SessionSettings.bUseLobbiesIfAvailable = !bIsNull;
        SessionSettings.NumPublicConnections = 3;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, this->DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
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
    World->ServerTravel(FString::Printf(TEXT("/Game/PuzzlePlatforms/Maps/Lobby?listen")));
    
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
}

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UObject* WorldContextObject, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    LoadMainMenu();
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if(!this->SessionInterface.IsValid() || !this->SessionSearch.IsValid()) return;
    if(!this->SessionSearch->SearchResults.IsValidIndex(Index)) return;
    this->SessionInterface->JoinSession(0, SESSION_NAME, this->SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::StartSession()
{
    if(this-SessionInterface.IsValid())
    {
        this->SessionInterface->StartSession(SESSION_NAME);
    }

}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if(!this->SessionInterface.IsValid()) return;
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to join session. Result: %d"), static_cast<int32>(Result));
        // 0: Success
        // 1: SessionIsFull
        // 2: SessionDoesNotExist
        // 3: CouldNotRetrieveAddress
        // 4: AlreadyInSession
        // 5: UnknownError
        return;
    }

    FString Address;
    if(!this->SessionInterface->GetResolvedConnectString(SessionName, Address)) return;

    // NULL 서브시스템은 listen 전에 세션을 만들어 포트가 0으로 광고되는 경우가 있음
    // 이 경우 포트를 7777로 변경
    if (Address.EndsWith(TEXT(":0")))
    {
        Address = Address.LeftChop(2) + TEXT(":7777");
    }

    // 메뉴 닫기
    if (this->Menu) this->Menu->Teardown();

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

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    if(SessionInterface.IsValid())
    {
        this->SessionSearch = MakeShareable(new FOnlineSessionSearch());
        if(this->SessionSearch.IsValid())
        {
            // NULL 서브시스템(-nosteam)이면 LAN 검색, Steam이면 온라인 검색
            const bool bIsNull = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
            SessionSearch->bIsLanQuery = bIsNull;
            // 테스트용 AppID 480 (Spacewar)을 사용중이기 때문에 최대 검색 결과를 5000으로 설정
            this->SessionSearch->MaxSearchResults = 5000;
            if (!bIsNull)
            {
                // UE5.5+: SEARCH_PRESENCE 제거됨 → Steam은 SEARCH_LOBBIES 사용
                this->SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
            }
            UE_LOG(LogTemp, Warning, TEXT("Finding Sessions..."));
            this->SessionInterface->FindSessions(0, this->SessionSearch.ToSharedRef());
        }
    }
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    if(Success && this->SessionSearch.IsValid() && Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finished finding sessions"));

        TArray<FServerData> ServerData;
        for(const FOnlineSessionSearchResult& SearchResult : this->SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found Session Name: %s"), *SearchResult.GetSessionIdStr());
            FServerData ServerDataItem;
            
            ServerDataItem.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
            ServerDataItem.CurrentPlayers = ServerDataItem.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
            ServerDataItem.HostUsername = SearchResult.Session.OwningUserName;

            FString ServerName;
            if(SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
            {
                ServerDataItem.Name = ServerName;
            }
            else
            {
                ServerDataItem.Name = "Could not find name.";
            }

            ServerData.Add(ServerDataItem);
        }
        this->Menu->SetServerList(ServerData);
    }
}