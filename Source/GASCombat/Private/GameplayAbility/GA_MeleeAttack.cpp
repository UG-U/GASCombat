// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GA_MeleeAttack.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <Abilities/Tasks/AbilityTask_WaitInputPress.h>
#include <AbilitySystemGlobals.h>
#include "GameplayEffect.h"

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

    // 2. 重置连击状态
    ComboIndex = 0;
    bInComboWindow = false;
    bInputBuffered = false;

    // 3. 添加攻击状态标签，防止释放其他技能
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->AddLooseGameplayTag(GASGameplayTags::State_Attacking);
    }

    // 4. 检查蒙太奇和段数配置
    if (!AttackMontage || ComboSections.Num() == 0)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 5. 播放蒙太奇（从第一段开始）
    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, AttackMontage, 1.0f, ComboSections[0]);

    MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
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
    // 移除攻击状态标签
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->RemoveLooseGameplayTag(GASGameplayTags::State_Attacking);
    }

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

    if (ComboIndex >= ComboSections.Num())
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }
    //通过 ASC 控制当前蒙太奇跳转 Section
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        FName NextSection = ComboSections[ComboIndex];
        ASC->CurrentMontageJumpToSection(NextSection);
    }
    else
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UGA_MeleeAttack::OnDamageFrame(FGameplayEventData Payload)
{
    TArray<FHitResult> HitResults = PerformMeleeTrace();
    if (HitResults.Num() == 0) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* Target = Hit.GetActor();
        if (!Target) continue;

        UAbilitySystemComponent* TargetASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
        if (!TargetASC) continue;

        FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
        ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
        ContextHandle.AddHitResult(Hit);

        FGameplayEffectSpecHandle SpecHandle =
            SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

        if (SpecHandle.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}

TArray<FHitResult> UGA_MeleeAttack::PerformMeleeTrace()
{
    TArray<FHitResult> HitResults;
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return HitResults;

    FVector Start = Avatar->GetActorLocation();
    FVector Forward = Avatar->GetActorForwardVector();
    FVector End = Start + Forward * TraceDistance;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Avatar);

    GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(TraceRadius),
        Params
    );

    return HitResults;
}
