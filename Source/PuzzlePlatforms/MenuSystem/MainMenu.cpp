// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if(!Success) return false;

    // TODO: Setup
    if(!ensure(QuitButton != nullptr)) return false;
    this->QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

    if(!ensure(HostButton != nullptr)) return false;
    this->HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    if(!ensure(JoinButton != nullptr)) return false;
    this->JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

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
    MenuInterface->Host();
}

void UMainMenu::OpenJoinMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(JoinMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(JoinMenu);
}

void UMainMenu::OpenMainMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(MainMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::JoinServer()
{
    if(MenuInterface != nullptr)
    {
        if(!ensure(IPAddressField != nullptr)) return;
        const FString& Address = IPAddressField->GetText().ToString();
        MenuInterface->Join(Address);
    }
}