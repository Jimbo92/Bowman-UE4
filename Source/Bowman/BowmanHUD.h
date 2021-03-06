// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "BowmanHUD.generated.h"

UCLASS()
class ABowmanHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABowmanHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	class UTexture2D* BowWeaponTex;
	class UTexture2D* SwordWeaponTex;

 	class UTexture2D* RegularArrowTex;
	class UTexture2D* FlameArrowTex;

	class UTexture2D* TempWeaponTex;
	class UTexture2D* TempAmmoTex;

};

