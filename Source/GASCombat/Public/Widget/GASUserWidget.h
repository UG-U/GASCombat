// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GASUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,Category="WidgetController")
	void SetWidgetController(UObject* NewWidgetController);

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UObject>WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent,Category="WidgetController")
	void WidgetControllerSet();
};
