#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerCharacterBase.generated.h"

UENUM(BlueprintType)//언리얼 리플렉션 시스템에 등록하겠다
enum class EPlayerStates : uint8//enum class 자료형의 EPlayerStates를 1바이트 정수 크기로 저장하여
{
	NONE UMETA(DisplayName = "NONE"),//코드용 이름 NONE/에디터에 표시되는 이름 NONE
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
	//언리얼의 APlayerCharacterBase를 상속받아서 ACharacter라는 새 클래스를 만든다
{
	GENERATED_BODY()//필요한 숨은 코드들 넣어주는 매크로

public://클래스 공개 기능
	APlayerCharacterBase();//생성자

protected://부모와 자식 사이에만 열어둠
	virtual void BeginPlay() override;
	//부모 클래스에 있는 BeginPlay 함수를 이 클래스에서 다시 정의해서 쓰겠다

public:	
	virtual void Tick(float DeltaTime) override;
	//부모 클래스에 있는 Tick(float DeltaTime) 함수를 이 클래스에서 다시 정의해서 쓰겠다

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//부모 클래스에 있는 SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) 함수를 이 클래스에서 다시 정의해서 쓰겠다

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//AActor::TakeDamage를 재정의 — ApplyDamage 호출 시 엔진이 이 함수를 실행해 CurrentHP를 깎는다

private://그 클래스 안에서만 사용 가능
	// 숫자키 액션 바인딩용 콜백(Enhanced Input 시그니처 고정)
	UFUNCTION()//이 함수를 언리얼 리플렉션 시스템에 등록하겠다
	void OnSelectSword(const FInputActionValue& Value);
	//OnSelectSword 함수에 const FInputActionValue& Value라는 매개변수를 넣어서 실행한다.

	UFUNCTION()
	void OnSelectGreatSword(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectBlunt(const FInputActionValue& Value);

	UFUNCTION()
	void OnSelectKatana(const FInputActionValue& Value);

	// 실제 무기 변경 로직 (입력 바인딩에 직접 연결하지 않음)
	void ChangeWeaponTo(EWeaponType NewType);

protected:
	UPROPERTY(EditAnywhere)//이 변수를 어디서든 수정 가능하게끔 리플렉션 시스템에 등록한다.
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	// 속도에 따른 팔 길이 조절을 위한 설정값
	UPROPERTY(EditAnywhere, Category = "Camera")
	//디테일 패널에서 이 변수를 Camera 묶음으로 보여주고 어디서든 수정 가능하게끔 리플렉션 시스템에 등록한다.
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
	void EffectInitialization();

	void Move(const FInputActionValue& Value);
	//Move 함수 안에서 참조로 받는 Enhanced Input이 넘겨주는 입력 데이터와 입력값 Value를 수정하지 않고 처리하는 함수 생성자
	void Look(const FInputActionValue& Value);
	void Attack();
	void AttackTrace();

private:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	//디테일 패널에서 이 변수를 Input 묶음으로 보여주고 어디서든 보기만 가능하게끔 리플렉션 시스템에 등록한다.
	class UInputMappingContext* DefaultContext;
	//Enhanced Input에서 쓰는 입력 설정 포인터 DefaultContext

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AttackAction;

public:
	UPROPERTY(VisibleAnywhere)
	bool bIsChangeWeaponTo;//bool 자료형 변수 bIsChangeWeaponTo

	UPROPERTY(EditAnywhere)
	float mouseSpeed = 30.0f;//30.0의 값을 대입한 실수형 자료형 변수 mouseSpeed

	UPROPERTY(EditAnywhere)
	float PlayerMoveSpeed = 30.0f;//30.0의 값을 대입한 실수형 자료형 변수 PlayerMoveSpeed

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* WeaponSkeletalMesh;
	//무기 스켈레탈 메시 컴포넌트를 가리키는 포인터 변수

	// 타입 -> 메쉬 매핑 (에디터에서 채우는 방식 권장)
	UPROPERTY(EditDefaultsOnly)//클래스의 기본값만 수정할 수 있다
	TMap<EWeaponType, TObjectPtr<USkeletalMesh>> WeaponMeshMap;//맵 자료구조/키/값/변수 이름

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHP = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float CurrentHP = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackSpeed = 1.f; //무기별 공격 속도 배율 (몽타주 재생 속도 + 쿨타임에 적용)

public:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

public:
	EPlayerStates playerState = EPlayerStates::NONE;
	EWeaponType WeaponType = EWeaponType::NONE;

	int32 SwordDamage = 25;
	int32 GreatSwordDamage = 200;
	int32 BluntDamage = 100;
	int32 KatanaDamage = 50;

public:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* AttackSound;

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* FireEffect;
	

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	UUserWidget* HUDWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UImage* Weapon_Icon = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UTextBlock* WeaponTextBlock;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> WeaponCamerashakeClass;
};
