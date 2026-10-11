// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCharacterBase.h"
#include "GASCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API AGASCharacter : public AGASCharacterBase
{
	GENERATED_BODY()
	
public:
	AGASCharacter();

	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual int32 GetPlayerLevel()override;

private:

	void InitAbilityActorInfo();

	void InitHUDOverlay();
};
