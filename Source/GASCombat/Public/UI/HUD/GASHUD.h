// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GASHUD.generated.h"



class APlayerState;
class APlayerController;
class UAbilitySystemComponent;
class UAttributeSet;
class UOverlayWidgetController;
class UGASUserWidget;
class UGASOverlayWidgetController;
class UMenuAttributeWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API AGASHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	void InitOverlay(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	UGASOverlayWidgetController* GetGASOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UMenuAttributeWidgetController* GetMenuAttributeWidgetController(const FWidgetControllerParams& WCParams);
	UPROPERTY()
	TObjectPtr<UGASUserWidget>OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGASUserWidget>OverlayWidgetClass;
	UPROPERTY()
	TObjectPtr<UGASOverlayWidgetController>OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGASOverlayWidgetController>OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UMenuAttributeWidgetController>MenuAttributeWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuAttributeWidgetController>MenuAttributeWidgetControllerClass;
};
