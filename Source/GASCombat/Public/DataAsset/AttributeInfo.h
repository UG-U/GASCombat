// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include"GameplayTags.h"
#include "AttributeInfo.generated.h"


struct FGameplayTag;
USTRUCT(BlueprintType)
struct FGASAttributeInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};



/**
 * 
 */
UCLASS()
class GASCOMBAT_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:

	FGASAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false)const;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FGASAttributeInfo>AttributeInfos;
};
