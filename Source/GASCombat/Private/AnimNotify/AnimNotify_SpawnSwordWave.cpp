// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_SpawnSwordWave.h"
#include <AbilitySystemGlobals.h>
#include <AbilitySystemComponent.h>
#include "GASGameplayTags.h"

void UAnimNotify_SpawnSwordWave::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		{
			FGameplayEventData Payload;
			Payload.EventTag = FGameplayTag::RequestGameplayTag("Event.Skill.SwordWave");
			ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}
}
