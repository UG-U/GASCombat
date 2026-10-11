// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/GASHUD.h"
#include"Widget/GASUserWidget.h"
#include"Widget/WidgetController/GASOverlayWidgetController.h"
#include"Widget/WidgetController/MenuAttributeWidgetController.h"
void AGASHUD::InitOverlay(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetClass);
	check(OverlayWidgetControllerClass);
	check(MenuAttributeWidgetControllerClass);
	UGASUserWidget* Widget = CreateWidget<UGASUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UGASUserWidget>(Widget);
	const FWidgetControllerParams WCParams(PS, PC, ASC, AS);
	UGASOverlayWidgetController* OverlayController = GetGASOverlayWidgetController(WCParams);
	UMenuAttributeWidgetController* MenuController = GetMenuAttributeWidgetController(WCParams);
	OverlayWidget->SetWidgetController(OverlayController);
	OverlayController->BroadcastInitialValue();
	Widget->AddToViewport();
}

UGASOverlayWidgetController* AGASHUD::GetGASOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UGASOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

UMenuAttributeWidgetController* AGASHUD::GetMenuAttributeWidgetController(const FWidgetControllerParams& WCParams)
{
	if (MenuAttributeWidgetController == nullptr)
	{
		MenuAttributeWidgetController = NewObject<UMenuAttributeWidgetController>(this, MenuAttributeWidgetControllerClass);
		MenuAttributeWidgetController->SetWidgetControllerParams(WCParams);
		MenuAttributeWidgetController->BindCallbacksToDependencies();
		return MenuAttributeWidgetController;
	}
	return MenuAttributeWidgetController;
}
