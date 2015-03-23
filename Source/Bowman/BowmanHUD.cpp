// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Bowman.h"
#include "BowmanHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"



ABowmanHUD::ABowmanHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> WeaponTexObj(TEXT("/Game/FirstPerson/Textures/BowHudText"));
	WeaponTex = WeaponTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> AmmoRegularTexObj(TEXT("/Game/FirstPerson/Textures/HudArrowRegularTex"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> AmmoFlameTexObj(TEXT("/Game/FirstPerson/Textures/HudArrowFlameTex"));
	AmmoTex = AmmoFlameTexObj.Object;
}


void ABowmanHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5f)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	//Draw the Weapon Hud Item
	const FVector2D WeaponDrawPosition(Canvas->ClipX * 0.9f - WeaponTex->GetSurfaceWidth() * 0.5f, Canvas->ClipY * 0.9f - WeaponTex->GetSurfaceHeight() * 0.5f);

	FCanvasTileItem WeaponTileItem(WeaponDrawPosition, WeaponTex->Resource, FLinearColor::White);
	WeaponTileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(WeaponTileItem);

	//Draw the Ammo Hud Item
	const FVector2D AmmoDrawPosition(Canvas->ClipX * 0.95f - AmmoTex->GetSurfaceWidth() * 0.5f, Canvas->ClipY * 0.9f - AmmoTex->GetSurfaceHeight() * 0.5f);

	FCanvasTileItem AmmoTileItem(AmmoDrawPosition, AmmoTex->Resource, FLinearColor::White);
	AmmoTileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(AmmoTileItem);
}