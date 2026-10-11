// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/GASInputConfig.h"

const UInputAction* UGASInputConfig::FindAbilityInputActionForTag(FGameplayTag& Tag, bool bLogNotFound) const
{
	for (auto& Action : AbilityInputActions)
	{
		if (Action.InputTag == Tag && Action.InputAction)
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found AbilityInputAction"));
	}
	return nullptr;
}
