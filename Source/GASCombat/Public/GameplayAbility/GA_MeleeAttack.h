// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GA_MeleeAttack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitInputPress;
class UGameplayEffect;


/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGA_MeleeAttack : public UGASGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGA_MeleeAttack();
    
protected:
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

    // 蒙太奇播放完成回调
    UFUNCTION()
    void OnMontageCompleted();

    // 蒙太奇被中断回调
    UFUNCTION()
    void OnMontageInterrupted();

    // 连击窗口开启回调
    UFUNCTION()
    void OnComboWindowOpened(FGameplayEventData Payload);

    // 连击窗口关闭回调
    UFUNCTION()
    void OnComboWindowClosed(FGameplayEventData Payload);

    // 输入缓冲回调（玩家在连击窗口内按了攻击键）
    UFUNCTION()
    void OnAttackInputBuffered(float TimeWaited);

    void AdvanceCombo();

    // 攻击判定回调，逻辑结算，打敌人（AnimNotify 触发）
    UFUNCTION()
    void OnDamageFrame(FGameplayEventData Payload);

	// 执行近战攻击检测,找敌人
    TArray<FHitResult> PerformMeleeTrace();

    //寻找距离最近的敌人
    UFUNCTION(BlueprintCallable, Category = "Combat")
    AActor* FindClosestEnemyInRange(float Range);

    // 在 UGA_MeleeAttack.h 中
    UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
    float WarpDistanceOffset = 100.0f; // 吸附到敌人面前的偏移距离

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
    float FindEnemyRadius = 500.0f; // 寻找敌人的半径

    // 当前连击段数
    int32 ComboIndex = 0;

    // 是否处于连击窗口
    bool bInComboWindow = false;

    // 是否缓冲了输入
    bool bInputBuffered = false;

    // 连击配置：每段的蒙太奇 Section 名
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TArray<FName> ComboSections = { "Attack1", "Attack2", "Attack3" };

    // 攻击蒙太奇
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TObjectPtr<UAnimMontage> AttackMontage;

    // 伤害 GE
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    // 近战检测参数
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    float TraceRadius = 50.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    float TraceDistance = 150.0f;

    // 当前激活的蒙太奇 Task
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitInputPress> WaitInputTask;
};
