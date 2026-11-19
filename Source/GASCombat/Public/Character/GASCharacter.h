// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCharacterBase.h"
#include "GASCharacter.generated.h"


class AGASBaseWeapon;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class GASCOMBAT_API AGASCharacter : public AGASCharacterBase
{
	GENERATED_BODY()
	
public:
	AGASCharacter();

	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual int32 GetPlayerLevel()override;

    // 生成默认双刀（在服务器端初始化时调用）
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SpawnDualWeapons();

    // 切换为合体刀（由技能调用）
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToCombinedWeapon();

    // 切换为双刀（由技能调用）
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwitchToDualWeapons();

    // 获取当前主武器
    UFUNCTION(BlueprintPure, Category = "Weapon")
    AGASBaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
    // 内部通用挂载函数
    void EquipWeapon(AGASBaseWeapon* InWeapon, FName SocketName);

    // 网络同步
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


    // 蓝图中配置的武器类
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AGASBaseWeapon> DualRightWeaponClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AGASBaseWeapon> DualLeftWeaponClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AGASBaseWeapon> CombinedWeaponClass;

    //配置标签类
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> DualWeaponGE;

    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<UGameplayEffect> CombinedWeaponGE;


    // 当前持有的武器实例
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<AGASBaseWeapon> LeftWeapon;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<AGASBaseWeapon> RightWeapon;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<AGASBaseWeapon> CurrentWeapon;


private:

	void InitAbilityActorInfo();

	void InitHUDOverlay();
};
