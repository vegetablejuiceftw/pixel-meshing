#include "UtilityTimer.h"


UtilityTimer::UtilityTimer()
{
	tick();
}


UtilityTimer::~UtilityTimer()
{
}

int64 UtilityTimer::unixTimeNow()
{
	FDateTime timeUtc = FDateTime::UtcNow();
	return timeUtc.ToUnixTimestamp() * 1000 + timeUtc.GetMillisecond();
}

void UtilityTimer::tick()
{
	TickTime = unixTimeNow();
}

int32 UtilityTimer::tock()
{
	TockTime = unixTimeNow();
	return TockTime - TickTime;
}

void UtilityTimer::log()
{
	int64 time = tock();
	UE_LOG(LogTemp, Warning, TEXT("Your message %d %d"), time / 1000, time % 1000);
	tick();
}

void UtilityTimer::log(FString text)
{
	int64 time = tock();
	UE_LOG(LogTemp, Warning, TEXT("%s %d %d"), *text, time / 1000, time % 1000);
	tick();
}