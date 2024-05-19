// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "TagGameGameMode.h"
#include "Navigation/PathFollowingComponent.h"


void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	GoToPlayer = MakeShared<FAIVState>(
		[](AAIController* AIController) {
			AIController->MoveToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), 100.f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			if (BestBall)
			{
				BestBall->AttachToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BestBall->SetActorRelativeLocation(FVector(0, 0, 0));
				BestBall = nullptr;
			}

			return SearchForBall;
		});

	SearchForBall = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			AGameModeBase* GameMode = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast< ATagGameGameMode>(GameMode);
			const TArray<ABallActor*>& BallList = AIGameMode->GetBalls();

			ABallActor* NearestBall = nullptr;
			for (int32 Index = 0; Index < BallList.Num(); Index++)
			{
				if (!BallList[Index]->GetAttachParentActor() &&
					(!NearestBall ||
						FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallList[Index]->GetActorLocation()) <
						FVector::Distance(AIController->GetPawn()->GetActorLocation(), NearestBall->GetActorLocation())))
				{
					NearestBall = BallList[Index];
				}
			}
			BestBall = NearestBall;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			if (BestBall)
			{
				return GoToBall;
			}
			else
			{
				return SearchForSpawnPoint;
			}
		});

	GoToBall = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			AIController->MoveToActor(BestBall, 100.f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			return GrabBall;
		});

	SearchForSpawnPoint = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			AGameModeBase* GameMode = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast< ATagGameGameMode>(GameMode);
			const TArray<ASpawnTargetPoint*>& SpawnPointerList = AIGameMode->GetEnemySpawnPointers();

			for (int32 Index = 1; Index < SpawnPointerList.Num(); Index++)
			{
				if (!SpawnPointerList[Index]->IsOccupied && SpawnPointerList[Index]->TypeSpawnPoint == Enemy)
				{
					SpawnPointer = SpawnPointerList[Index];
					SpawnPointer->IsOccupied = true;
					return;
				}
			}
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			if (SpawnPointer)
			{
				return GoToSpawnPoint;
			}
			else
			{
				return Wait;
			}
		});

	GoToSpawnPoint = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			AIController->MoveToActor(SpawnPointer, 100.f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			if (SpawnPointer)
			{
				SpawnPointer = nullptr;
			}
			return Wait;
		});

	GrabBall = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			if (BestBall->GetAttachParentActor())
			{
				BestBall = nullptr;
			}
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			if (!BestBall)
			{
				return SearchForBall;
			}

			BestBall->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
			BestBall->SetActorRelativeLocation(FVector(0, 0, 0));
			return GoToPlayer;
		});

	Wait = MakeShared<FAIVState>(
		[this](AAIController* AIController) {
			WaitTime = 1.f;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {
			WaitTime += DeltaTime;
			if (WaitTime >= 1.f)
			{
				return SearchForBall;
			}
			return Wait;
		});

	CurrentState = SearchForBall;
	CurrentState->CallEnter(this);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState = CurrentState->CallTick(this, DeltaTime);
	}
}