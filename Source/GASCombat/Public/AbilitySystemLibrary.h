// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemLibrary.generated.h"


class UGASOverlayWidgetController;
class UMenuAttributeWidgetController;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API UAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UGASOverlayWidgetController* GetGASOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UMenuAttributeWidgetController* GetMenuAttributeWidgetController(const UObject* WorldContextObject);
};
