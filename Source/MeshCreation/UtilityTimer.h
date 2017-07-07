// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MESHCREATION_API UtilityTimer
{

public:
	UtilityTimer();
	~UtilityTimer();

	int64 unixTimeNow();
	void tick();
	int32 tock();

	void log();
	void log(FString text);

protected:
	int64 TickTime = 0;
	int64 TockTime = 0;
};

