// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASGameplayAbility.generated.h"


struct FGameplayTag;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag InputTag;
};
