// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"GameplayEffectTypes.h"
#include "GasGameplayEffectActor.generated.h"

class UGameplayEffect;

struct FActiveGameplayEffectHandle;
UENUM()
enum class EEffectApplyPolicy:uint8 
{
	ApplyOnBeginOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM()
enum class EEffectRemovePolicy :uint8 
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class GASCOMBAT_API AGasGameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGasGameplayEffectActor();

	UFUNCTION(BlueprintCallable,Category="GameplayEffect")
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect>GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor*TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor*TargetActor);
protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameplayEffect")
	TSubclassOf<UGameplayEffect>GasGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	EEffectApplyPolicy InstantApplyPolicy=EEffectApplyPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	EEffectApplyPolicy DurationApplyPolicy=EEffectApplyPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	EEffectApplyPolicy InfiniteApplyPolicy=EEffectApplyPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	EEffectRemovePolicy InfiniteRemovePolicy = EEffectRemovePolicy::DoNotRemove;

	TMap<FActiveGameplayEffectHandle, TWeakObjectPtr<AActor>>ActiveEffectHandles;

	
	UPROPERTY(EditDefaultsOnly,Category="GameplayEffect")
	float ActorLevel = 1.f;
public:	


};
