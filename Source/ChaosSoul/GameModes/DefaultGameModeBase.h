// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameModeBase.generated.h"
//#include는 다른 파일 내용을 지금 파일에 붙여넣기하라라는 뜻

/**
 * 
 */
UCLASS()
class CHAOSSOUL_API ADefaultGameModeBase : public AGameModeBase//언리얼의 AGameModeBase를 상속받아서 ADefaultGameModeBase라는 새 클래스를 만든다
{
	GENERATED_BODY()//필요한 숨은 코드들 넣어주는 매크로
	
public:
	ADefaultGameModeBase();//생성자
};
