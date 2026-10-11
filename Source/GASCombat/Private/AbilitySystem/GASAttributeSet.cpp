// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GASAttributeSet.h"
#include <Net/UnrealNetwork.h>
#include"GameplayEffectExtension.h"
#include"AbilitySystemBlueprintLibrary.h"
#include"GameFramework/Character.h"
#include"GASGameplayTags.h"
UGASAttributeSet::UGASAttributeSet()
{

	//拿到标签后，快速获取FGameplayAttribute（）
	/*
	* Primary Attributes
	*/
	TagsToAttribute.Add(GASGameplayTags::Attributes_Primary_Strength,GetStrengthAttribute);
	TagsToAttribute.Add(GASGameplayTags::Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttribute.Add(GASGameplayTags::Attributes_Primary_Resistance, GetResistanceAttribute);
	TagsToAttribute.Add(GASGameplayTags::Attributes_Primary_Vigar, GetVigarAttribute);
	/*
	* Secondary Attributes
	*/
	TagsToAttribute.Add(GASGameplayTags::Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttribute.Add(GASGameplayTags::Attributes_Secondary_MaxMana, GetMaxManaAttribute);
}

void UGASAttributeSet::SetFEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (Props.SourceASC && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	//Target
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.SourceAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.SourceAvatarActor);
	}
	
}

void UGASAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*
	* Vital Attributes
	*/
	//为生命值添加复制功能，条件为none，通知模式为always
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	/*
	* Primary Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, Vigar, COND_None, REPNOTIFY_Always);

	/*
	* Secondary Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UGASAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute,NewValue);
	if (GetHealthAttribute() == Attribute)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (GetManaAttribute() == Attribute)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UGASAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetFEffectProperties(Data, Props);
	//限制最终的属性值范围
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
}

void UGASAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	//使用默认的属性复制通知处理程序来处理Health属性的复制
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Health, OldHealth);
}

void UGASAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Mana, OldMana);
}

void UGASAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Strength, OldStrength);
}

void UGASAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Intelligence, OldIntelligence);
}

void UGASAttributeSet::OnRep_Resistance(const FGameplayAttributeData& OldResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Resistance, OldResistance);
}

void UGASAttributeSet::OnRep_Vigar(const FGameplayAttributeData& OldVigar)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, Vigar, OldVigar);
}

void UGASAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, MaxHealth, OldMaxHealth);
}

void UGASAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASAttributeSet, MaxMana, OldMaxMana);
}
