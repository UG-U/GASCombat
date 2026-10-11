// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GASWidgetController.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;


USTRUCT()
struct FWidgetControllerParams {
	GENERATED_BODY()
	FWidgetControllerParams() {};

	FWidgetControllerParams(APlayerState* PS, APlayerController* PC, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		:PlayerState(PS), PlayerController(PC), AbilitySystemComponent(ASC), AttributeSet(AS) {};

	UPROPERTY()
	TObjectPtr<APlayerState>PlayerState = nullptr;
	UPROPERTY()
	TObjectPtr<APlayerController>PlayerController = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UAttributeSet>AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class GASCOMBAT_API UGASWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValue();
	UFUNCTION(BlueprintCallable)
	virtual void BindCallbacksToDependencies();

protected:
	UPROPERTY()
	TObjectPtr<APlayerState>PlayerState;
	UPROPERTY()
	TObjectPtr<APlayerController>PlayerController;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet>AttributeSet;
};
