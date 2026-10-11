// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCharacterBase.h"
#include "GASEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API AGASEnemy : public AGASCharacterBase
{
	GENERATED_BODY()
	
public:
	AGASEnemy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual int32 GetPlayerLevel()override;

	UPROPERTY(EditAnywhere,Category="PlayerLevel")
	int32 Level;

protected:
	virtual void BeginPlay() override;

};
