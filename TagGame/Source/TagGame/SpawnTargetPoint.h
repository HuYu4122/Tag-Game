// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SpawnTargetPoint.generated.h"

UENUM(BlueprintType)
enum ESpawnPoint
{
	Ball UMETA(DisplayName = "Ball"),
	Enemy UMETA(DisplayName = "Enemy"),
};

/**
 * 
 */
UCLASS()
class TAGGAME_API ASpawnTargetPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ESpawnPoint> TypeSpawnPoint;
	bool IsOccupied = false;
};
