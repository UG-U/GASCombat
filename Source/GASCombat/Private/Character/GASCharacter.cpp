// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacter.h"
#include <Player/GASPlayerState.h>
#include"AbilitySystemComponent.h"
#include <Player/GASPlayerController.h>
#include"UI/HUD/GASHUD.h"
#include <AbilitySystem/GASAbilitySystemComponent.h>
#include <Net/UnrealNetwork.h>
#include"Weapon/GASBaseWeapon.h"
#include"GASGameplayTags.h"

AGASCharacter::AGASCharacter()
{

}

void AGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	InitHUDOverlay();
	InitialAttributes();

	AddCharacterAbilities();
}

void AGASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	InitHUDOverlay();
}

int32 AGASCharacter::GetPlayerLevel()
{
	AGASPlayerState* GASPlayerState = GetPlayerState<AGASPlayerState>();
	if (!GASPlayerState) return 0;

	return GASPlayerState->GetPlayerLevel();
}

void AGASCharacter::SpawnDualWeapons()
{
	if (!HasAuthority()) return; // 只在服务器生成

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// 生成右手主武器
	if (DualRightWeaponClass)
	{
		RightWeapon = GetWorld()->SpawnActor<AGASBaseWeapon>(DualRightWeaponClass, GetActorTransform(), SpawnParams);
		EquipWeapon(RightWeapon, "WeaponRight");
		CurrentWeapon = RightWeapon;
	}

	// 生成左手副武器
	if (DualLeftWeaponClass)
	{
		LeftWeapon = GetWorld()->SpawnActor<AGASBaseWeapon>(DualLeftWeaponClass, GetActorTransform(), SpawnParams);
		EquipWeapon(LeftWeapon, "WeaponLeft");
	}

	// 确保有双刀标签
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(CombinedWeaponGE, nullptr);
		ASC->ApplyGameplayEffectToSelf(DualWeaponGE.GetDefaultObject(), 1.f, ASC->MakeEffectContext());
	}
}

void AGASCharacter::SwitchToCombinedWeapon()
{
	if (!HasAuthority()) return;

	// 1. 销毁左右双刀
	if (LeftWeapon) { LeftWeapon->Destroy(); LeftWeapon = nullptr; }
	if (RightWeapon) { RightWeapon->Destroy(); RightWeapon = nullptr; }
	if (CurrentWeapon) { CurrentWeapon->Destroy(); CurrentWeapon = nullptr; }

	// 2. 生成合体刀
	if (CombinedWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		CurrentWeapon = GetWorld()->SpawnActor<AGASBaseWeapon>(CombinedWeaponClass, GetActorTransform(), SpawnParams);
		// 合体刀通常也是握在右手
		EquipWeapon(CurrentWeapon, "Weapon_Combine");
	}

	// 3. 切换标签状态
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(DualWeaponGE, nullptr);
		ASC->ApplyGameplayEffectToSelf(CombinedWeaponGE.GetDefaultObject(), 1.f, ASC->MakeEffectContext());
	}
}

void AGASCharacter::SwitchToDualWeapons()
{
	if (!HasAuthority()) return;

	if (CurrentWeapon) { CurrentWeapon->Destroy(); CurrentWeapon = nullptr; }

	SpawnDualWeapons(); // 内部已经处理了移除 Combined 标签和添加 Dual 标签
}

void AGASCharacter::EquipWeapon(AGASBaseWeapon* InWeapon, FName SocketName)
{
	if (InWeapon)
	{
		InWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void AGASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASCharacter, LeftWeapon);
	DOREPLIFETIME(AGASCharacter, RightWeapon);
	DOREPLIFETIME(AGASCharacter, CurrentWeapon);
}

void AGASCharacter::InitAbilityActorInfo()
{
	AGASPlayerState* PS = GetPlayerState<AGASPlayerState>();
	if (!PS)return;
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)return;
	ASC->InitAbilityActorInfo(PS, this);//初始化AbilityActorInfo，传入PlayerState(拥有者）和Character（化身）
	Cast<UGASAbilitySystemComponent>(ASC)->AbilityActorInfoSet();

	SpawnDualWeapons(); // 确保在初始化时生成双刀
}

void AGASCharacter::InitHUDOverlay()
{
	AGASPlayerState* PS = GetPlayerState<AGASPlayerState>();
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	AGASPlayerController* PC = Cast<AGASPlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitHUDOverlay: PlayerController is null, skipping."));
		return;
	}
	AGASHUD* GASHUD = Cast<AGASHUD>(PC->GetHUD());
	if (!GASHUD) return;
	GASHUD->InitOverlay(PS, PC, ASC, AS);

}
