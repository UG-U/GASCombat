// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffect/GasGameplayEffectActor.h"
#include <GameplayEffect.h>
#include <AbilitySystemBlueprintLibrary.h>
#include"AbilitySystemComponent.h"
AGasGameplayEffectActor::AGasGameplayEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AGasGameplayEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	check(GameplayEffectClass);
	//获取TargetActor的ASC组件
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;
	//创建效果上下文，用来携带效果的元数据
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	//用配方(GE类)、等级、上下文，打包一份完整的效果规格
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	//应用效果并获取活跃效果句柄
	const FActiveGameplayEffectHandle ActiveEffectHandle= TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	//判断是否为无限效果类型
	const bool bIsInfiniteEffect = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//是无限效果且配置为离开区域取消时，添加
	if (bIsInfiniteEffect && InfiniteRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetActor);
	}
}
void AGasGameplayEffectActor::OnBeginOverlap(AActor*TargetActor)
{
	if (InstantApplyPolicy == EEffectApplyPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
	if (DurationApplyPolicy == EEffectApplyPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
	if (InfiniteApplyPolicy == EEffectApplyPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
}
void AGasGameplayEffectActor::OnEndOverlap(AActor*TargetActor)
{
	if (InstantApplyPolicy == EEffectApplyPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
	if (DurationApplyPolicy == EEffectApplyPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
	if (InfiniteApplyPolicy == EEffectApplyPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, GasGameplayEffectClass);
	}
	//离开重叠区域时，需要移除的无限效果
	if (InfiniteRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		TArray<FActiveGameplayEffectHandle>HandlesToRemove;//收集已经移除的句柄
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value.IsValid() && HandlePair.Value.Get() == TargetActor)
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
		}
		//删除已移除的句柄
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.Remove(Handle);
		}
	}
}


