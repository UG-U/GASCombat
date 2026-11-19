// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillDataAsset.generated.h"


class UAnimMontage;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FSkillFormData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly) 
    TObjectPtr<UAnimMontage> SkillMontage; // 该形态下的动画

    UPROPERTY(EditDefaultsOnly) 
    TSubclassOf<UGameplayEffect> DamageGE; // 该形态下的伤害（可能双刀伤害高）

    UPROPERTY(EditDefaultsOnly) 
    TSubclassOf<UGameplayEffect> CostGE;   // 该形态下的消耗
};

/**
 * 
 */
UCLASS()
class GASCOMBAT_API USkillDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditDefaultsOnly) 
    FSkillFormData DualFormData;     // 双刀形态数据

    UPROPERTY(EditDefaultsOnly) 
    FSkillFormData CombinedFormData; // 单刀形态数据
};
