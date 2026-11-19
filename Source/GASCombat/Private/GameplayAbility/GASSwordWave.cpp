// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASSwordWave.h"
#include"GameplayEffect/SwordWaveProjectile.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"

UGASSwordWave::UGASSwordWave()
{
}

void UGASSwordWave::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    //冷却
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
    // 添加攻击状态标签，防止释放其他技能
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->AddLooseGameplayTag(GASGameplayTags::State_Attacking);
    }

	//播放动画
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, SwordWaveMontage, 1.f, NAME_None, false);
	MontageTask->OnCompleted.AddDynamic(this, &UGASSwordWave::OnMontageCompleted);
	MontageTask->ReadyForActivation();

	//监听动画中剑气生成事件
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, FGameplayTag::RequestGameplayTag("Event.Skill.SwordWave"), nullptr, false, false);
	EventTask->EventReceived.AddDynamic(this, &UGASSwordWave::OnSpawnWaveEvent);
    EventTask->ReadyForActivation();

}

void UGASSwordWave::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 移除攻击状态标签，允许释放其他技能
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(GASGameplayTags::State_Attacking);
	}


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGASSwordWave::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGASSwordWave::OnSpawnWaveEvent(FGameplayEventData Payload)
{
    //  网络权限：只在服务器生成具有碰撞伤害的剑气实体
    if (!GetOwningActorFromActorInfo()-> HasAuthority()) return;

    AActor* Avatar = GetAvatarActorFromActorInfo();
    FVector SpawnLoc = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * SpawnDistance;
    FRotator SpawnRot = Avatar->GetActorRotation();

    // 生成剑气
    ASwordWaveProjectile* Wave = GetWorld()->SpawnActor<ASwordWaveProjectile>(
        SwordWaveClass, SpawnLoc, SpawnRot);

    if (Wave)
    {
        Wave->InitSwordWave(Avatar, SkillDamageClass, SpawnSpeed); 
    }
}
