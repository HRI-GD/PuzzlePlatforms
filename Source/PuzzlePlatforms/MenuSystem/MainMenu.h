// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"


USTRUCT()
struct FServerData
{
	GENERATED_BODY()
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerData);

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize() override;
	
private:
	TSubclassOf<class UUserWidget> ServerRowClass;
	// 메인 메뉴
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	// 호스트 메뉴
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HostServerButton;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerHostName;

	// 조인 메뉴
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinServerButton;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	// 메뉴 스위처
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UFUNCTION(Exec)
	void QuitGame();
	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void OpenHostMenu();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void OpenMainMenu();
	UFUNCTION()
	void JoinServer();

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();
};