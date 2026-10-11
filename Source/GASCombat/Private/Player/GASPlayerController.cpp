// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GASPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include"GameFramework/Character.h"
#include <EnhancedInput/GASInputComponent.h>
#include <AbilitySystem/GASAbilitySystemComponent.h>
#include <AbilitySystemBlueprintLibrary.h>
AGASPlayerController::AGASPlayerController()
{

	
}
void AGASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UGASInputComponent* GASInput = Cast<UGASInputComponent>(InputComponent);
	if (GASInput)
	{
		//Move
		GASInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGASPlayerController::Move);
		//Look
		GASInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGASPlayerController::Look);
		//Jump
		GASInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AGASPlayerController::Jump);
		GASInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGASPlayerController::StopJumping);
		//Ability
		GASInput->BindAbilityAction(DAInputConfig,this,
			&AGASPlayerController::AbilityInputTagPressed,
			&AGASPlayerController::AbilityInputTagReleased, 
			&AGASPlayerController::AbilityInputTagHeld);
	}

}

void AGASPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(GASInputContext);
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(GASInputContext, 0);
		}
	}

	bShowMouseCursor = true;//强制显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default;//设置默认鼠标光标也即将光标形式为箭头

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AGASPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	//forward/backward movement
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//right/left movement
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControllerPawn = GetPawn())
	{
		ControllerPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControllerPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGASPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (APawn* ControllerPawn = GetPawn())
	{
		ControllerPawn->AddControllerYawInput(LookAxisVector.X);
		ControllerPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGASPlayerController::Jump()
{
	ACharacter* GCharacter = Cast<ACharacter>(GetPawn());
	if(GCharacter)
	{
		GCharacter->Jump();
	}
}

void AGASPlayerController::StopJumping()
{
	ACharacter* GCharacter = Cast<ACharacter>(GetPawn());
	if (GCharacter)
	{
		GCharacter->StopJumping();
	}
}

void AGASPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Green, *InputTag.ToString());
}

void AGASPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Black, *InputTag.ToString());
	if (GetGASASC() == nullptr) return;
	GetGASASC()->AbilityInputTagReleased(InputTag);
}

void AGASPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Blue, *InputTag.ToString());
	if (GetGASASC() == nullptr) return;
	GetGASASC()->AbilityInputTagHeld(InputTag);
}

UGASAbilitySystemComponent* AGASPlayerController::GetGASASC()
{
	if (GASASC == nullptr)
	{
		GASASC= Cast<UGASAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return GASASC;
}


