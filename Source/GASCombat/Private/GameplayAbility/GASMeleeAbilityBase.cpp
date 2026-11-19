// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GASMeleeAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GASGameplayTags.h"
#include "MotionWarpingComponent.h"
#include "GenericTeamAgentInterface.h"

void UGASMeleeAbilityBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 结束吸附，避免旧的 WarpTarget 影响后续蒙太奇
	ClearWarpTarget();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

TSubclassOf<UGameplayEffect> UGASMeleeAbilityBase::GetMeleeDamageEffect() const
{
	return nullptr;
}

float UGASMeleeAbilityBase::GetMeleeDamageMultiplier() const
{
	return 1.f;
}

UMotionWarpingComponent* UGASMeleeAbilityBase::GetWarpComponent() const
{
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		return Avatar->FindComponentByClass<UMotionWarpingComponent>();
	}
	return nullptr;
}

void UGASMeleeAbilityBase::ClearWarpTarget()
{
	if (UMotionWarpingComponent* WarpComp = GetWarpComponent())
	{
		WarpComp->RemoveWarpTarget(WarpTargetName);
	}
}

void UGASMeleeAbilityBase::TryWarpToNearestEnemy(float Radius)
{
	ClearWarpTarget();

	if (!bUseMotionWarping)
	{
		return;
	}

	const float SearchRadius = Radius > 0.f ? Radius : FindEnemyRadius;
	if (AActor* TargetEnemy = FindClosestEnemyInRange(SearchRadius))
	{
		UpdateWarpTarget(TargetEnemy);
	}
}

void UGASMeleeAbilityBase::UpdateWarpTarget(AActor* TargetEnemy)
{
	UMotionWarpingComponent* WarpComp = GetWarpComponent();
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!WarpComp || !Avatar || !TargetEnemy)
	{
		return;
	}

	const FVector AvatarLoc = Avatar->GetActorLocation();
	const FVector TargetLoc = TargetEnemy->GetActorLocation();
	const FVector ToTarget = TargetLoc - AvatarLoc;
	const float Dist = ToTarget.Size();
	if (Dist <= MinGap)
	{
		return;
	}

	const FVector Direction = ToTarget / Dist;// 归一化方向向量

	// 只吸附前方锥形范围内的目标，背后目标直接放弃
	const float Dot = FVector::DotProduct(Avatar->GetActorForwardVector(), Direction);//通过 DotProduct（点乘）计算角色当前朝向（ForwardVector）和目标方向（Direction）的夹角。
	if (Dot < FMath::Cos(FMath::DegreesToRadians(MaxWarpAngle)))
	{
		return;
	}

	// 落点永不越过角色：距离不足时按 Dist - MinGap 收缩
	FVector Desired = TargetLoc - Direction * FMath::Min(WarpDistanceOffset, Dist - MinGap);
	Desired.Z = AvatarLoc.Z;

	// 防穿墙：撞到障碍就停在障碍前方
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);
	Params.AddIgnoredActor(TargetEnemy);
	if (GetWorld()->LineTraceSingleByChannel(Hit, AvatarLoc, Desired, ECC_Visibility, Params))
	{
		Desired = Hit.ImpactPoint - Direction * MinGap;
	}

	// 兜底：无论如何都不允许把角色拽到身后
	if (FVector::DotProduct(Desired - AvatarLoc, Direction) <= 0.f)
	{
		return;
	}

	WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(
		WarpTargetName, Desired, FRotator(0.f, Direction.Rotation().Yaw, 0.f));
}

TArray<FHitResult> UGASMeleeAbilityBase::PerformMeleeTrace() const
{
	TArray<FHitResult> HitResults;

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return HitResults;
	}

	const FVector Start = Avatar->GetActorLocation();
	const FVector End = Start + Avatar->GetActorForwardVector() * TraceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params);

	return HitResults;
}

bool UGASMeleeAbilityBase::IsValidMeleeTarget(AActor* Target, UAbilitySystemComponent*& OutTargetASC) const
{
	OutTargetASC = nullptr;

	if (!Target)
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!TargetASC)
	{
		return false;
	}

	if (TargetASC->HasMatchingGameplayTag(GASGameplayTags::State_Invincible))
	{
		return false;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return false;
	}

	IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Avatar);
	IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(Target);
	if (OwnerTeam && TargetTeam && OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
	{
		return false;
	}

	OutTargetASC = TargetASC;
	return true;
}

int32 UGASMeleeAbilityBase::ApplyMeleeDamage(const TArray<FHitResult>& HitResults)
{
	if (HitResults.Num() == 0)
	{
		return 0;
	}

	// 服务器权威：伤害只在服务器结算
	if (!GetOwningActorFromActorInfo() || !GetOwningActorFromActorInfo()->HasAuthority())
	{
		return 0;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	const TSubclassOf<UGameplayEffect> DamageEffect = GetMeleeDamageEffect();
	if (!SourceASC || !DamageEffect)
	{
		return 0;
	}

	TSet<AActor*> UniqueTargets;
	int32 AppliedCount = 0;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* Target = Hit.GetActor();
		if (!Target || UniqueTargets.Contains(Target))
		{
			continue;
		}
		UniqueTargets.Add(Target);

		UAbilitySystemComponent* TargetASC = nullptr;
		if (!IsValidMeleeTarget(Target, TargetASC))
		{
			continue;
		}

		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
		ContextHandle.AddHitResult(Hit);

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), ContextHandle);
		if (!SpecHandle.IsValid())
		{
			continue;
		}

		SpecHandle.Data->SetSetByCallerMagnitude(
			GASGameplayTags::Data_Damage_Multiplier, GetMeleeDamageMultiplier());

		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		++AppliedCount;
	}

	return AppliedCount;
}

AActor* UGASMeleeAbilityBase::FindClosestEnemyInRange(float Range) const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return nullptr;
	}

	const FVector StartLoc = Avatar->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Avatar);

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		StartLoc,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Range),
		QueryParams);

	AActor* BestTarget = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* TargetActor = Result.GetActor();
		if (!TargetActor)
		{
			continue;
		}

		IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Avatar);
		IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(TargetActor);
		if (OwnerTeam && TargetTeam && OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
		{
			continue;
		}

		if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(StartLoc, TargetActor->GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			BestTarget = TargetActor;
		}
	}

	return BestTarget;
}
