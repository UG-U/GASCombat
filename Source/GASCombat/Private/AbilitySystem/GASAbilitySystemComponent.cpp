// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GASAbilitySystemComponent.h"
#include <GameplayAbility/GASGameplayAbility.h>

void UGASAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UGASAbilitySystemComponent::ClientEffectApplied);
}

void UGASAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC,const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	FGameplayTagContainer TagContainer;
	Spec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UGASAbilitySystemComponent::AddCharacterAbility(TArray<TSubclassOf<UGameplayAbility>> Abilities)
{
	for (auto& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (UGASGameplayAbility* GameplayAbility = Cast<UGASGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(GameplayAbility->InputTag);
			GiveAbility(AbilitySpec);
		}

	}
}

void UGASAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTag(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UGASAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTag(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}
