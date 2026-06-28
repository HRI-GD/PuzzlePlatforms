// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
	public:
	
		AMovingPlatform();

		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;

		UPROPERTY(EditAnywhere, Category = "Moving Platform")
		float PlatformSpeed = 10.0f;
		UPROPERTY(EditAnywhere, Category = "Moving Platform", meta = (MakeEditWidget = true))
		FVector TargetLocation;
		UPROPERTY(EditAnywhere, Category = "Moving Platform")
		bool bShouldReturn = true;

		void AddActiveTrigger();
		void RemoveActiveTrigger();

	private:
		FVector GlobalStartLocation;
		FVector GlobalTargetLocation;
		bool bHasReachedTarget = false;

		UPROPERTY(EditAnywhere, Category = "Moving Platform")
		int32 RequiredActiveTriggers = 1;
		UPROPERTY(EditAnywhere, Category = "Moving Platform")
		bool bTriggerablePlatform = false;
		int32 ActiveTriggers = 0;
};
