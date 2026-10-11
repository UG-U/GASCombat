// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetController/GASWidgetController.h"

void UGASWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerState = WCParams.PlayerState;
	PlayerController = WCParams.PlayerController;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UGASWidgetController::BroadcastInitialValue()
{
}

void UGASWidgetController::BindCallbacksToDependencies()
{
}
