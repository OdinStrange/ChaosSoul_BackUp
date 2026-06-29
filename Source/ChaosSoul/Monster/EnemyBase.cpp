// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EnemyBase.h"
#include "Monster/EnemyAIController.h"
#include "Player/PlayerCharacterBase.h"
#include "HUDUserWidget.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InitializeAIController();

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageFinder(
		TEXT("/Game/ChaosSoul/Enemy/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage"));
	if (AttackMontageFinder.Succeeded())
	{
		AttackMontage = AttackMontageFinder.Object;
	}
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeMeshes();
	InitializeAnimInstance();
	InitializeSize();
	InitializeHp();
	InitializeDamage();
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
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
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
		MaxEnemyHp = 100;
	}
	else if (CurrentEnemyType == EEnemyType::MAGICENEMY)
	{
		MaxEnemyHp = 200;
	}
	CurrentEnemyHp = MaxEnemyHp; //시작 시 현재 HP를 최대 HP로 초기화
}

void AEnemyBase::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackMontage) return;
	if (AnimInstance->Montage_IsPlaying(AttackMontage)) return;

	AnimInstance->Montage_Play(AttackMontage);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	//부모(AActor)의 TakeDamage를 먼저 실행해 엔진 내부 처리를 보장
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//CurrentEnemyHp에서 데미지를 빼되 0~MaxEnemyHp 범위를 벗어나지 않게 고정
	CurrentEnemyHp = FMath::Clamp(CurrentEnemyHp - ActualDamage, 0.f, MaxEnemyHp);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(PC->GetPawn()))
		{
			if (UHUDUserWidget* HUD = Cast<UHUDUserWidget>(PlayerChar->HUDWidget))
			{
				if (HUD->EnemyHPBar)
					HUD->EnemyHPBar->SetPercent(CurrentEnemyHp / MaxEnemyHp);
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
		FString::Printf(TEXT("[Enemy] 데미지: %.0f | HP: %.0f / %.0f"), ActualDamage, CurrentEnemyHp, MaxEnemyHp));

	if (CurrentEnemyHp <= 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("[Enemy] 사망 → Destroy"));
		//HP가 0 이하면 이 액터를 월드에서 제거
		Destroy();
	}

	return ActualDamage; //실제로 적용된 데미지 값을 반환
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
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

