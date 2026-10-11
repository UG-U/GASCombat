// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacter.h"
#include <Player/GASPlayerState.h>
#include"AbilitySystemComponent.h"
#include <Player/GASPlayerController.h>
#include"UI/HUD/GASHUD.h"
#include <AbilitySystem/GASAbilitySystemComponent.h>
AGASCharacter::AGASCharacter()
{

}

void AGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	InitHUDOverlay();
	InitialAttributes();

	AddCharacterAbilities();
}

void AGASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	InitHUDOverlay();
}

int32 AGASCharacter::GetPlayerLevel()
{
	AGASPlayerState* GASPlayerState = GetPlayerState<AGASPlayerState>();
	if (!GASPlayerState) return 0;

	return GASPlayerState->GetPlayerLevel();
}

void AGASCharacter::InitAbilityActorInfo()
{
	AGASPlayerState* PS = GetPlayerState<AGASPlayerState>();
	if (!PS)return;
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)return;
	ASC->InitAbilityActorInfo(PS, this);//初始化AbilityActorInfo，传入PlayerState(拥有者）和Character（化身）
	Cast<UGASAbilitySystemComponent>(ASC)->AbilityActorInfoSet();
}

void AGASCharacter::InitHUDOverlay()
{
	AGASPlayerState* PS = GetPlayerState<AGASPlayerState>();
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	AGASPlayerController* PC = Cast<AGASPlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitHUDOverlay: PlayerController is null, skipping."));
		return;
	}
	AGASHUD* GASHUD = Cast<AGASHUD>(PC->GetHUD());
	if (!GASHUD) return;
	GASHUD->InitOverlay(PS, PC, ASC, AS);

}
