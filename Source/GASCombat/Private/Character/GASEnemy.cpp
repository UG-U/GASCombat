// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASEnemy.h"
#include <AbilitySystem/GASAbilitySystemComponent.h>
#include <AbilitySystem/GASAttributeSet.h>

AGASEnemy::AGASEnemy()
{
	//初始化ASC以及设置复制模式
	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSet = CreateDefaultSubobject<UGASAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int32 AGASEnemy::GetPlayerLevel()
{
	return Level;
}

void AGASEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (!AbilitySystemComponent)return;
	AbilitySystemComponent->InitAbilityActorInfo(this, this);//初始化AbilityActorInfo，拥有者和化身都是自己
	Cast<UGASAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
