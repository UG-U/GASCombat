// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASMeleeAbilityBase.h"
#include "GASAbility_F.generated.h"


class USkillDataAsset;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASAbility_F : public UGASMeleeAbilityBase
{
	GENERATED_BODY()
	

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnDamageFrameEvent(FGameplayEventData Payload);

	virtual TSubclassOf<UGameplayEffect> GetMeleeDamageEffect() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<USkillDataAsset>SkillDataAsset;

};
