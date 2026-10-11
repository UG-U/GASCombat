// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GASUserWidget.h"

void UGASUserWidget::SetWidgetController(UObject* NewWidgetController)
{
	WidgetController = NewWidgetController;
	WidgetControllerSet();
}
