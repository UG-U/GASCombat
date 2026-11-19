// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GASAbility_E.generated.h"

class USkillDataAsset;

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASAbility_E : public UGASGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnDamageFrameEvent(FGameplayEventData Payload);

	// 执行近战攻击检测,找敌人
	TArray<FHitResult> PerformMeleeTrace();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<USkillDataAsset> SkillData;


	// 近战检测参数
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float TraceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float TraceDistance = 150.0f;

};
