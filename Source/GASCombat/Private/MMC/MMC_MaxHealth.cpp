// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC/MMC_MaxHealth.h"
#include <AbilitySystem/GASAttributeSet.h>
#include <Interface/CombatInterface.h>

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigarDef.AttributeToCapture = UGASAttributeSet::GetVigarAttribute();//属性捕获
	VigarDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;//属性来源
	VigarDef.bSnapshot = false;//不进行快照，让捕获值随属性变化而实时变化
	RelevantAttributesToCapture.Add(VigarDef);//将此捕获定义注册到 RelevantAttributesToCapture 列表中
	//这样 GE 应用前，系统会自动抓取该属性的值并缓存到 Spec 中
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Spec.CapturedSourceTags 包含了 GE 应用时来源方（施法者）身上所有 GameplayTag 的集合。.GetAggregatedTags() 返回一个指针，指向一个经过聚合（考虑 Tag 继承关系、GE 临时授予的 Tag 等）的最终 Tag 容器。
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;//创建一个聚合器评估参数结构体，这是一个轻量级的配置包裹，用来告诉 GAS：“我要用哪些标签信息作为条件去读取属性值”。
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Vigar = 0.f;//不是 UE 里设置的初始值，而是一个空箱子，即将被填入运行时真正的智力值。声明一个运行时变量当空箱子，0 是兜底值，拿的是经过 GE 叠加后的最终当前值
	GetCapturedAttributeMagnitude(VigarDef, Spec, EvaluateParameters, Vigar);//获取捕获的属性值
	Vigar = FMath::Max<float>(Vigar, 0.f);//确保 Vigor 不为负数（防御性编程）

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());//通过上下文获取效果来源对象，并转换为 ICombatInterface 以调用 GetPlayerLevel()

	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	return 80.f + 1.5f * (Vigar + 2) + 5.f * PlayerLevel;
}


