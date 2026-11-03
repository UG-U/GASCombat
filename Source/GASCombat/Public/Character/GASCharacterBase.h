// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"AbilitySystemInterface.h"
#include"Interface/CombatInterface.h"
#include <GenericTeamAgentInterface.h>
#include "GASCharacterBase.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UStaticMeshComponent;

UCLASS()
class GASCOMBAT_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase();

	virtual void Tick(float DeltaTime) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;

	virtual UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	void InitialAttributes();

	void AddCharacterAbilities();

	UPROPERTY(EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect>VitalAttributes;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect>PrimaryAttributes;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect>SecondaryAttributes;

protected:
	virtual void BeginPlay() override;

	//阵营判定，通过蓝图中设置的ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Team")
	FGenericTeamId TeamId;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent>WeaponSocket;

	UPROPERTY(EditAnywhere,Category="GameplayAbility")
	TArray<TSubclassOf<UGameplayAbility>>AbilityLists;
public:	



};
