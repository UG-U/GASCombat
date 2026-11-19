// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASAbility_F.h"
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include"DataAsset/SkillDataAsset.h"
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

}

void UGASAbility_F::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGASAbility_F::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
