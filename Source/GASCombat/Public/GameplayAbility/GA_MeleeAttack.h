// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASMeleeAbilityBase.h"
#include "GA_MeleeAttack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitInputPress;
class UGameplayEffect;
class UComboDataAsset;
class UMotionWarpingComponent;

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGA_MeleeAttack : public UGASMeleeAbilityBase
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




    // 辅助函数：获取当前形态的连招数据
    UComboDataAsset* GetComboConfigByForm() const;

    virtual TSubclassOf<UGameplayEffect> GetMeleeDamageEffect() const override;
    virtual float GetMeleeDamageMultiplier() const override;


    // 当前连击段数
    int32 ComboIndex = 0;

    // 是否处于连击窗口
    bool bInComboWindow = false;

    // 是否缓冲了输入
    bool bInputBuffered = false;


    // 攻击蒙太奇
	TObjectPtr<UAnimMontage>CurrentMontage;

	UPROPERTY(EditAnywhere, Category = "Combo")
    TObjectPtr<UComboDataAsset>DualComboConfig;

    UPROPERTY(EditAnywhere, Category = "Combo")
    TObjectPtr<UComboDataAsset>CombinedComboConfig;

    //运行选中的连招配置
    UPROPERTY()
	TObjectPtr<UComboDataAsset> CurrentComboConfig;

    // 伤害 GE
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TSubclassOf<UGameplayEffect> DamageEffectClass;


    // 近战检测参数


    // 当前激活的蒙太奇 Task
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitInputPress> WaitInputTask;
};
