// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASWeaponSwitch.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include <Character/GASCharacter.h>


UGASWeaponSwitch::UGASWeaponSwitch()
{
}

void UGASWeaponSwitch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	//注意character需要实时获取，因为角色死亡后会被销毁，重新生成的角色是新的对象，若保留之前的指针会导致访问空指针
	AGASCharacter* Character = Cast<AGASCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	bool bIsCombined = Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Combined);

	if (bIsCombined)
	{
		Character->SwitchToDualWeapons(); // 分离
	}
	else
	{
		Character->SwitchToCombinedWeapon(); // 合体
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGASWeaponSwitch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


