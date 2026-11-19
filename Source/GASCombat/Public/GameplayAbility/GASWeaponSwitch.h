// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GASWeaponSwitch.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASWeaponSwitch : public UGASGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGASWeaponSwitch();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;


};
