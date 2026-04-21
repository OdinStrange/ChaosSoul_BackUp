// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/EnemyBase.h"
#include "Engine/Engine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree>EnemyBT(TEXT("/Script/AIModule.BehaviorTree'/Game/ChaosSoul/AI/BT_Enemy.BT_Enemy'"));
	if (EnemyBT.Succeeded())
	{
		EnemyBehaviorTree = EnemyBT.Object;
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (EnemyBehaviorTree != nullptr)
	{
		RunBehaviorTree(EnemyBehaviorTree);

		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
	//SetFocus(PlayerPawn);
	//MoveToActor(PlayerPawn);

	FVector MyLocation = GetPawn()->GetActorLocation();
	

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	float Distance = FVector::Dist(MyLocation, PlayerLocation);

	if (Distance <= 150)
	{
		//Attack
		GetBlackboardComponent()->SetValueAsBool(TEXT("bIsAttack"), true);

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Attack"));
	}
}