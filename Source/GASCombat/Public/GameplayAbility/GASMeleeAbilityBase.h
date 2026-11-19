// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/GASGameplayAbility.h"
#include "GASMeleeAbilityBase.generated.h"

class UMotionWarpingComponent;
class UAbilitySystemComponent;
class UGameplayEffect;

/**
 * 近战能力基类。
 * 提供：吸附目标计算、近战球形扫描、伤害结算。
 * 连招 / 形态切换等玩法逻辑留在子类。
 */
UCLASS(Abstract)
class GASCOMBAT_API UGASMeleeAbilityBase : public UGASGameplayAbility
{
	GENERATED_BODY()

public:
	/** 清掉旧目标 -> 找最近敌人 -> 计算并写入 Warp 目标。bUseMotionWarping 为 false 时只清理。 */
	void TryWarpToNearestEnemy(float Radius);
\

	UFUNCTION(BlueprintCallable, Category = "Combat|Warp")
	void ClearWarpTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* FindClosestEnemyInRange(float Range) const;

protected:
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** 子类覆写：本次命中使用的伤害 GE；返回空表示只做检测不结算伤害 */
	virtual TSubclassOf<UGameplayEffect> GetMeleeDamageEffect() const;
	/** 子类覆写：伤害倍率，写入 SetByCaller(Data.Damage.Multiplier) */
	virtual float GetMeleeDamageMultiplier() const;

	TArray<FHitResult> PerformMeleeTrace() const;

	/** 目标合法性：有 ASC、非无敌、不同阵营 */
	bool IsValidMeleeTarget(AActor* Target, UAbilitySystemComponent*& OutTargetASC) const;

	/** 对扫描结果结算伤害（服务器权威、去重），返回实际结算的目标数 */
	int32 ApplyMeleeDamage(const TArray<FHitResult>& HitResults);

	UMotionWarpingComponent* GetWarpComponent() const;
	void UpdateWarpTarget(AActor* TargetEnemy);

	/* ---------------- 可调参数 ---------------- */

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	bool bUseMotionWarping = true;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	FName WarpTargetName = FName("FaceWarping");

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	float WarpDistanceOffset = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	float MinGap = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	float MaxWarpAngle = 75.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Warp")
	float FindEnemyRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
	float TraceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
	float TraceDistance = 150.0f;
};
