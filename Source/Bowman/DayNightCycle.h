// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "GameFramework/Actor.h"
#include "GameFramework/Actor.h"
#include "DayNightCycle.generated.h"

UCLASS(ClassGroup = (Lights, DirectionalLights))
class BOWMAN_API ADayNightCycle : public ALight
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Sun)
	class UDirectionalLightComponent* SunLightComp;
	
public:	

	// Sets default values for this actor's properties
	ADayNightCycle(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE class UDirectionalLightComponent* GetDirectionalLight() const { return SunLightComp; }


protected:

	FRotator	m_rSunRotation;
	float		m_fSunPitch;
	float		m_fCycleSpeed;

	bool		m_bisNight;

	int			DegreesHoursConverter(float Input, bool toHours);
	
};
