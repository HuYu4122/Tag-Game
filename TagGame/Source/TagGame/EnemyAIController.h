// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BallActor.h"
#include "SpawnTargetPoint.h"
#include "EnemyAIController.generated.h"

struct FAIVState : public TSharedFromThis<FAIVState>
{
private:
	TFunction<void(AAIController*)> Enter;
	TFunction<void(AAIController*)> Exit;
	TFunction<TSharedPtr<FAIVState>(AAIController*, const float)> Tick;

public:
	FAIVState()
	{
		Enter = nullptr;
		Exit = nullptr;
		Tick = nullptr;
	}

	FAIVState(
		TFunction<void(AAIController*)> InEnter = nullptr,
		TFunction<void(AAIController*)> InExit = nullptr,
		TFunction<TSharedPtr<FAIVState>(AAIController*, const float)> InTick = nullptr
	)
	{
		Enter = InEnter;
		Exit = InExit;
		Tick = InTick;
	}

	FAIVState(const FAIVState& Other) = delete;
	FAIVState& operator=(const FAIVState& Other) = delete;
	FAIVState& operator=(const FAIVState&& Other) = delete;
	FAIVState(FAIVState&& Other) = delete;

	void CallEnter(AAIController* AIController)
	{
		if (Enter)
		{
			Enter(AIController);
		}
	}

	void CallExit(AAIController* AIController)
	{
		if (Exit)
		{
			Exit(AIController);
		}
	}

	TSharedPtr<FAIVState> CallTick(AAIController* AIController, const float DeltaTime)
	{
		if (Tick)
		{
			TSharedPtr<FAIVState> NewState = Tick(AIController, DeltaTime);

			if (NewState != nullptr && NewState != AsShared())
			{
				CallExit(AIController);
				NewState->CallEnter(AIController);
				return NewState;
			}
		}
		return AsShared();
	}
};

/**
 * 
 */
UCLASS()
class TAGGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	TSharedPtr<FAIVState> CurrentState;
	TSharedPtr<FAIVState> GoToPlayer;
	TSharedPtr<FAIVState> GoToBall;
	TSharedPtr<FAIVState> GoToSpawnPoint;
	TSharedPtr<FAIVState> GrabBall;
	TSharedPtr<FAIVState> SearchForBall;
	TSharedPtr<FAIVState> SearchForSpawnPoint;
	TSharedPtr<FAIVState> Wait;

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	ABallActor* BestBall;
	ASpawnTargetPoint* SpawnPointer;
	float WaitTime;
};
