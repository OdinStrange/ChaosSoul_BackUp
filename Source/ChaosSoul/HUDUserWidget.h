#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
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
};
