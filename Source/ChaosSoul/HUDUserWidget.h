#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUDUserWidget.generated.h"

UCLASS()
class CHAOSSOUL_API UHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* EnemyHPBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PotionQuantity;

protected:
	// 팩트: UI가 생성될 때 초기화를 담당하는 가상 함수 선언을 추가합니다!
	virtual void NativeConstruct() override;

	// 캐릭터가 가진 포션 이벤트에 등록할 함수 선언
	UFUNCTION()
	void UpdatePotionTextBox(int32 NewCurrentPotion);
};
