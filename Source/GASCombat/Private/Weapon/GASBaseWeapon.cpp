// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GASBaseWeapon.h"
#include"Components/StaticMeshComponent.h"


AGASBaseWeapon::AGASBaseWeapon()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGASBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}


