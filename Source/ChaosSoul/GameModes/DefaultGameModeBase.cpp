// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameModeBase.h"
#include "ChaosSoulCharacter.h"
#include "Engine/Engine.h"

ADefaultGameModeBase::ADefaultGameModeBase()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ADefaultGameModeBase Call"));
	static ConstructorHelpers::FClassFinder<AChaosSoulCharacter>
		PlayerCharacter(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	
	if (PlayerCharacter.Class)//예외처리
	{
		DefaultPawnClass = PlayerCharacter.Class;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("PlayerCharacter.Class InB"));
	}
}
