// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacterBase.h"
#include <Player/GASPlayerState.h>
#include <GameplayEffect.h>
#include"AbilitySystemComponent.h"
#include"AbilitySystem/GASAbilitySystemComponent.h"
AGASCharacterBase::AGASCharacterBase()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	WeaponSocket = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponSocket"));
	WeaponSocket->SetupAttachment(GetMesh(), FName("WeaponSocket"));
}


UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
	if(AbilitySystemComponent) return AbilitySystemComponent;
	if (AGASPlayerState* PS = GetPlayerState<AGASPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AGASCharacterBase::InitialAttributes()
{
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle PrimaryEffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(PrimaryAttributes, 1.f, EffectContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*PrimaryEffectSpecHandle.Data.Get());

	FGameplayEffectSpecHandle SecondaryEffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(SecondaryAttributes, 1.f, EffectContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SecondaryEffectSpecHandle.Data.Get());
	FGameplayEffectSpecHandle VitalEffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(VitalAttributes, 1.f, EffectContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*VitalEffectSpecHandle.Data.Get());

}

void AGASCharacterBase::AddCharacterAbilities()
{
	
	UGASAbilitySystemComponent* GASASC = Cast<UGASAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!HasAuthority() || !GASASC) return;
	GASASC->AddCharacterAbility(AbilityLists);
}


void AGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGASCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


