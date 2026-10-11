// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetController/GASWidgetController.h"
#include <GameplayEffectTypes.h>
#include "GASOverlayWidgetController.generated.h"


class UGASUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetRow :public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGASUserWidget>MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

//属性变化委托声明
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewHealth);

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class GASCOMBAT_API UGASOverlayWidgetController : public UGASWidgetController
{
	GENERATED_BODY()
	
public:

	virtual void BroadcastInitialValue()override;
	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;


	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;


	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;


	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;


	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable>MessageWidgetDataTable;

	template<typename T>
	T* GetDataTableRowByTag(TObjectPtr<UDataTable> DataTable, const FGameplayTag& Tag);



};
template<typename T>
T* UGASOverlayWidgetController::GetDataTableRowByTag(TObjectPtr<UDataTable> DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
