// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiplayerSessionSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeSetting;
			InputModeSetting.SetWidgetToFocus(TakeWidget());
			InputModeSetting.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeSetting);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}

}

bool UMenu::Initialize()
{
	// unless the superclass returns false, we'll always return true for intialized
	if (!Super::Initialize())
	{
		return false;
	}

	// bind callbacks to host and join buttons
	// Constructor is too early, have to wait until widget fully setup before creating bindings
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::onHostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::onJoinButtonClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTeardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

}

void UMenu::onHostButtonClicked()
{

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Host Button Clicked"))
		);
	}

	if (MultiplayerSessionSubsystem) {
		MultiplayerSessionSubsystem->CreateSession(8, FString(TEXT("FreeForAll")));
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("Game/ThirdPerson/Maps/Lobby?listen");

		}
	}
}

void UMenu::onJoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Join Button Clicked"))
		);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		FInputModeGameOnly InputModeSetting;
		PlayerController->SetInputMode(InputModeSetting);
		PlayerController->SetShowMouseCursor(false);
	}
}
