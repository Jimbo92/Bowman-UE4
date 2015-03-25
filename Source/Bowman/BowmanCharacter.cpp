// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Bowman.h"
#include "BowmanCharacter.h"
#include "BowmanProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ABowmanCharacter

ABowmanCharacter::ABowmanCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	//init Vars
	m_bisFireArrow = false;
	m_bBowDrawing = false;
	m_fArrowSpeed = 0.0f;
	m_bisLanternOn = false;
	m_fLanternIntensity = 1500.0f;

	WeaponIndex = 0;

	CharacterMovement->NavAgentProps.bCanCrouch = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	//Create Lantern light
	LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("LanternLightComp"));
	LightComp->SetLightColor(FColor::FColor(255, 145, 0));
	LightComp->AttachParent = FirstPersonCameraComponent;
	LightComp->SetIntensity(m_fLanternIntensity);
	LightComp->SetVisibility(false, true);



	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABowmanCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	//Player Movement
	InputComponent->BindAxis("MoveForward", this, &ABowmanCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABowmanCharacter::MoveRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ABowmanCharacter::Crouching);
	InputComponent->BindAction("Crouch", IE_Released, this, &ABowmanCharacter::StopCrouching);

	//Player Attack
	InputComponent->BindAction("Primary Attack", IE_Pressed, this, &ABowmanCharacter::PrimaryAttack);
	InputComponent->BindAction("Primary Attack", IE_Released, this, &ABowmanCharacter::PrimaryAttackOnRelease);
	InputComponent->BindAction("Secondary Attack", IE_Pressed, this, &ABowmanCharacter::SecondaryAttack);
	InputComponent->BindAction("Secondary Attack", IE_Released, this, &ABowmanCharacter::SecondaryAttackOnRelease);

	//Player Weapon Slots
	InputComponent->BindAction("Equipt Primary", IE_Pressed, this, &ABowmanCharacter::PrimaryWeapon);
	InputComponent->BindAction("Equipt Secondary", IE_Pressed, this, &ABowmanCharacter::SecondaryWeapon);


	InputComponent->BindAction("ArrowSwitch", IE_Pressed, this, &ABowmanCharacter::ArrowToggle);

	InputComponent->BindAction("LanternToggle", IE_Pressed, this, &ABowmanCharacter::Lantern);



	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ABowmanCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ABowmanCharacter::LookUpAtRate);

	InputComponent->BindAction("QuitGame", IE_Released, this, &ABowmanCharacter::QuickQuit);
}

void ABowmanCharacter::PrimaryWeapon()
{
	WeaponIndex = 0;
}
void ABowmanCharacter::SecondaryWeapon()
{
	WeaponIndex = 1;
}

//Primary Attack Function
void ABowmanCharacter::PrimaryAttack()
{
	switch (WeaponIndex)
	{
	case 0:
	{
		BowDrawBack();
	}
		break;
	case 1:
	{
		SwordSwing();
	}
		break;


	default:
		break;
	}
}
//PA On Release
void ABowmanCharacter::PrimaryAttackOnRelease()
{
	switch (WeaponIndex)
	{
	case 0:
	{
		OnFire();
	}
		break;
	case 1:
	{

	}
		break;


	default:
		break;
	}
}
//Secondary Attack Function
void ABowmanCharacter::SecondaryAttack()
{

}
//SA On Release
void ABowmanCharacter::SecondaryAttackOnRelease()
{

}

void ABowmanCharacter::SwordSwing()
{

}

void ABowmanCharacter::SwordBlock()
{

}

void ABowmanCharacter::Crouching()
{
	if (ACharacter::CharacterMovement)
	{
		ACharacter::Crouch();
	}
}

void ABowmanCharacter::StopCrouching()
{
	if (ACharacter::CharacterMovement)
	{
		ACharacter::UnCrouch();
	}
}

void ABowmanCharacter::ArrowToggle()
{
	if (!m_bisFireArrow)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::FString("Fire Arrows"));
		m_bisFireArrow = true;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::FString("Regular Arrows"));
		m_bisFireArrow = false;
	}
}

void ABowmanCharacter::Lantern()
{
	//Toggle Lantern
	if (!m_bisLanternOn)
	{
		m_bisLanternOn = true;
	}
	else
	{
		m_bisLanternOn = false;
	}
}

void ABowmanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Lantern Light Intensity Tick
	if (m_bisLanternOn && m_fLanternIntensity < 1500.0f)
	{
		if (m_fLanternIntensity > 0.0f)
		{
			LightComp->SetVisibility(true, true);
			LightComp->SetActive(true, true);
		}

		m_fLanternIntensity += 5000.0f * DeltaTime;

		LightComp->SetIntensity(m_fLanternIntensity);
	}
	else if (!m_bisLanternOn && m_fLanternIntensity > 0.0f)
	{
		if (m_fLanternIntensity <= 0.0f)
		{
			LightComp->SetVisibility(false, true);
			LightComp->SetActive(false, true);
		}

		m_fLanternIntensity -= 4500.0f * DeltaTime;

		LightComp->SetIntensity(m_fLanternIntensity);
	}

	//Bow DrawBack Tick
	if (m_bBowDrawing && m_fArrowSpeed < 4100.0f)
	{
		m_fArrowSpeed += 2500.0f * DeltaTime;

		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, FString::SanitizeFloat(m_fArrowSpeed));
	}
}

void ABowmanCharacter::BowDrawBack()
{
	m_fArrowSpeed = 0;
	m_bBowDrawing = true;
}

void ABowmanCharacter::OnFire()
{
	//Release
	m_bBowDrawing = false;


	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();

		UWorld* const World = GetWorld();
		if (World != NULL)
		{

			ABowmanProjectile* Arrow = World->SpawnActor<ABowmanProjectile>(ProjectileClass, Mesh1P->GetSocketLocation("Muzzle"), SpawnRotation);

			if (Arrow)
			{
				FVector velocity = Arrow->GetVelocity() * m_fArrowSpeed;

				Arrow->SetVelocity(velocity);

				if (m_bisFireArrow)
					Arrow->SetFireArrow();
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void ABowmanCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ABowmanCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ABowmanCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ABowmanCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABowmanCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ABowmanCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABowmanCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ABowmanCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ABowmanCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ABowmanCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ABowmanCharacter::TouchUpdate);
	}

	return bResult;
}

void ABowmanCharacter::QuickQuit()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}
