// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASEnemy.h"
#include <AbilitySystem/GASAbilitySystemComponent.h>
#include <AbilitySystem/GASAttributeSet.h>
#include"Components/WidgetComponent.h"
#include"Widget/GASUserWidget.h"
AGASEnemy::AGASEnemy()
{
	//初始化ASC以及设置复制模式
	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSet = CreateDefaultSubobject<UGASAttributeSet>(TEXT("AttributeSet"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
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

	InitialAttributes();

	if (UGASUserWidget* GASUserWidget = Cast<UGASUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		GASUserWidget->SetWidgetController(this);
	}
	if (UGASAttributeSet* GASAS = Cast<UGASAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GASAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChange.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GASAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChange.Broadcast(Data.NewValue);
			}
		);

		OnHealthChange.Broadcast(GASAS->GetHealth());
		OnMaxHealthChange.Broadcast(GASAS->GetMaxHealth());
	}
}
