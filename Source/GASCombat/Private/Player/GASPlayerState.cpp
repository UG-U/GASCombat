// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GASPlayerState.h"
#include <AbilitySystem/GASAbilitySystemComponent.h>
#include <AbilitySystem/GASAttributeSet.h>
#include <Net/UnrealNetwork.h>

AGASPlayerState::AGASPlayerState()
{
	NetUpdateFrequency = 100.f;

	//初始化AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);//设置AbilitySystemComponent为可复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);//设置AbilitySystemComponent的复制模式为混合模式
	
	AttributeSet = CreateDefaultSubobject<UGASAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASPlayerState::GetAbilitySystemComponent()const
{
    return AbilitySystemComponent;
}

void AGASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASPlayerState, Level);
}



void AGASPlayerState::OnRep_Level(int32 OldLevel)
{
}

