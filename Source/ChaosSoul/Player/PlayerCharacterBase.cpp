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


APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerMeshInitialization();
	CameraInitialization();
	InputInitialization();
	WeaponMeshInitialization();

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/ChaosSoul/Blueprints/Player/ABP_Player.ABP_Player_C"));

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
	if (HUDClass)
	{
		HUDWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), HUDClass);
	}
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();

		Weapon_Icon = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("Sword_Icon")));
		WeaponTextBlock = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SubTitleText")));

	}
	if (Weapon_Icon)
	{
		Weapon_Icon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (WeaponTextBlock)
	{
		WeaponTextBlock->SetVisibility(ESlateVisibility::Hidden);
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

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacterBase::Attack);
	EnhancedInputComponent->BindAction(WeaponChangeAction, ETriggerEvent::Started, this, &APlayerCharacterBase::WeaponChange);
}

void APlayerCharacterBase::PlayerMeshInitialization()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		PlayerSkeletalMesh(TEXT("/Script/Engine.Skeleton'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone_Skeleton.Greystone_Skeleton'"));
	if (PlayerSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
		GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
}

void APlayerCharacterBase::WeaponMeshInitialization()
{
	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SWORD"));
	WeaponStaticMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SWORD(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Sword/sword/StaticMeshes/sword.sword'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GREATSWORD(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Free_Prototype_Stylized_Weapons_V1/Wpn_2HSword_Set_01A1.Wpn_2HSword_Set_01A1'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BLUNT(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Weapon_Mace_1/weapon_mace_1/StaticMeshes/weapon_mace_1.weapon_mace_1'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KATANA(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Katana/scene/StaticMeshes/scene.scene'"));

	if (SWORD.Succeeded())
	{
		WeaponStaticMesh->SetStaticMesh(SWORD.Object);
	}
	if (GREATSWORD.Succeeded())
	{
		WeaponStaticMesh->SetStaticMesh(GREATSWORD.Object);
	}
	if (BLUNT.Succeeded())
	{
		WeaponStaticMesh->SetStaticMesh(BLUNT.Object);
	}
	if (KATANA.Succeeded())
	{
		WeaponStaticMesh->SetStaticMesh(KATANA.Object);
	}

	if (WeaponStaticMesh)
	{
		WeaponStaticMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_Socket"));

		WeaponStaticMesh->SetVisibility(false);
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

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMove(TEXT("/ Script / EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Move.IA_Move'"));
	if (InputMove.Object != nullptr)
	{
		MoveAction = InputMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLook(TEXT("/ Script / EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Look.IA_Look'"));
	if (InputLook.Object != nullptr)
	{
		LookAction = InputLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAttack(TEXT("/ Script / EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_Attack.IA_Attack'"));
	if (InputAttack.Object != nullptr)
	{
		AttackAction = InputAttack.Object;
	}

	static ConstructorHelpers::FObjectFinder <UInputAction> InputWeaponChange(TEXT("/Script/EnhancedInput.InputAction'/Game/ChaosSoul/Input/Action/IA_WeaponChange.IA_WeaponChange'"));
	if (InputWeaponChange.Object != nullptr)
	{
		WeaponChangeAction = InputWeaponChange.Object;
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
	playerState = EPlayerStates::ATTACK;

	WeaponType = EWeaponType::NONE;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);



	}

	//람다 형식 : [캡처리스트](매개변수)->반환형 {실행코드};

	FTimerHandle AttackHandle;

	GetWorld()->GetTimerManager().SetTimer(
		AttackHandle,
		[this]()
		{
			playerState = EPlayerStates::NONE;
		}, 1.5f,
		false);

	if (WeaponType == EWeaponType::NONE) return;

	if (playerState == EPlayerStates::ATTACK) return;
}
void APlayerCharacterBase::WeaponChange()
{
	bIsWeaponChange = !bIsWeaponChange;

	if (bIsWeaponChange)
	{
		WeaponType = EWeaponType::SWORD;
		if (Weapon_Icon)
		{
			Weapon_Icon->SetVisibility(ESlateVisibility::Visible);
		}

		if (WeaponTextBlock)
		{
			WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
			FText Damage = FText::AsNumber(SwordDamage);
			FText NewText = FText::Format(FText::FromString(TEXT("소검: Power {0}")), Damage);
			WeaponTextBlock->SetText(NewText);
		}
	}
	else if (bIsWeaponChange)
	{
			WeaponType = EWeaponType::GREATSWORD;
			if (Weapon_Icon)
			{
				Weapon_Icon->SetVisibility(ESlateVisibility::Visible);
			}

			if (WeaponTextBlock)
			{
				WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
				FText Damage = FText::AsNumber(GreatSwordDamage);
				FText NewText = FText::Format(FText::FromString(TEXT("대검: Power {0}")), Damage);
				WeaponTextBlock->SetText(NewText);
			}
	}
	else if (bIsWeaponChange)
	{
			WeaponType = EWeaponType::BLUNT;
			if (Weapon_Icon)
			{
				Weapon_Icon->SetVisibility(ESlateVisibility::Visible);
			}

			if (WeaponTextBlock)
			{
				WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
				FText Damage = FText::AsNumber(BluntDamage);
				FText NewText = FText::Format(FText::FromString(TEXT("둔기: Power {0}")), Damage);
				WeaponTextBlock->SetText(NewText);
			}
	}
	else if (bIsWeaponChange)
	{
			WeaponType = EWeaponType::KATANA;
			if (Weapon_Icon)
			{
				Weapon_Icon->SetVisibility(ESlateVisibility::Visible);
			}

			if (WeaponTextBlock)
			{
				WeaponTextBlock->SetVisibility(ESlateVisibility::Visible);
				FText Damage = FText::AsNumber(KatanaDamage);
				FText NewText = FText::Format(FText::FromString(TEXT("도: Power {0}")), Damage);
				WeaponTextBlock->SetText(NewText);
			}
	}

	else
	{
		WeaponType = EWeaponType::NONE;
		if (Weapon_Icon)
		{
			Weapon_Icon->SetVisibility(ESlateVisibility::Hidden);
		}

		if (WeaponTextBlock)
		{
			WeaponTextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (WeaponStaticMesh)
	{
		WeaponStaticMesh->SetVisibility(bIsWeaponChange);
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString(TEXT("WeaponChange")));
}
