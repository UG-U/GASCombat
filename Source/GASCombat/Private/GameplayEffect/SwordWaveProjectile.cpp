// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEffect/SwordWaveProjectile.h"
#include"Components/SphereComponent.h"
#include"GameFramework/ProjectileMovementComponent.h"
#include"NiagaraComponent.h"
#include <AbilitySystemGlobals.h>
#include"AbilitySystemComponent.h"
ASwordWaveProjectile::ASwordWaveProjectile()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
    
	WaveEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WaveEffect"));
	WaveEffect->SetupAttachment(RootComponent);
}

void ASwordWaveProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASwordWaveProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* Target = OtherActor;
    if (!Target || Target == SourceActor || HitActors.Contains(Target)) return;

    // 1. 阵营判定（复用你之前的接口）
    // 2. 网络权限判定：只有服务器才能造成伤害
    if (!HasAuthority()) return;

    UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor.Get());
    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);

    if (SourceASC && TargetASC)
    {
        HitActors.Add(Target); // 记录已命中

        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        Context.AddInstigator(SourceActor.Get(), this);
        Context.AddHitResult(SweepResult);

        FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
        SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

        // 命中后可以触发 GameplayCue 播放爆炸特效，并销毁自身
        // Destroy(); (视你的穿透需求而定)
    }
}


void ASwordWaveProjectile::InitSwordWave(AActor* InInstigator, TSubclassOf<UGameplayEffect> InDamageGE, float InSpeed)
{
    SourceActor = InInstigator;
    DamageEffectClass = InDamageGE;
    ProjectileMovement->InitialSpeed = InSpeed;
    ProjectileMovement->MaxSpeed = InSpeed;
}
