// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask_EnemyAttack.h"
#include "Player/PlayerCharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Monster/EnemyBase.h"
#include "Monster/EnemyAIController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "BTTask_EnemyAttack.h"


UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = "MonsterAttackTask";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Attack"));

	return EBTNodeResult::Succeeded;
}

