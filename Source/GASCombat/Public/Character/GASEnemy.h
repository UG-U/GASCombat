// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCharacterBase.h"
#include"Widget/WidgetController/GASOverlayWidgetController.h"
#include "GASEnemy.generated.h"


class UWidgetComponent;
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

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChange;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UWidgetComponent>HealthBar;

};
