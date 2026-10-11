// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include"GameplayTagContainer.h"
#include "GASInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FGASInputAction {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditAnywhere)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//通过 Tag 反向查找 InputAction（可用于 UI 显示、按键提示等）
	const UInputAction* FindAbilityInputActionForTag(FGameplayTag& Tag, bool bLogNotFound = false)const;

	UPROPERTY(EditAnywhere)
	TArray<FGASInputAction> AbilityInputActions;
};
