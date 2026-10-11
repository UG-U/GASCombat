// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASAbilitySystemComponent.generated.h"


//多播委托声明
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&);
/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	//ActorInfo设置完之后的回调（故调用时机在actorinfo之后），通常是各类函数的委托
	void AbilityActorInfoSet();

	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	void AddCharacterAbility(TArray<TSubclassOf<UGameplayAbility>>Abilities);

	void AbilityInputTagHeld(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	FEffectAssetTags EffectAssetTags;

};
