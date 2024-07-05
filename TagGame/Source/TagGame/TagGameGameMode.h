// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BallActor.h"
#include "SpawnTargetPoint.h"
#include "GameFramework/GameModeBase.h"
#include "TagGameGameMode.generated.h"

UENUM(BlueprintType)
enum EGamePhase
{
	WaitPhase UMETA(DisplayName = "Ball"),
	PlayPhase UMETA(DisplayName = "Enemy"),
};

UCLASS(minimalapi)
class ATagGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	//TArray<ATargetPoint*> TargetPoints;
	TArray<ABallActor*> BallsActors;
	TArray<ASpawnTargetPoint*> SpawnPointers;

	void ResetMatch();

public:
	ATagGameGameMode();
	void BeginPlay() override;
	void Tick(const float DeltaTime) override;

	const TArray<ABallActor*> GetBalls() const;
	const TArray<ASpawnTargetPoint*> GetEnemySpawnPointers() const;
	EGamePhase GamePhase;
};



