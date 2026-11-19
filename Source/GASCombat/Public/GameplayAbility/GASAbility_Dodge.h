// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GASAbility_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASAbility_Dodge : public UGASGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageCompleted();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge Ability")
	TObjectPtr<UAnimMontage> DodgeMontage;

};
