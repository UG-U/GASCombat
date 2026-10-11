// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_ComboWindow.h"
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include"GASGameplayTags.h"
void UAnimNotify_ComboWindow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UAbilitySystemComponent* ASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
        {
            // 发送连击窗口开启/关闭事件
            FGameplayEventData Payload;
            Payload.EventTag = bOpenWindow
                ? GASGameplayTags::Event_Combo_Window_Open
                : GASGameplayTags::Event_Combo_Window_Close;
            ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
        }
    }
}
