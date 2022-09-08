// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONPLUGIN_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public: 
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected: 
	
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
	
private: 
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void onHostButtonClicked();

	UFUNCTION()
	void onJoinButtonClicked();

	void MenuTeardown();

	// Subsystem that handles all online session functionality
	class UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;
};
