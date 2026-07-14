// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"


UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
    if(!ensure(ServerRowBPClass.Class != nullptr)) return;
    this->ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if(!Success) return false;

    // TODO: Setup
    // 메인 메뉴
    if(!ensure(QuitButton != nullptr)) return false;
    this->QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);
    if(!ensure(HostButton != nullptr)) return false;
    this->HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
    if(!ensure(JoinButton != nullptr)) return false;
    this->JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

    // 호스트 메뉴
    if(!ensure(CancelHostMenuButton != nullptr)) return false;
    this->CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
    if(!ensure(HostServerButton != nullptr)) return false;
    this->HostServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    // 조인 메뉴
    if(!ensure(CancelJoinMenuButton != nullptr)) return false;
    this->CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
    if(!ensure(JoinServerButton != nullptr)) return false;
    this->JoinServerButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);


    return true;
}

void UMainMenu::QuitGame()
{
    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    PlayerController->ConsoleCommand("Quit");
}

void UMainMenu::HostServer()
{
    if(!ensure(MenuInterface != nullptr)) return;
    FString ServerName = this->ServerHostName->GetText().ToString();
    MenuInterface->Host(ServerName);
}

void UMainMenu::OpenHostMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(HostMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenJoinMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(JoinMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(JoinMenu);
    if(MenuInterface != nullptr)
    {
        MenuInterface->RefreshServerList();
    }
}

void UMainMenu::OpenMainMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(MainMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::SetServerList(TArray<FServerData> ServerData)
{
    ServerList->ClearChildren();

    uint32 Index = 0;
    for(const FServerData& ServerDataItem : ServerData)
    {
        UServerRow* ServerRow = CreateWidget<UServerRow>(this, this->ServerRowClass);
        if(!ensure(ServerRow != nullptr)) return;

        ServerRow->ServerName->SetText(FText::FromString(ServerDataItem.Name));
        ServerRow->HostUsername->SetText(FText::FromString(ServerDataItem.HostUsername));
        ServerRow->ConnectionFraction->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ServerDataItem.CurrentPlayers, ServerDataItem.MaxPlayers)));
        ServerRow->Setup(this, Index);

        ServerList->AddChild(ServerRow);
        ++Index;
    }
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
    for(int32 i = 0; i < ServerList->GetChildrenCount(); i++)
    {
        auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
        if(Row != nullptr)
        {
            Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
        }
    }
}

void UMainMenu::JoinServer()
{
    if(SelectedIndex.IsSet() && MenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index: %d"), SelectedIndex.GetValue());
        MenuInterface->Join(this->SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No index selected"));
    }
}