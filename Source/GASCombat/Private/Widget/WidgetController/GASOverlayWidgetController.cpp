// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetController/GASOverlayWidgetController.h"
#include <AbilitySystem/GASAttributeSet.h>
#include <AbilitySystem/GASAbilitySystemComponent.h>

void UGASOverlayWidgetController::BroadcastInitialValue()
{
	UGASAttributeSet* GASAttributeSet = Cast<UGASAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(GASAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(GASAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(GASAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(GASAttributeSet->GetMaxMana());
}

void UGASOverlayWidgetController::BindCallbacksToDependencies()
{
	UGASAttributeSet* GASAttributeSet = Cast<UGASAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GASAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GASAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GASAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GASAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);
	Cast<UGASAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					if (Row)
					{
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		}
	);
}


