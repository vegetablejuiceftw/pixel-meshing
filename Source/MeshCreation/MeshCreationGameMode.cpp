// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MeshCreationGameMode.h"
#include "MeshCreationCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameUserSettings.h"

AMeshCreationGameMode::AMeshCreationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	UE_LOG(LogTemp, Warning, TEXT("starto gameo"));
	/*UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->SetFullscreenMode(EWindowMode::Fullscreen);
	Settings->SetVSyncEnabled(false);
	Settings->SetPostProcessingQuality(0);
	Settings->SetShadowQuality(0);
	Settings->SetAntiAliasingQuality(0);
	Settings->SetFoliageQuality(0);
	Settings->SetTextureQuality(0);
	Settings->SetVisualEffectQuality(0);
	Settings->ApplySettings(true);*/
}
