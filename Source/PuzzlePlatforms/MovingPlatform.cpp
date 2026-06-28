// Fill out your copyright notice in the Description page of Project Settings.
// 이동 플랫폼
// 기본적으로 플랫폼은 움직이는 상태지만 bTriggerablePlatform이 true인 경우 정지 상태 (BeginPlay에서 SetActorTickEnabled(false))
// 트리거가 활성화되면 플랫폼이 움직임 (틱 활성화)
// 트리거가 비활성화되면 플랫폼이 정지 (틱 비활성화)

#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if(this->bTriggerablePlatform)
    {
        this->ActiveTriggers = 0;
        SetActorTickEnabled(false);
    }

    if(HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }

    this->GlobalStartLocation = GetActorLocation(); // GetActorLocation은 이미 글로벌 좌표
    this->GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation); // TargetLocation은 로컬 좌표기 때문에 글로벌 좌표로 변환
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    if(HasAuthority())
    {
        if(bHasReachedTarget)
        {
            return;
        }

        FVector CurrentLocation = GetActorLocation();

        FVector NextLocation = FMath::VInterpConstantTo(
            CurrentLocation,
            GlobalTargetLocation,
            DeltaTime,
            this->PlatformSpeed
        );

        SetActorLocation(NextLocation);

        float Distance = FVector::Dist(NextLocation, this->GlobalTargetLocation);
        if(Distance <= 0.1f)
        {
            SetActorLocation(GlobalTargetLocation);
            if(bShouldReturn)
            {
                Swap(GlobalStartLocation, GlobalTargetLocation);
            }
            else
            {
                bHasReachedTarget = true;
            }
        }
    }
}

void AMovingPlatform::AddActiveTrigger()
{
    ActiveTriggers++;

    if(ActiveTriggers >= RequiredActiveTriggers)
    {
        SetActorTickEnabled(true);
    }
}

void AMovingPlatform::RemoveActiveTrigger()
{
    ActiveTriggers--;

    if (ActiveTriggers <= 0)
    {
        ActiveTriggers = 0;
    }

    if(ActiveTriggers < RequiredActiveTriggers)
    {
        SetActorTickEnabled(false);
    }
}