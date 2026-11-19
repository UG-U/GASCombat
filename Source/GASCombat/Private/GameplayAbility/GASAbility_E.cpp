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


    TryWarpToNearestEnemy(FindEnemyRadius);

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
    ApplyMeleeDamage(PerformMeleeTrace());
}

TSubclassOf<UGameplayEffect> UGASAbility_E::GetMeleeDamageEffect() const
{
	if (!SkillData)
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const bool bIsDual = ASC && ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual);
	return bIsDual ? SkillData->DualFormData.DamageGE : SkillData->CombinedFormData.DamageGE;
}
