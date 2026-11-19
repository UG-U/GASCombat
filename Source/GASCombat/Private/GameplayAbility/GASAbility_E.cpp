  // Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASAbility_E.h"
#include"GASGameplayTags.h"
#include"DataAsset/SkillDataAsset.h"
#include"AbilitySystemComponent.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include"AbilitySystem/GASAttributeSet.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <AbilitySystemGlobals.h>
#include <GenericTeamAgentInterface.h>

void UGASAbility_E::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 提交动态消耗(如消耗蓝量,应用冷却)，只有提交成功才会继续执行技能逻辑，否则结束技能
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }


    // 判断形态
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    bool bIsDual = ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual);
    FSkillFormData CurrentForm = bIsDual ? SkillData->DualFormData : SkillData->CombinedFormData;


    // 播放对应形态的蒙太奇
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, CurrentForm.SkillMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGASAbility_E::OnMontageCompleted);
    MontageTask->ReadyForActivation();


    //监听伤害判定事件
	UAbilityTask_WaitGameplayEvent* DamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, GASGameplayTags::Event_Combat_DamageFrame);
	DamageEventTask->EventReceived.AddDynamic(this, &UGASAbility_E::OnDamageFrameEvent);
	DamageEventTask->ReadyForActivation();

}

void UGASAbility_E::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}





void UGASAbility_E::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGASAbility_E::OnDamageFrameEvent(FGameplayEventData Payload)
{
    TArray<FHitResult> HitResults = PerformMeleeTrace();
    if (HitResults.Num() == 0) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    //去重
    TSet<AActor*> UniqueTargets;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* Target = Hit.GetActor();
        if (!Target || UniqueTargets.Contains(Target)) continue;
        UniqueTargets.Add(Target);

        UAbilitySystemComponent* TargetASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
        //TargetASC为空或者目标处于无敌状态，跳过
        if (!TargetASC || TargetASC->HasMatchingGameplayTag(GASGameplayTags::State_Invincible)) continue;

        //同阵营忽略
        AActor* Avatar = GetAvatarActorFromActorInfo();
        if (!Avatar) continue;
        IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Avatar);
        IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(Target);
        if (OwnerTeam && TargetTeam && OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
        {

            continue;
        }

        // =================  网络同步（服务器权威） =================
        if (!GetOwningActorFromActorInfo()->HasAuthority())
        {
            // 客户端只负责表现（比如在 Hit.ImpactPoint 播放火花特效）
            // 伤害结算交由服务器处理，然后通过 GE 自动同步回客户端
            continue;
        }

        // 判断形态
        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        bool bIsDual = ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual);
        FSkillFormData CurrentForm = bIsDual ? SkillData->DualFormData : SkillData->CombinedFormData;

        //应用伤害效果
        TSubclassOf<UGameplayEffect> DamageEffect = CurrentForm.DamageGE;

        FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
        ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
        ContextHandle.AddHitResult(Hit);

        FGameplayEffectSpecHandle SpecHandle =
            SourceASC->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), ContextHandle);

        if (SpecHandle.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}

TArray<FHitResult> UGASAbility_E::PerformMeleeTrace()
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
