// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GA_MeleeAttack.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <Abilities/Tasks/AbilityTask_WaitInputPress.h>
#include <AbilitySystemGlobals.h>
#include "GameplayEffect.h"
#include "AbilitySystem/GASAttributeSet.h"
#include <GenericTeamAgentInterface.h>
#include <MotionWarpingComponent.h>
#include"DataAsset/ComboDataAsset.h"


UGA_MeleeAttack::UGA_MeleeAttack()
{
    // 可选：设置技能标签、冷却等
}

void UGA_MeleeAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 1. 提交消耗和冷却
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }


    TryWarpToNearestEnemy(FindEnemyRadius);

    // 2. 重置连击状态
    ComboIndex = 0;
    bInComboWindow = false;
    bInputBuffered = false;


	CurrentComboConfig = GetComboConfigByForm();
	//检查连击配置是否有效
    if (!CurrentComboConfig || CurrentComboConfig->ComboSteps.Num() == 0)
    {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
	//获取当前连击的第一段蒙太奇和 Section
	const FComboStep& FirstStep = CurrentComboConfig->ComboSteps[0];
	CurrentMontage = FirstStep.Montage;


    // 5. 播放蒙太奇（从第一段开始）Task标准用法：创建→绑定→启动
    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, CurrentMontage, 1.0f, FirstStep.SectionName);

    MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
    //这里被取消和被中断，之后的回调函数逻辑一样，故用同一个
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
    MontageTask->ReadyForActivation();

    // 6. 监听连击窗口开启事件
    UAbilityTask_WaitGameplayEvent* WaitComboOpen =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, GASGameplayTags::Event_Combo_Window_Open, nullptr, false, true);
    WaitComboOpen->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnComboWindowOpened);
    WaitComboOpen->ReadyForActivation();

    // 7. 监听连击窗口关闭事件
    UAbilityTask_WaitGameplayEvent* WaitComboClose =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, GASGameplayTags::Event_Combo_Window_Close, nullptr, false, true);
    WaitComboClose->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnComboWindowClosed);
    WaitComboClose->ReadyForActivation();

    // 8. 监听伤害判定事件
    UAbilityTask_WaitGameplayEvent* WaitDamage =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, FGameplayTag::RequestGameplayTag("Event.Combat.DamageFrame"), nullptr, false, true);
    WaitDamage->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnDamageFrame);
    WaitDamage->ReadyForActivation();
}

void UGA_MeleeAttack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    //C++继承通用原则：先清理子类，最后清理父类

    // 结束吸附，避免旧的 WarpTarget 影响后续蒙太奇
    ClearWarpTarget();

    // 结束蒙太奇任务
    if (MontageTask && IsValid(MontageTask))
    {
        MontageTask->EndTask();
        MontageTask = nullptr;
    }

    // 结束等待输入任务
    if (WaitInputTask && IsValid(WaitInputTask))
    {
        WaitInputTask->EndTask();
        WaitInputTask = nullptr;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_MeleeAttack::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MeleeAttack::OnComboWindowOpened(FGameplayEventData Payload)
{
    bInComboWindow = true;

    if (bInputBuffered)
    {
        bInputBuffered = false;
        AdvanceCombo();
    }
    else
    {
        // 等待玩家在窗口内再次按下攻击键
        WaitInputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, true);
        WaitInputTask->OnPress.AddDynamic(this, &UGA_MeleeAttack::OnAttackInputBuffered);
        WaitInputTask->ReadyForActivation();
    }
}

void UGA_MeleeAttack::OnComboWindowClosed(FGameplayEventData Payload)
{
    bInComboWindow = false;
    bInputBuffered = false;

    if (WaitInputTask && IsValid(WaitInputTask))
    {
        WaitInputTask->EndTask();
        WaitInputTask = nullptr;
    }
}

void UGA_MeleeAttack::OnAttackInputBuffered(float TimeWaited)
{
    bInputBuffered = true;

    if (bInComboWindow)
    {
        bInputBuffered = false;
        AdvanceCombo();
    }
}

void UGA_MeleeAttack::AdvanceCombo()
{
    ComboIndex++;

    // 1. 检查是否打完全部连招
    if (ComboIndex >= CurrentComboConfig->ComboSteps.Num())
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // 2. 获取下一段的配置
    const FComboStep& NextStep = CurrentComboConfig->ComboSteps[ComboIndex];
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    if (!NextStep.Montage || !ASC)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    TryWarpToNearestEnemy(FindEnemyRadius);

    // 3. 判断是否切换了蒙太奇
    if (NextStep.Montage == CurrentMontage)
    {
        // 情况 A：同一个蒙太奇，只是跳到下一段 Section
        FName NextSection = NextStep.SectionName;
        // 如果没用 Section 机制，传 NAME_None 会从头播（通常不建议）
        ASC->CurrentMontageJumpToSection(NextSection);
    }
    else
    {
        // 情况 B：蒙太奇变了，必须结束旧 Task，开启新 Task
        if (MontageTask && IsValid(MontageTask))
        {
            MontageTask->EndTask();
        }

        CurrentMontage = NextStep.Montage;

        MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, NAME_None, CurrentMontage, 1.0f, NextStep.SectionName);

        MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
        MontageTask->ReadyForActivation();
        UE_LOG(LogTemp, Error, TEXT("MontageTask Activated for Combo Index %d"), ComboIndex);
    }
}

void UGA_MeleeAttack::OnDamageFrame(FGameplayEventData Payload)
{
    if (!CurrentComboConfig || !CurrentComboConfig->ComboSteps.IsValidIndex(ComboIndex)) return;
    ApplyMeleeDamage(PerformMeleeTrace());
}

UComboDataAsset* UGA_MeleeAttack::GetComboConfigByForm() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC && ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual))
    {
		return DualComboConfig;
    }
    return CombinedComboConfig;
}

TSubclassOf<UGameplayEffect> UGA_MeleeAttack::GetMeleeDamageEffect() const
{
    return DamageEffectClass;
}

float UGA_MeleeAttack::GetMeleeDamageMultiplier() const
{
    if (CurrentComboConfig && CurrentComboConfig->ComboSteps.IsValidIndex(ComboIndex))
    {
        return CurrentComboConfig->ComboSteps[ComboIndex].DamageMultiplier;
    }
    return 1.f;
}
