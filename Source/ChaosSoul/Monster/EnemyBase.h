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

public:
	UPROPERTY(EditAnywhere)
	EEnemyType CurrentEnemyType = EEnemyType::NORMALENEMY;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, USkeletalMesh*> EnemyMeshes;

	UPROPERTY(EditAnywhere)
	TMap<EEnemyType, TSubclassOf<UAnimInstance>> EnemyAnimBlueprints;

	UPROPERTY(EditAnywhere)
	FVector EnemySize;

	UPROPERTY(EditAnywhere)
	float EnemyHp;

public:
	void InitializeMeshes();
	void InitializeAnimInstance();
	void InitializeSize();
	void InitializeHp();
	void InitializeDamage();
};
