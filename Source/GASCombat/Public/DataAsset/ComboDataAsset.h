// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboDataAsset.generated.h"



USTRUCT(BlueprintType)
struct FComboStep 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	FName SectionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	float DamageMultiplier = 1.0f;
};


/**
 * 
 */
UCLASS()
class GASCOMBAT_API UComboDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TArray<FComboStep> ComboSteps;

};
