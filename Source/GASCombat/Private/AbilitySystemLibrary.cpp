// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemLibrary.h"
#include <Kismet/GameplayStatics.h>
#include <UI/HUD/GASHUD.h>
#include <Player/GASPlayerState.h>
#include"Widget/WidgetController/GASWidgetController.h"

UGASOverlayWidgetController* UAbilitySystemLibrary::GetGASOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AGASHUD* GASHUD = Cast<AGASHUD>(PC->GetHUD()))
		{
			AGASPlayerState* PS = PC->GetPlayerState<AGASPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PS, PC, ASC, AS);
			return GASHUD->GetGASOverlayWidgetController(WidgetControllerParams);
		}
	}
    return nullptr;
}

UMenuAttributeWidgetController* UAbilitySystemLibrary::GetMenuAttributeWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AGASHUD* GASHUD = Cast<AGASHUD>(PC->GetHUD()))
		{
			AGASPlayerState* PS = PC->GetPlayerState<AGASPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PS, PC, ASC, AS);
			return GASHUD->GetMenuAttributeWidgetController(WidgetControllerParams);
		}
	}
    return nullptr;
}
