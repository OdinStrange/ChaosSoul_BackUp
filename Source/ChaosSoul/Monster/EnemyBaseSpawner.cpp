#include "EnemyBaseSpawner.h"
#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Variant_Combat/Interfaces/CombatActivatable.h"

AEnemyBaseSpawner::AEnemyBaseSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SpawnCapsule"));
	SpawnCapsule->SetupAttachment(RootComponent);
	SpawnCapsule->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpawnCapsule->SetCapsuleSize(35.0f, 90.0f);
	SpawnCapsule->SetCollisionProfileName(FName("NoCollision"));

	SpawnDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));
	SpawnDirection->SetupAttachment(RootComponent);
}

void AEnemyBaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bShouldSpawnEnemiesImmediately)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyBaseSpawner::SpawnEnemy, InitialSpawnDelay);
	}
}

void AEnemyBaseSpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
}

void AEnemyBaseSpawner::SpawnEnemy()
{
	if (!IsValid(EnemyClass))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, SpawnCapsule->GetComponentTransform(), SpawnParams);
	if (SpawnedEnemy)
	{
		SpawnedEnemy->OnEnemyDied.AddDynamic(this, &AEnemyBaseSpawner::OnEnemyDied);
	}
}

void AEnemyBaseSpawner::OnEnemyDied()
{
	--SpawnCount;

	if (SpawnCount <= 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyBaseSpawner::SpawnerDepleted, ActivationDelay);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyBaseSpawner::SpawnEnemy, RespawnDelay);
}

void AEnemyBaseSpawner::SpawnerDepleted()
{
	for (AActor* Actor : ActorsToActivateWhenDepleted)
	{
		if (ICombatActivatable* Activatable = Cast<ICombatActivatable>(Actor))
		{
			Activatable->ActivateInteraction(this);
		}
	}
}

void AEnemyBaseSpawner::ToggleInteraction(AActor* ActivationInstigator)
{
	// stub
}

void AEnemyBaseSpawner::ActivateInteraction(AActor* ActivationInstigator)
{
	if (bHasBeenActivated || bShouldSpawnEnemiesImmediately)
	{
		return;
	}

	bHasBeenActivated = true;
	SpawnEnemy();
}

void AEnemyBaseSpawner::DeactivateInteraction(AActor* ActivationInstigator)
{
	// stub
}
