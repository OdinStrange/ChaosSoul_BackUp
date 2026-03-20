#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/Image.h"
#include "PlayerCharacterBase.generated.h"

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	ATTACK UMETA(DisplayName = "ATTACK"),
	RUN UMETA(DisplayName = "RUN"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE UMETA(DisplayName = "NONE"),
	SWORD UMETA(DisplayName = "SWORD"),
	GREATSWORD UMETA(DisplayName = "GREATSWORD"),
};

UCLASS()
class CHAOSSOUL_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	// 속도에 따른 팔 길이 조절을 위한 설정값
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinArmLength = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxArmLength = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomInterpSpeed = 2.0f; // 변화 속도

private:
	void PlayerMeshInitialization();
	void WeaponMeshInitialization();
	void CameraInitialization();
	void InputInitialization();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Attack();
	void WeaponChange();

private:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* WeaponChangeAction;

public:
	UPROPERTY(VisibleAnywhere)
	bool bIsWeaponChange;

	UPROPERTY(EditAnywhere)
	float mouseSpeed = 30.0f;

	UPROPERTY(EditAnywhere)
	float PlayerMoveSpeed = 30.0f;

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* WeaponStaticMesh;

private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

public:
	EPlayerStates playerState = EPlayerStates::NONE;
	EWeaponType WeaponType = EWeaponType::NONE;

public:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* AttackSound;

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	UUserWidget* HUDWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UImage* Weapon_Icon = nullptr;
};
