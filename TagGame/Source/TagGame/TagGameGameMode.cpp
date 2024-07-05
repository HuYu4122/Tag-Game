// Copyright Epic Games, Inc. All Rights Reserved.

#include "TagGameGameMode.h"
#include "TagGameCharacter.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ATagGameGameMode::ATagGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PrimaryActorTick.bCanEverTick = true;
}

void ATagGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	ResetMatch();
}

void ATagGameGameMode::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (int32 Index = 0; Index < BallsActors.Num(); Index++)
	{
		if (BallsActors[Index]->GetAttachParentActor() == GetWorld()->GetFirstPlayerController()->GetAttachParentActor())
		{
			return;
		}
	}

	GamePhase = EGamePhase::WaitPhase;
	if (GamePhase == EGamePhase::WaitPhase)
	{
		bool CanReset = true;
		for (int32 Index = 0; Index < SpawnPointers.Num(); Index++)
		{
			if (SpawnPointers[Index]->TypeSpawnPoint == ESpawnPoint::Enemy && !SpawnPointers[Index]->IsOccupied)
			{
				CanReset = false;
			}
		}
		if (CanReset)
		{
			ResetMatch();
		}
	}
}

void ATagGameGameMode::ResetMatch()
{
	GamePhase = EGamePhase::PlayPhase;

	SpawnPointers.Empty();
	for (TActorIterator<ASpawnTargetPoint> It(GetWorld()); It; ++It)
	{
		SpawnPointers.Add(*It);
	}

	TArray<ASpawnTargetPoint*> RandomTargetPoints;
	for (int32 Index = 0; Index < SpawnPointers.Num(); Index++)
	{
		if (SpawnPointers[Index]->TypeSpawnPoint == Enemy)
		{
			SpawnPointers[Index]->IsOccupied = false;
		}
		if (SpawnPointers[Index]->TypeSpawnPoint == Ball)
		{
			RandomTargetPoints.Add(SpawnPointers[Index]);
		}
	}

	BallsActors.Empty();
	for (TActorIterator<ABallActor> It(GetWorld()); It; ++It)
	{
		if (It->GetAttachParentActor())
		{
			It->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//It->AttachToActor(nullptr, FAttachmentTransformRules::KeepWorldTransform);
		}
		BallsActors.Add(*It);
	}

	for (int32 Index = 0; Index < BallsActors.Num(); Index++)
	{
		const int32 RandomIndex = FMath::RandRange(0, RandomTargetPoints.Num() - 1);
		BallsActors[Index]->SetActorLocation(RandomTargetPoints[RandomIndex]->GetActorLocation());
		RandomTargetPoints.RemoveAt(RandomIndex);
	}
}

const TArray<ABallActor*> ATagGameGameMode::GetBalls() const
{
	return BallsActors;
}

const TArray<ASpawnTargetPoint*> ATagGameGameMode::GetEnemySpawnPointers() const
{
	TArray<ASpawnTargetPoint*> EnemySpawn;
	for (int32 Index = 0; Index < SpawnPointers.Num(); Index++)
	{
		if (SpawnPointers[Index]->TypeSpawnPoint == Enemy)
		{
			EnemySpawn.Add(SpawnPointers[Index]);
		}
	}
	return EnemySpawn;
}