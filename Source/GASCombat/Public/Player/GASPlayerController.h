// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <InputActionValue.h>
#include "GASPlayerController.generated.h"



class UInputMappingContext;
class UInputAction;
class UGASInputConfig;
class UGASAbilitySystemComponent;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API AGASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGASPlayerController();
	virtual void SetupInputComponent() override;
protected:
	virtual void BeginPlay() override;
	/*
	* Input Mapping Context and Input Action for the Player Controller
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> GASInputContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction>MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction>LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction>JumpAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UGASInputConfig>DAInputConfig;

	/*
	* Input Action Functions for the Player Controller
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void StopJumping();

	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FGameplayTag InputTag);

	UGASAbilitySystemComponent* GetGASASC();

private:
	UPROPERTY()
	TObjectPtr<UGASAbilitySystemComponent>GASASC;
};
