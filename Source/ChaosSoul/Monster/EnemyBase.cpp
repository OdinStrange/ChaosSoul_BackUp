// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::InitializeMeshes()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		if (EnemyMeshes[EEnemyType::NORMALENEMY])
		{
			GetMesh()->SetSkeletalMesh(EnemyMeshes[EEnemyType::NORMALENEMY]);
		}
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		if (EnemyMeshes[EEnemyType::MAGICENEMY])
		{
			GetMesh()->SetSkeletalMesh(EnemyMeshes[EEnemyType::MAGICENEMY]);
		}
	}
}

void AEnemyBase::InitializeAnimInstance()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		GetMesh()->SetAnimInstanceClass(EnemyAnimBlueprints[EEnemyType::NORMALENEMY]);
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		GetMesh()->SetAnimInstanceClass(EnemyAnimBlueprints[EEnemyType::MAGICENEMY]);
	}
}

void AEnemyBase::InitializeSize()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		EnemySize = FVector(1.1f, 1.1f, 1.1f);
		SetActorScale3D(EnemySize);
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		EnemySize = FVector(1.5f, 1.5f, 1.5f);
		SetActorScale3D(EnemySize);
	}
}

void AEnemyBase::InitializeHp()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		EnemyHp = 100;
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		EnemyHp = 200;
	}
}

void AEnemyBase::InitializeDamage()
{
	if (CurrentEnemyType == EEnemyType::NORMALENEMY)
	{
		EnemyDamage = 10;
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		EnemyDamage = 20;
	}
}

void AEnemyBase::InitializeAIController()
{
	/*AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;*/
}

