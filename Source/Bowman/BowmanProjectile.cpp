// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Bowman.h"
#include "BowmanProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GeneralEngineSettings.h"

ABowmanProjectile::ABowmanProjectile(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ABowmanProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	//FireLight
	FireLightComp = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "FireLightComp");
	FireLightComp->SetIntensity(256.0f);
	FireLightComp->LightColor = FColor::FColor(255, 128, 0);
	FireLightComp->SetVisibility(false, true);
	FireLightComp->AttachTo(RootComponent);

	//FireParticle
	FireParticleComp = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, "FireParticleComp");
	FireParticleComp->bAutoActivate = false;
	FireParticleComp->AttachTo(RootComponent);
	FireParticleComp->SetWorldScale3D(FVector::FVector(1.0f, 1.0f, 1.0f));
	FireParticleComp->SetWorldRotation(FRotator::FRotator(90,0,0));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticleFlyingAsset(TEXT("/Game/StarterContent/Particles/FireArrow_Flying.FireArrow_Flying"));
	if (FireParticleFlyingAsset.Succeeded())
		FireParticleComp->SetTemplate(FireParticleFlyingAsset.Object);


	// Die after 3 seconds by default
	InitialLifeSpan = 15.0f;
}

void ABowmanProjectile::SetFireArrow()
{
	FireLightComp->SetVisibility(true, true);
	FireParticleComp->Activate();
}

void ABowmanProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		//Remove collision and movement then attach to other actor
		ProjectileMovement->DestroyComponent(true);
		CollisionComp->DestroyComponent(true);
		AttachRootComponentToActor(OtherActor, NAME_None, EAttachLocation::KeepWorldPosition);	
	}
}

void ABowmanProjectile::SetVelocity(const FVector& velocity)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = velocity;
	}
}

void ABowmanProjectile::SetSpeed(const float& speed)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = speed;
	}
}

void ABowmanProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}