// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include"AbilitySystemComponent.h"
#include "GASAttributeSet.generated.h"



class UAbilitySystemComponent;
class ACharacter;
/**
 * Macros to define attribute accessors for a given class and property name.
 * These macros generate getter, setter, and initializer functions for the specified attribute.
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;


USTRUCT(BlueprintType)
struct FEffectProperties {
	GENERATED_BODY()

	FEffectProperties() {};

	FGameplayEffectContextHandle EffectContextHandle;
	UPROPERTY()
	UAbilitySystemComponent*SourceASC = nullptr;
	UPROPERTY()
	AController*SourceController = nullptr;
	UPROPERTY()
	ACharacter*SourceCharacter = nullptr;
	UPROPERTY()
	AActor*SourceAvatarActor = nullptr;
	UPROPERTY()
	UAbilitySystemComponent*TargetASC = nullptr;
	UPROPERTY()
	AController*TargetController = nullptr;
	UPROPERTY()
	ACharacter*TargetCharacter = nullptr;
	UPROPERTY()
	AActor*TargetAvatarActor = nullptr;
};


/**
 * 
 */
UCLASS()
class GASCOMBAT_API UGASAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGASAttributeSet();
	void SetFEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props)const;

	//复制属性注册函数，只在服务器调用一次，用于构建复制列表
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	//C++运行时，通过Tag高效、安全的获取属性数值
	TMap<FGameplayTag,TStaticFuncPtr<FGameplayAttribute()>>TagsToAttribute;
	/*
	* Vital Attributes
	*/
	UPROPERTY( BlueprintReadOnly, ReplicatedUsing=OnRep_Health,Category="Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Health);//自动为该属性生成静态的 FGameplayAttribute 访问器,可直接使用Set等函数
	
	/*当新的生命值到达网络时被调用，用于处理UI更新等纯表现逻辑*/
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana,Category="Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Mana);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	/*
	* Primary Attributes
	*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Strength);

	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Intelligence);

	UFUNCTION()
	virtual void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resistance, Category = "Primary Attributes")
	FGameplayAttributeData Resistance;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Resistance);

	UFUNCTION()
	virtual void OnRep_Resistance(const FGameplayAttributeData& OldResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigar, Category = "Primary Attributes")
	FGameplayAttributeData Vigar;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Vigar);

	UFUNCTION()
	virtual void OnRep_Vigar(const FGameplayAttributeData& OldVigar);

	/*
	* Secondary Attributes
	*/

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxHealth,Category="Primary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Primary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
};
