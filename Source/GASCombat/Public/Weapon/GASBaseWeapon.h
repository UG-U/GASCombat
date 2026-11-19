// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GASBaseWeapon.generated.h"

class UStaticMeshComponent;

UCLASS()
class GASCOMBAT_API AGASBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AGASBaseWeapon();

	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName AttachSocketName;

public:	

};
