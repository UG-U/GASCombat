// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASAbility_F.h"
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include "DataAsset/SkillDataAsset.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include "GameplayEffect.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
void UGASAbility_F::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	bool bIsDual = ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual);
	FSkillFormData CurrentForm = bIsDual ? SkillDataAsset->DualFormData : SkillDataAsset->CombinedFormData;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, CurrentForm.SkillMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGASAbility_F::OnMontageCompleted);
	MontageTask->ReadyForActivation();

	// 监听伤害帧
	UAbilityTask_WaitGameplayEvent* DamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, GASGameplayTags::Event_Combat_DamageFrame);
	DamageEventTask->EventReceived.AddDynamic(this, &UGASAbility_F::OnDamageFrameEvent);
	DamageEventTask->ReadyForActivation();

	TryWarpToNearestEnemy(FindEnemyRadius);

}

void UGASAbility_F::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGASAbility_F::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGASAbility_F::OnDamageFrameEvent(FGameplayEventData Payload)
{
	ApplyMeleeDamage(PerformMeleeTrace());
}

TSubclassOf<UGameplayEffect> UGASAbility_F::GetMeleeDamageEffect() const
{
	if (!SkillDataAsset)
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const bool bIsDual = ASC && ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual);
	return bIsDual ? SkillDataAsset->DualFormData.DamageGE : SkillDataAsset->CombinedFormData.DamageGE;
}
