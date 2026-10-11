// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"NativeGameplayTags.h"



namespace GASGameplayTags {
	/*
	* Primary Attributes
	*/
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resistance);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigar);
	/*
	* Secondary Attributes
	*/
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana);

	/*
	* Ability InputTag
	*/
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LM);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RM);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_1);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_2);
	GASCOMBAT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_3);
}





