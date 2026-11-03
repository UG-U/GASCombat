// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class GASCOMBAT_API UAnimNotify_ComboWindow : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// 是否开启连击窗口。勾选=开启，取消勾选=关闭
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	bool bOpenWindow = true;
};
