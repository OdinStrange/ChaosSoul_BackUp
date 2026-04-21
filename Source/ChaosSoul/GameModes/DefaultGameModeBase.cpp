// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameModeBase.h"
#include "ChaosSoulCharacter.h"
#include "Engine/Engine.h"

ADefaultGameModeBase::ADefaultGameModeBase()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ADefaultGameModeBase Call"));
	////이 문구는 화면의 -1, 3.0f 좌표에 붉은 글씨로 디버그 텍스트를 잠깐 띄우라는 뜻
	static ConstructorHelpers::FClassFinder<AChaosSoulCharacter>
		PlayerCharacter(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	//생성자에서 BP_ThirdPersonCharacter 블루프린트 클래스를 찾아서 PlayerCharacter라는 변수에 담는다.
	
	if (PlayerCharacter.Class)//예외처리
	{
		DefaultPawnClass = PlayerCharacter.Class;//기본 폰 클래스에 플레이어캐릭터 클래스를 대입하라.
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("PlayerCharacter.Class InB"));
	}
}
