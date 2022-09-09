// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

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

	if (MultiplayerSessionSubsystem) {
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &UMenu::OnFindSession);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
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
		HostButton->OnClicked.AddDynamic(this, &UMenu::OnHostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::OnJoinButtonClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTeardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

}

void UMenu::OnHostButtonClicked()
{
	if (MultiplayerSessionSubsystem) {
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::OnJoinButtonClicked()
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
	if (MultiplayerSessionSubsystem) {
		MultiplayerSessionSubsystem->FindSessions(20);
	}
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("onCreateSession called"))
		);
	}
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Yellow,
					FString(TEXT("Session Created Successfully"))
				);
			}
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Session Creation Failed"))
			);
		}
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{

	if (MultiplayerSessionSubsystem == nullptr)
	{
		return;
	}
	
	// select most appropriate match 
	for (auto Result : SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString SettingsMatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsMatchType);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}
		if (SettingsMatchType == MatchType) 
		{
			MultiplayerSessionSubsystem->JoinSession(Result);
			return;
		}
		
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
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
