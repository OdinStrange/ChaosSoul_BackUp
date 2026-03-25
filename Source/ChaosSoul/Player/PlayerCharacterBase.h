#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
	BLUNT UMETA(DisplayName = "BLUNT"),
	KATANA UMETA(DisplayName = "KATANA"),
	
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

private:
	// 숫자키 액션 바인딩용 콜백(Enhanced Input 시그니처 고정)
	UFUNCTION()
	void OnSelectSword(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectGreatSword(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectBlunt(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectKatana(const FInputActionValue& Value);

	// 실제 무기 변경 로직 (입력 바인딩에 직접 연결하지 않음)
	void ChangeWeaponTo(EWeaponType NewType);

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

private:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

public:
	UPROPERTY(VisibleAnywhere)
	bool bIsChangeWeaponTo;

	UPROPERTY(EditAnywhere)
	float mouseSpeed = 30.0f;

	UPROPERTY(EditAnywhere)
	float PlayerMoveSpeed = 30.0f;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* WeaponStaticMesh;

	// 타입 -> 메쉬 매핑 (에디터에서 채우는 방식 권장)
	UPROPERTY(EditDefaultsOnly)
	TMap<EWeaponType, TObjectPtr<UStaticMesh>> WeaponMeshMap;

protected:
	// 숫자키 입력 액션들(에디터/블루프린트에서 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SWORDAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* GREATSWORDAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* BLUNTAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* KATANAAction = nullptr;

public:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

public:
	EPlayerStates playerState = EPlayerStates::NONE;
	EWeaponType WeaponType = EWeaponType::NONE;

	int32 SwordDamage = 50;
	int32 GreatSwordDamage = 200;
	int32 BluntDamage = 150;
	int32 KatanaDamage = 100;

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

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UTextBlock* WeaponTextBlock;
};
