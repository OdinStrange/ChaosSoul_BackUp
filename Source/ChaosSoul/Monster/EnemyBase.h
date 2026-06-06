// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	NORMALENEMY UMETA(DisplayName = "Zombie"),
	MAGICENEMY UMETA(DisplayName = "Juggernaut"),
};

UCLASS()
class CHAOSSOUL_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	//ApplyDamage 호출 시 엔진이 이 함수를 실행해 CurrentEnemyHp를 깎는다

public:
	UPROPERTY(EditAnywhere)
	EEnemyType CurrentEnemyType = EEnemyType::NORMALENEMY;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, USkeletalMesh*> EnemyMeshes;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, TSubclassOf<UAnimInstance>> EnemyAnimBlueprints;

	UPROPERTY(EditAnywhere)
	FVector EnemySize;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxEnemyHp; //에디터에서 몬스터 타입별 최대 HP 설정

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CurrentEnemyHp; //런타임 중 실제로 깎이는 HP

	UPROPERTY(EditAnywhere)
	float EnemyDamage;

public:
	void InitializeMeshes();
	void InitializeAnimInstance();
	void InitializeSize();
	void InitializeHp();
	void InitializeDamage();
	void InitializeAIController();
};
