// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "SwordWaveProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;
class UNiagaraComponent;

UCLASS()
class GASCOMBAT_API ASwordWaveProjectile : public AActor
{
    GENERATED_BODY()
public:
    ASwordWaveProjectile();

    // 初始化剑气（由技能调用传入伤害GE和施法者）
    void InitSwordWave(AActor* InInstigator, TSubclassOf<UGameplayEffect> InDamageGE, float InSpeed);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(VisibleAnywhere) 
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(VisibleAnywhere) 
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(VisibleAnywhere) 
    TObjectPtr<UNiagaraComponent> WaveEffect; // 剑气特效

    UPROPERTY(EditAnywhere,Category="Damage")
    TSubclassOf<UGameplayEffect> DamageEffectClass;
private:
    

    TWeakObjectPtr<AActor> SourceActor;

    TSet<AActor*> HitActors; // 防止同一个敌人受到多次伤害
};
