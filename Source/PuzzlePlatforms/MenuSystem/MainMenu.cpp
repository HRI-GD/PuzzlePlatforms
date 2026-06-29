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

void UMainMenu::SetMenuInterface(IMenuInterface* InMenuInterface)
{
    this->MenuInterface = InMenuInterface;
}

void UMainMenu::Setup()
{
    // 메뉴 위젯을 뷰포트에 추가
    this->AddToViewport();

    // Input Mode와 Cursor 설정을 위해 PlayerController를 가져옴
    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    // 마우스 커서를 표시하고 포커스를 메뉴 위젯에 설정
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(this->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PlayerController->SetInputMode(InputMode);
    PlayerController->bShowMouseCursor = true;
}

void UMainMenu::Teardown()
{
    this->RemoveFromParent();

    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    FInputModeGameOnly InputMode;
    PlayerController->SetInputMode(InputMode);
    PlayerController->bShowMouseCursor = false;
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