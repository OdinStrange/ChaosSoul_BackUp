// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosSoul_AnimInstance.h"
#include "GameFrameWork/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UChaosSoul_AnimInstance::UChaosSoul_AnimInstance()
{

}

void UChaosSoul_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UChaosSoul_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		MoveSpeed = Velocity.Size2D();
		isFalling = Movement->IsFalling();

	}
}
