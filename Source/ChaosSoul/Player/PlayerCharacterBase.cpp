#include "Player/PlayerCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Particles/ParticleSystem.h"
#include "TimerManager.h"
#include "GameFramework/GameModeBase.h"
#include "Monster/EnemyBase.h"


APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerMeshInitialization();
	CameraInitialization();
	InputInitialization();
	WeaponMeshInitialization();
	// 초기 포션 개수 설정
	MaxPotion = 3;
	CurrentPotion = MaxPotion;

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/ChaosSoul/Blueprints/Player/ABP_PlayerCharacterBase.ABP_PlayerCharacterBase_C"));

	if (AnimInstance.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}

	static ConstructorHelpers::FClassFinder<UUserWidget>HUD(TEXT("/Game/ChaosSoul/Blueprints/BP_HUDUserWidget.BP_HUDUserWidget_C"));
	if (HUD.Succeeded())
	{
		HUDClass = HUD.Class;
	}
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultContext, 0);
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = PlayerMoveSpeed;

	if (WeaponSkeletalMesh)
	{
		WeaponSkeletalMesh->SetHiddenInGame(true);
	}

	if (HUDClass)
	{
		HUDWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass);
	}
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();

		Weapon_Icon = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("Weapon_Icon")));
		WeaponTextBlock = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SubTitleText")));
		if (UHUDUserWidget* HUD = Cast<UHUDUserWidget>(HUDWidget))
		{
			HPBar = HUD->HPBar;
			if (HPBar) HPBar->SetPercent(1.f);
		}
		// HUD에 초기 포션 개수 표시
		OnPotionChanged.Broadcast(CurrentPotion);

	}
	if (Weapon_Icon)
	{
		Weapon_Icon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (WeaponTextBlock)
	{
		WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
		WeaponTextBlock->SetText(FText::FromString(TEXT("None")));
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. 캐릭터의 로컬 앞방향 속도 계산
	float ForwardSpeed = FVector::DotProduct(GetVelocity(), GetActorForwardVector());

	// 2. 목표 거리 결정 (앞으로 가면 Max, 뒤로 가면 Min에 가깝게)
	// 캐릭터의 최대 속도 대비 현재 속도 비율로 계산하거나 간단한 조건문 사용
	float TargetLength = MinArmLength;

	if (ForwardSpeed > 10.0f) {
		TargetLength = MaxArmLength; // 앞으로 갈 때 멀어짐
	}
	else if (ForwardSpeed < -10.0f) {
		TargetLength = MinArmLength * 0.5f; // 뒤로 갈 때 더 당겨짐
	}
	else {
		TargetLength = MinArmLength; // 정지 시 기본 거리
	}

	// 3. FInterpTo를 사용하여 부드럽게 거리 조절
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		TargetLength,
		DeltaTime,
		ZoomInterpSpeed
	);

}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (!EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacterBase::Attack);
	// 숫자키 바인딩(Started가 의도에 맞음: "누른 순간 1회")
	if (SWORDAction) {
		EnhancedInputComponent->BindAction(SWORDAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnSelectSword);
	}
	if (GREATSWORDAction) {
		EnhancedInputComponent->BindAction(GREATSWORDAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnSelectGreatSword);
	}
	if (BLUNTAction) {
		EnhancedInputComponent->BindAction(BLUNTAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnSelectBlunt);
	}
	if (KATANAAction) {
		EnhancedInputComponent->BindAction(KATANAAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnSelectKatana);
	}
	if (PotionAction) {
		EnhancedInputComponent->BindAction(PotionAction, ETriggerEvent::Started, this, &APlayerCharacterBase::UsePotion);
	}

}

void APlayerCharacterBase::OnSelectSword(const FInputActionValue& Value)
{
	ChangeWeaponTo(EWeaponType::SWORD);
}

void APlayerCharacterBase::OnSelectGreatSword(const FInputActionValue& Value)
{
	ChangeWeaponTo(EWeaponType::GREATSWORD);
}

void APlayerCharacterBase::OnSelectBlunt(const FInputActionValue& Value)
{
	ChangeWeaponTo(EWeaponType::BLUNT);
}

void APlayerCharacterBase::OnSelectKatana(const FInputActionValue& Value)
{
	ChangeWeaponTo(EWeaponType::KATANA);
}

void APlayerCharacterBase::ChangeWeaponTo(EWeaponType NewType)//무기 변경 로직
{
	if (!WeaponSkeletalMesh) return;
	WeaponType = NewType;

    // TODO: Weapon_Icon 보이기
    // TODO: WeaponTextBlock 보이기
    // TODO: switch(NewType) 로 무기명/데미지 텍스트 구성
	if (Weapon_Icon)
	{
		Weapon_Icon->SetVisibility(ESlateVisibility::Visible);
		Weapon_Icon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));//컬러 및 투명도 조절 가능
	}
	if (WeaponTextBlock)
	{
		WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	if (NewType == EWeaponType::NONE)
	{
		if (Weapon_Icon)
		{
			Weapon_Icon->SetVisibility(ESlateVisibility::Hidden);
		}

		if (WeaponTextBlock)
		{
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			WeaponTextBlock->SetText(FText::FromString(TEXT("None")));
		}

		if (WeaponSkeletalMesh)
		{
			WeaponSkeletalMesh->SetHiddenInGame(true);
		}
		return;
	}

	WeaponSkeletalMesh->SetHiddenInGame(false);

	const TObjectPtr<USkeletalMesh>* FoundValuePtr = WeaponMeshMap.Find(NewType);
	if (FoundValuePtr == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString(TEXT("Weapon is not found")));
		return;
	}

	// TODO: UObject 포인터 꺼내기 (UStaticMesh*)
	USkeletalMesh* FoundMesh = FoundValuePtr->Get();

	// TODO: FoundMesh 널 체크 분기
	if (FoundMesh == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString(TEXT("WeaponMesh is not found")));
		return;
	}

	WeaponSkeletalMesh->SetSkeletalMesh(FoundMesh); //선택한 무기 타입에 맞는 메시로 교체

	switch (NewType)
	{
		case EWeaponType::SWORD:
		{
			AttackSpeed = 4.f;
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			FText Damage = FText::AsNumber(SwordDamage);
			FText NewText = FText::Format(FText::FromString(TEXT("소검: Power {0}")), Damage);
			WeaponTextBlock->SetText(NewText);
			// 소검 위치·회전 오프셋 (X=앞뒤, Y=좌우, Z=상하, 단위: cm)
			WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
			WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
			break;
		}

		case EWeaponType::GREATSWORD:
		{
			AttackSpeed = 0.5f;
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			FText Damage = FText::AsNumber(GreatSwordDamage);
			FText NewText = FText::Format(FText::FromString(TEXT("대검: Power {0}")), Damage);
			WeaponTextBlock->SetText(NewText);
			// 대검 위치·회전 오프셋
			WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
			WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
			break;
		}
		case EWeaponType::BLUNT:
		{
			AttackSpeed = 1.f;
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			FText Damage = FText::AsNumber(BluntDamage);
			FText NewText = FText::Format(FText::FromString(TEXT("둔기: Power {0}")), Damage);
			WeaponTextBlock->SetText(NewText);
			// 둔기 위치·회전 오프셋
			WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
			WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
			break;
		}
		case EWeaponType::KATANA:
		{
			AttackSpeed = 2.f;
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			FText Damage = FText::AsNumber(KatanaDamage);
			FText NewText = FText::Format(FText::FromString(TEXT("도: Power {0}")), Damage);
			WeaponTextBlock->SetText(NewText);
			// 도 위치·회전 오프셋
			WeaponSkeletalMesh->SetRelativeLocation(FVector(60, 0, 0));
			WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, 0, 180));
			break;
		}

		default:
			// TODO: 예상 못한 타입 방어(숨김 or 기본 문구)
			break;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString(TEXT("WeaponChange")));

	// TODO: BeginPlay에서 입력 액션 포인터 유효성 로그 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
			FString::Printf(TEXT("MoveAction=%s LookAction=%s AttackAction=%s SwordAction=%s"),
				MoveAction ? TEXT("OK") : TEXT("NULL"),
				LookAction ? TEXT("OK") : TEXT("NULL"),
				AttackAction ? TEXT("OK") : TEXT("NULL"),
				SWORDAction ? TEXT("OK") : TEXT("NULL")
			)
		);
	}
}

void APlayerCharacterBase::PlayerMeshInitialization()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		PlayerSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Fab/Armored_Guard_Knight_Rig/armored_guard_knight_rig/SkeletalMeshes/armored_guard_knight_rig.armored_guard_knight_rig'"));
	if (PlayerSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
		GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
}

void APlayerCharacterBase::WeaponMeshInitialization()
{
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SWORD"));
	// 생성자에서는 반드시 부모 컴포넌트가 있어야 하므로 일단 RootComponent에 임시 부착;
	// 실제 소켓 부착은 아래 AttachToComponent에서 처리
	WeaponSkeletalMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SWORD(TEXT("/Script/Engine.SkeletalMesh'/Game/ChaosSoul/SKM_short_sword.SKM_short_sword'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GREATSWORD(TEXT("/Script/Engine.SkeletalMesh'/Game/ChaosSoul/SKM_sword.SKM_sword'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BLUNT(TEXT("/Script/Engine.SkeletalMesh'/Game/ChaosSoul/SKM_weapon_mace_1.SKM_weapon_mace_1'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> KATANA(TEXT("/Script/Engine.SkeletalMesh'/Game/ChaosSoul/SKM_scene.SKM_scene'"));

	WeaponMeshMap.Empty();

	if (SWORD.Succeeded())
	{
		WeaponMeshMap.Add(EWeaponType::SWORD, SWORD.Object);
		WeaponSkeletalMesh->SetSkeletalMesh(SWORD.Object);
		// Weapon_Socket 기준 상대 회전 보정 — 무기 메시가 소켓 방향과 어긋날 때 수치를 조정
		WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
		// Weapon_Socket 기준 위치 오프셋 (X=앞뒤, Y=좌우, Z=상하, 단위: cm)
		WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
	}
	if (GREATSWORD.Succeeded())
	{
		WeaponMeshMap.Add(EWeaponType::GREATSWORD, GREATSWORD.Object);
		WeaponSkeletalMesh->SetSkeletalMesh(GREATSWORD.Object);
		// Weapon_Socket 기준 상대 회전 보정
		WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
		// Weapon_Socket 기준 위치 오프셋 (X=앞뒤, Y=좌우, Z=상하, 단위: cm)
		WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
	}
	if (BLUNT.Succeeded())
	{
		WeaponMeshMap.Add(EWeaponType::BLUNT, BLUNT.Object);
		WeaponSkeletalMesh->SetSkeletalMesh(BLUNT.Object);
		// Weapon_Socket 기준 상대 회전 보정
		WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
		// Weapon_Socket 기준 위치 오프셋 (X=앞뒤, Y=좌우, Z=상하, 단위: cm)
		WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
	}
	if (KATANA.Succeeded())
	{
		WeaponMeshMap.Add(EWeaponType::KATANA, KATANA.Object);
		WeaponSkeletalMesh->SetSkeletalMesh(KATANA.Object);
		// Weapon_Socket 기준 상대 회전 보정
		WeaponSkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
		// Weapon_Socket 기준 위치 오프셋 (X=앞뒤, Y=좌우, Z=상하, 단위: cm)
		WeaponSkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
	}

	if (WeaponSkeletalMesh)
	{
		// 캐릭터 메시의 Weapon_Socket에 무기를 부착 — 소켓 위치·회전을 기준으로 무기가 배치됨
		// 무기 위치를 바꾸려면 스켈레톤 에디터에서 Weapon_Socket 위치를 조정하거나
		// 위의 SetRelativeRotation / SetRelativeLocation으로 오프셋을 추가
		WeaponSkeletalMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_Socket"));

		WeaponSkeletalMesh->SetVisibility(false);
	}
}

void APlayerCharacterBase::CameraInitialization()
{
	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bOrientRotationToMovement = true;
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (SpringArm)
	{
		SpringArm->SetupAttachment(RootComponent);
		SpringArm->SetWorldLocation(FVector(0, 0, 40));
		SpringArm->TargetArmLength = 250;
		SpringArm->SocketOffset = FVector(0, 40, 0);
		SpringArm->bUsePawnControlRotation = true;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->SetupAttachment(SpringArm);
	}
}

void APlayerCharacterBase::InputInitialization()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ChaosSoul/Input/IMC_Default.IMC_Default'"));
	if (InputContext.Object != nullptr)
	{
		DefaultContext = InputContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMove(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Move.IA_Move'"));
	if (InputMove.Object != nullptr)
	{
		MoveAction = InputMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLook(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Look.IA_Look'"));
	if (InputLook.Object != nullptr)
	{
		LookAction = InputLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAttack(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Attack.IA_Attack'"));
	if (InputAttack.Object != nullptr)
	{
		AttackAction = InputAttack.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputSWORDChange(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_SWORD.IA_SWORD'"));
	if (InputSWORDChange.Object != nullptr)
	{
		SWORDAction = InputSWORDChange.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputGREATSWORDChange(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_GREATSWORD.IA_GREATSWORD'"));
	if (InputGREATSWORDChange.Object != nullptr)
	{
		GREATSWORDAction = InputGREATSWORDChange.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputBLUNTChange(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_BLUNT.IA_BLUNT'"));
	if (InputBLUNTChange.Object != nullptr)
	{
		BLUNTAction = InputBLUNTChange.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputKATANAChange(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_KATANA.IA_KATANA'"));
	if (InputKATANAChange.Object != nullptr)
	{
		KATANAAction = InputKATANAChange.Object;
	}
}

void APlayerCharacterBase::EffectInitialization()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireEffectObj(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (FireEffectObj.Succeeded())
	{
		FireEffect = FireEffectObj.Object;
	}
}



void APlayerCharacterBase::Move(const FInputActionValue& Value) 
{
	if (playerState == EPlayerStates::ATTACK) return;

	const FVector2D Movement = Value.Get<FVector2D>();//X=좌우, Y=앞뒤

	const FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
	const FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = UKismetMathLibrary::GetForwardVector(YawOnly);
	const FVector Right = UKismetMathLibrary::GetRightVector(YawOnly);

	AddMovementInput(Forward, Movement.Y);
	AddMovementInput(Right, Movement.X);

}

void APlayerCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * GetWorld()->DeltaTimeSeconds * mouseSpeed);
	AddControllerPitchInput(LookAxisVector.Y * GetWorld()->DeltaRealTimeSeconds * mouseSpeed);

}

void APlayerCharacterBase::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !bCanAttack || AnimInstance->Montage_IsPlaying(AttackMontage)) return;

	bCanAttack = false;
	playerState = EPlayerStates::ATTACK;

	if (AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, AttackSpeed);
		AttackTrace();
	}

	if (WeaponCamerashakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(WeaponCamerashakeClass);
	}

	//람다 형식 : [캡처리스트](매개변수)->반환형 {실행코드};

	FTimerHandle AttackHandle;

	GetWorld()->GetTimerManager().SetTimer(
		AttackHandle,
		[this]()
		{
			playerState = EPlayerStates::NONE;
		}, 1.5f / AttackSpeed, //공격 속도가 빠를수록 쿨타임도 짧아짐
		false);

	// 공격 쿨타임 타이머 — AttackCooldown(초) 후 다시 공격 가능
	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownHandle,
		[this]()
		{
			bCanAttack = true;
		},
		AttackCooldown,
		false);

	if (WeaponType == EWeaponType::NONE) return;
}

void APlayerCharacterBase::AttackTrace()
{
	FVector StartLocation = WeaponSkeletalMesh->GetSocketLocation("Attack_Socket"); //공격 소켓 위치를 판정 시작점으로 사용
	FVector EndLocation = StartLocation; //제자리 구체 스윕이므로 끝점도 동일
	float SphereRadius = 100.0f; //공격 판정 반경(cm)

	//현재 장착 무기 타입에 따라 데미지 값 결정
	float Damage = 0.f;
	switch (WeaponType)
	{
	case EWeaponType::SWORD:      Damage = SwordDamage;      break;
	case EWeaponType::GREATSWORD: Damage = GreatSwordDamage; break;
	case EWeaponType::BLUNT:      Damage = BluntDamage;      break;
	case EWeaponType::KATANA:     Damage = KatanaDamage;     break;
	default: break;
	}

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); //자기 자신은 판정에서 제외

	//구체 스윕으로 공격 범위 안의 액터 한 개를 감지 (ECC_Pawn: ACharacter 기본 응답 채널)
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SphereRadius),
		Params
	);

	AActor* HitActor = HitResult.GetActor();

	// 스윕 결과와 무관하게 공격 범위 시각화 (맞으면 빨강, 빗나가면 초록)
	DrawDebugSphere(GetWorld(), StartLocation, SphereRadius, 12,
		bHit ? FColor::Red : FColor::Green, false, 1.0f);

	//진단 로그: 스윕 결과 확인
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
		FString::Printf(TEXT("[AttackTrace] bHit=%s | Actor=%s | Damage=%.0f"),
			bHit ? TEXT("true") : TEXT("false"),
			HitActor ? *HitActor->GetName() : TEXT("null"),
			Damage));

	if (bHit && HitActor)
	{
		//피격 액터의 TakeDamage를 호출해 실제 데미지를 전달 — HP 감소는 상대방 TakeDamage에서 처리
		UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());

		// 적(AEnemyBase)에게 맞았을 때만 구체와 이펙트 표시
		if (Cast<AEnemyBase>(HitActor))
		{
			DrawDebugSphere(GetWorld(), StartLocation, SphereRadius, 12, FColor::Red, false, 1.0f);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, HitActor->GetActorLocation());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, StartLocation);
		}
	}
}

float APlayerCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	//부모(AActor)의 TakeDamage를 먼저 실행해 엔진 내부 처리를 보장
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//CurrentHP에서 데미지를 빼되 0~MaxHP 범위를 벗어나지 않게 고정
	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.f, MaxHP);
	if (HPBar) HPBar->SetPercent(CurrentHP / MaxHP);

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
		FString::Printf(TEXT("[Player] 데미지: %.0f | HP: %.0f / %.0f"), ActualDamage, CurrentHP, MaxHP));

	if (CurrentHP <= 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("[Player] 사망 → 리스폰 대기"));
		Die();
	}

	return ActualDamage; //실제로 적용된 데미지 값을 반환
}


void APlayerCharacterBase::UsePotion()
{
	if (CurrentPotion <= 0) return;

	CurrentPotion--;

	// HP를 HealAmount만큼 회복, MaxHP 초과 방지
	CurrentHP = FMath::Clamp(CurrentHP + HealAmount, 0.f, MaxHP);
	if (HPBar) HPBar->SetPercent(CurrentHP / MaxHP);

	if (OnPotionChanged.IsBound())
	{
		OnPotionChanged.Broadcast(CurrentPotion);
	}
}

void APlayerCharacterBase::Die()
{
	// 사망 시 포션 초기화
	CurrentPotion = MaxPotion;
	OnPotionChanged.Broadcast(CurrentPotion);

	// 리스폰 후 새 HUD가 생성되기 전에 기존 HUD를 화면에서 제거
	if (HUDWidget) HUDWidget->RemoveFromParent();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// PC는 폰이 Destroy된 후에도 살아있으므로 타이머를 PC에 설정
		FTimerHandle TimerHandle;
		PC->GetWorldTimerManager().SetTimer(TimerHandle, [PC]()
		{
			if (PC && PC->GetWorld())
			{
				PC->GetWorld()->GetAuthGameMode()->RestartPlayer(PC);
			}
		}, RespawnDelay, false);
	}

	Destroy();
}
