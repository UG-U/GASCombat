// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include <DataAsset/GASInputConfig.h>
#include "GASInputComponent.generated.h"


/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template <class UserClass,typename PressedFuncPtr,typename ReleasedFuncPtr,typename HeldFuncPtr>
	void BindAbilityAction(const UGASInputConfig* InputConfig, UserClass* Object, PressedFuncPtr PressedFunc, ReleasedFuncPtr ReleasedFunc, HeldFuncPtr HeldFunc);

};

template<class UserClass, typename PressedFuncPtr, typename ReleasedFuncPtr, typename HeldFuncPtr>
inline void UGASInputComponent::BindAbilityAction(const UGASInputConfig* InputConfig, UserClass* Object, PressedFuncPtr PressedFunc, ReleasedFuncPtr ReleasedFunc, HeldFuncPtr HeldFunc) 
{
	check(InputConfig);
	for (auto& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
		
	}
}
