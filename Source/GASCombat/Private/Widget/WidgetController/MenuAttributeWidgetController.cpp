// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetController/MenuAttributeWidgetController.h"
#include"AbilitySystem/GASAttributeSet.h"
#include"DataAsset/AttributeInfo.h"
void UMenuAttributeWidgetController::BroadcastInitialValue()
{
	UGASAttributeSet* AS = Cast<UGASAttributeSet>(AttributeSet);
	check(DAAttributeInfo);
	for (auto& Pair : AS->TagsToAttribute)
	{
		FGASAttributeInfo Info = DAAttributeInfo->FindAttributeInfoForTag(Pair.Key);
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UMenuAttributeWidgetController::BindCallbacksToDependencies()
{
	UGASAttributeSet* AS = Cast<UGASAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair,AS](const FOnAttributeChangeData&Data)
			{
				FGASAttributeInfo Info = DAAttributeInfo->FindAttributeInfoForTag(Pair.Key);
				Info.AttributeValue = Pair.Value().GetNumericValue(AS);
				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}
}
