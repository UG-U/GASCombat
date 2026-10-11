// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetController/GASWidgetController.h"
#include "MenuAttributeWidgetController.generated.h"


class UAttributeInfo;
struct FGASAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FGASAttributeInfo&, Info);

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UMenuAttributeWidgetController : public UGASWidgetController
{
	GENERATED_BODY()
	

public:
	virtual void BroadcastInitialValue()override;
	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly);
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAttributeInfo>DAAttributeInfo;
};
