// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDUserWidget.h"
#include "Player/PlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// UI가 생성되거나 초기화될 때 호출되는 함수를 오버라이드하여 연동합니다.
void UHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 1. 플레이어 캐릭터를 가져옵니다.
	APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		// 2. 처음 UI가 켜졌을 때 현재 포션 개수로 초기화
		// (캐릭터에 GetCurrentPotion() 같은 Getter 함수를 만들거나 변수를 Public/BlueprintReadOnly로 두어 가져옵니다)

		// 3. 캐릭터의 포션 변경 이벤트와 UI의 갱신 함수를 연결(바인딩)합니다.
		PlayerCharacter->OnPotionChanged.AddDynamic(this, &UHUDUserWidget::UpdatePotionTextBox);
	}
}

// 캐릭터에서 Broadcast를 하면 이 함수가 자동으로 실행됩니다.
void UHUDUserWidget::UpdatePotionTextBox(int32 NewCurrentPotion)
{
	if (PotionQuantity)
	{
		// TextBlock의 텍스트를 숫자로 변경 (예: "3")
		PotionQuantity->SetText(FText::AsNumber(NewCurrentPotion));

		// 만약 "3 / 5" 형태로 표현하고 싶다면 아래처럼 가공할 수 있습니다.
		// FText PotionText = FText::Format(FText::FromString("{0} / 5"), FText::AsNumber(NewCurrentPotion));
		// PotionQuantity->SetText(PotionText);
	}
}

