// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_DamageFrame.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include"GASGameplayTags.h"
void UAnimNotify_DamageFrame::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UAbilitySystemComponent* ASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
        {
            FGameplayEventData Payload;
            Payload.EventTag = FGameplayTag::RequestGameplayTag("Event.Combat.DamageFrame");
            ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
        }
    }
}
