// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;
    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

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