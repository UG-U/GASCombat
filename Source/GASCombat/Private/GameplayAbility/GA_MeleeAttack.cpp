// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/GA_MeleeAttack.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include"AbilitySystemComponent.h"
#include"GASGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <Abilities/Tasks/AbilityTask_WaitInputPress.h>
#include <AbilitySystemGlobals.h>
#include "GameplayEffect.h"
#include "AbilitySystem/GASAttributeSet.h"
#include <GenericTeamAgentInterface.h>
#include <MotionWarpingComponent.h>
#include"DataAsset/ComboDataAsset.h"


UGA_MeleeAttack::UGA_MeleeAttack()
{
    // 可选：设置技能标签、冷却等
}

void UGA_MeleeAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 1. 提交消耗和冷却
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }


    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    // 寻找最近的敌人（可以用球体扫描找最近的敌对目标）
    AActor* TargetEnemy = FindClosestEnemyInRange(FindEnemyRadius);
    if (TargetEnemy)
    {
        UMotionWarpingComponent* WarpComp = Avatar->FindComponentByClass<UMotionWarpingComponent>();
  
        if (WarpComp)
        {
            FVector TargetLoc = TargetEnemy->GetActorLocation();
            FVector AvatarLoc = Avatar->GetActorLocation();
            FVector Direction = (TargetLoc - AvatarLoc).GetSafeNormal();

            // 理想落脚点
            FVector DesiredWarpLocation = TargetLoc - (Direction * WarpDistanceOffset);
            FRotator DesiredWarpRotation = Direction.Rotation();

            // ================= 关键：防穿墙检测 =================
            FHitResult HitResult;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(Avatar);
            Params.AddIgnoredActor(TargetEnemy); // 忽略目标敌人本身

            // 从自己向目标点发射一条射线，看看中间有没有墙
            bool bHit = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                AvatarLoc,
                DesiredWarpLocation,
                ECC_Visibility, // 假设墙的通道是 Visibility
                Params
            );

            if (bHit)
            {
                // 如果撞到墙了，把落脚点设在撞墙的位置，并往后退一点点（比如 20 厘米）
                DesiredWarpLocation = HitResult.ImpactPoint - (Direction * 20.0f);
            }
            // ====================================================

            WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(
                FName("FaceWarping"),
                DesiredWarpLocation,
                DesiredWarpRotation
            );
        }
    }


    // 2. 重置连击状态
    ComboIndex = 0;
    bInComboWindow = false;
    bInputBuffered = false;


	CurrentComboConfig = GetComboConfigByForm();
	//检查连击配置是否有效
    if (!CurrentComboConfig || CurrentComboConfig->ComboSteps.Num() == 0)
    {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
	//获取当前连击的第一段蒙太奇和 Section
	const FComboStep& FirstStep = CurrentComboConfig->ComboSteps[0];
	CurrentMontage = FirstStep.Montage;


    // 5. 播放蒙太奇（从第一段开始）Task标准用法：创建→绑定→启动
    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, CurrentMontage, 1.0f, FirstStep.SectionName);

    MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
    //这里被取消和被中断，之后的回调函数逻辑一样，故用同一个
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
    MontageTask->ReadyForActivation();

    // 6. 监听连击窗口开启事件
    UAbilityTask_WaitGameplayEvent* WaitComboOpen =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, GASGameplayTags::Event_Combo_Window_Open, nullptr, false, true);
    WaitComboOpen->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnComboWindowOpened);
    WaitComboOpen->ReadyForActivation();

    // 7. 监听连击窗口关闭事件
    UAbilityTask_WaitGameplayEvent* WaitComboClose =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, GASGameplayTags::Event_Combo_Window_Close, nullptr, false, true);
    WaitComboClose->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnComboWindowClosed);
    WaitComboClose->ReadyForActivation();

    // 8. 监听伤害判定事件
    UAbilityTask_WaitGameplayEvent* WaitDamage =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this, FGameplayTag::RequestGameplayTag("Event.Combat.DamageFrame"), nullptr, false, true);
    WaitDamage->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnDamageFrame);
    WaitDamage->ReadyForActivation();
}

void UGA_MeleeAttack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    //C++继承通用原则：先清理子类，最后清理父类
    // 移除攻击状态标签

    // 结束蒙太奇任务
    if (MontageTask && IsValid(MontageTask))
    {
        MontageTask->EndTask();
        MontageTask = nullptr;
    }

    // 结束等待输入任务
    if (WaitInputTask && IsValid(WaitInputTask))
    {
        WaitInputTask->EndTask();
        WaitInputTask = nullptr;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_MeleeAttack::OnMontageInterrupted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MeleeAttack::OnComboWindowOpened(FGameplayEventData Payload)
{
    bInComboWindow = true;

    if (bInputBuffered)
    {
        bInputBuffered = false;
        AdvanceCombo();
    }
    else
    {
        // 等待玩家在窗口内再次按下攻击键
        WaitInputTask = UAbilityTask_WaitInputPress::WaitInputPress(this, true);
        WaitInputTask->OnPress.AddDynamic(this, &UGA_MeleeAttack::OnAttackInputBuffered);
        WaitInputTask->ReadyForActivation();
    }
}

void UGA_MeleeAttack::OnComboWindowClosed(FGameplayEventData Payload)
{
    bInComboWindow = false;
    bInputBuffered = false;

    if (WaitInputTask && IsValid(WaitInputTask))
    {
        WaitInputTask->EndTask();
        WaitInputTask = nullptr;
    }
}

void UGA_MeleeAttack::OnAttackInputBuffered(float TimeWaited)
{
    bInputBuffered = true;

    if (bInComboWindow)
    {
        bInputBuffered = false;
        AdvanceCombo();
    }
}

void UGA_MeleeAttack::AdvanceCombo()
{
    ComboIndex++;

    // 1. 检查是否打完全部连招
    if (ComboIndex >= CurrentComboConfig->ComboSteps.Num())
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // 2. 获取下一段的配置
    const FComboStep& NextStep = CurrentComboConfig->ComboSteps[ComboIndex];
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    if (!NextStep.Montage || !ASC)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // 3. 判断是否切换了蒙太奇
    if (NextStep.Montage == CurrentMontage)
    {
        // 情况 A：同一个蒙太奇，只是跳到下一段 Section
        FName NextSection = NextStep.SectionName;
        // 如果没用 Section 机制，传 NAME_None 会从头播（通常不建议）
        ASC->CurrentMontageJumpToSection(NextSection);
    }
    else
    {
        // 情况 B：蒙太奇变了，必须结束旧 Task，开启新 Task
        if (MontageTask && IsValid(MontageTask))
        {
            MontageTask->EndTask();
        }

        CurrentMontage = NextStep.Montage;

        MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, NAME_None, CurrentMontage, 1.0f, NextStep.SectionName);

        MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
        MontageTask->ReadyForActivation();
        UE_LOG(LogTemp, Error, TEXT("MontageTask Activated for Combo Index %d"), ComboIndex);
    }
}

void UGA_MeleeAttack::OnDamageFrame(FGameplayEventData Payload)
{
    if (!CurrentComboConfig || !CurrentComboConfig->ComboSteps.IsValidIndex(ComboIndex)) return;
    TArray<FHitResult> HitResults = PerformMeleeTrace();
    if (HitResults.Num() == 0) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    // 获取当前段的伤害倍率
    float CurrentMultiplier = CurrentComboConfig->ComboSteps[ComboIndex].DamageMultiplier;
    // 将字符串 "Data.Damage.Multiplier" 转换为一个可以在 C++ 中使用的 FGameplayTag 对象。
    FGameplayTag MultiplierTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage.Multiplier"));
    //去重
    TSet<AActor*> UniqueTargets;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* Target = Hit.GetActor();
        if (!Target|| UniqueTargets.Contains(Target)) continue;
		UniqueTargets.Add(Target);

        UAbilitySystemComponent* TargetASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
		//TargetASC为空或者目标处于无敌状态，跳过
        if (!TargetASC|| TargetASC->HasMatchingGameplayTag(GASGameplayTags::State_Invincible)) continue;

        //同阵营忽略
		AActor* Avatar = GetAvatarActorFromActorInfo();
        if (!Avatar) continue;
		IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Avatar);
		IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(Target);
		if (OwnerTeam && TargetTeam && OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
		{
			
			continue;
		}

        // =================  网络同步（服务器权威） =================
        if (!GetOwningActorFromActorInfo()->HasAuthority())
        {
            // 客户端只负责表现（比如在 Hit.ImpactPoint 播放火花特效）
            // 伤害结算交由服务器处理，然后通过 GE 自动同步回客户端
            continue;
        }

		//应用伤害效果
        FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
        ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
        ContextHandle.AddHitResult(Hit);

        FGameplayEffectSpecHandle SpecHandle =
            SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
        
        if (SpecHandle.IsValid())
        {
			// 设置伤害倍率
			SpecHandle.Data->SetSetByCallerMagnitude(MultiplierTag, CurrentMultiplier);
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}

TArray<FHitResult> UGA_MeleeAttack::PerformMeleeTrace()
{
    TArray<FHitResult> HitResults;
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return HitResults;

    FVector Start = Avatar->GetActorLocation();
    FVector Forward = Avatar->GetActorForwardVector();
    FVector End = Start + Forward * TraceDistance;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Avatar);

    GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(TraceRadius),
        Params
    );

    return HitResults;
}

AActor* UGA_MeleeAttack::FindClosestEnemyInRange(float Range)
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return nullptr;

    FVector StartLoc = Avatar->GetActorLocation();

    // 忽略自己
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Avatar);

    // 使用多重重叠扫描，查找范围内的 Pawn
    TArray<FOverlapResult> OverlapResults;
    GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        StartLoc,
        FQuat::Identity,
        ECC_Pawn, // 扫描 Pawn 通道
        FCollisionShape::MakeSphere(Range),
        QueryParams
    );

    AActor* BestTarget = nullptr;
    float ClosestDistSq = TNumericLimits<float>::Max(); // 记录最短距离

    for (const FOverlapResult& Result : OverlapResults)
    {
        AActor* TargetActor = Result.GetActor();
        if (!TargetActor) continue;

        // 1. 阵营判定：如果是友军，直接跳过
        IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(Avatar);
        IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(TargetActor);
        if (OwnerTeam && TargetTeam && OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
        {
            continue;
        }

        // 2. 检查目标是否有 ASC（确保是战斗单位）
        if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
        {
            continue;
        }

        // 3. 距离计算（用平方距离比较，省去开方运算，性能更好）
        float DistSq = FVector::DistSquared(StartLoc, TargetActor->GetActorLocation());
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            BestTarget = TargetActor;
        }
    }

    return BestTarget;
}

UComboDataAsset* UGA_MeleeAttack::GetComboConfigByForm() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC && ASC->HasMatchingGameplayTag(GASGameplayTags::State_Weapon_Dual))
    {
		return DualComboConfig;
    }
    return CombinedComboConfig;
}
