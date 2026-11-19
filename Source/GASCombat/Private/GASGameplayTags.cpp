// Fill out your copyright notice in the Description page of Project Settings.


#include "GASGameplayTags.h"




namespace GASGameplayTags {
	UE_DEFINE_GAMEPLAY_TAG(Attributes_Primary_Strength, "Attributes.Primary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Attributes_Primary_Resistance, "Attributes.Primary.Resistance");
	UE_DEFINE_GAMEPLAY_TAG(Attributes_Primary_Vigar, "Attributes.Primary.Vigar");

	UE_DEFINE_GAMEPLAY_TAG(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana");

	/*
	* ¼¼ÄÜÊäÈë±êÇ©
	*/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_RM, "InputTag.RM");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LM, "InputTag.LM");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_1, "InputTag.1");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_2, "InputTag.2");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_3, "InputTag.3");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_F, "InputTag.F");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_E, "InputTag.E");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_R, "InputTag.R");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shift, "InputTag.Shift");


	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Melee, "Ability.Attack.Melee");

	UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_SwitchForm, "Event.Weapon.SwitchForm");

	UE_DEFINE_GAMEPLAY_TAG(Event_Combo_Window_Open, "Event.Combo.Window.Open");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combo_Window_Close, "Event.Combo.Window.Close");

	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_DamageFrame, "Event.Combat.DamageFrame");

	UE_DEFINE_GAMEPLAY_TAG(Event_Skill_SpawnWave, "Event.Skill.SwordWave");

	//ÉËº¦±¶ÂÊ±êÇ©
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage_Multiplier, "Data.Damage.Multiplier");

	/*
	* ×´Ì¬±êÇ©
	*/
	UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking");//¹¥»÷×´Ì¬
	UE_DEFINE_GAMEPLAY_TAG(State_Invincible, "State.Invincible");//ÎÞµÐ×´Ì¬
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Dual, "State.Weapon.Dual");//Ë«µ¶ÐÎÌ¬
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Combined, "State.Weapon.Combined");//µ¥µ¶ÐÎÌ¬
}





