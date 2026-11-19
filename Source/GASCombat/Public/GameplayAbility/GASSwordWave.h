// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GASSwordWave.generated.h"


class ASwordWaveProjectile;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASSwordWave : public UGASGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGASSwordWave();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
    // 蒙太奇播放完成回调
    UFUNCTION()
    void OnMontageCompleted();

    UFUNCTION()
    void OnSpawnWaveEvent(FGameplayEventData Payload); // 监听动画事件

    UPROPERTY(EditAnywhere,Category="Montage")
	TObjectPtr<UAnimMontage> SwordWaveMontage;

    UPROPERTY(EditAnywhere,Category="Skill")
	float SpawnDistance = 100.f; // 剑气生成距离施法者的偏移量

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SpawnSpeed = 2000.f; // 剑气生成速度

    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<ASwordWaveProjectile> SwordWaveClass;


    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<UGameplayEffect> SkillDamageClass;

};
