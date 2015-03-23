// Fill out your copyright notice in the Description page of Project Settings.

#include "Bowman.h"
#include "DayNightCycle.h"

//Set Console Vars
static TAutoConsoleVariable<float> CVarDayNightCycleSpeed(TEXT("TOD.DayNightCycleSpeed"), 1.5f, TEXT("Sets Day/Night Cycle Speed"), ECVF_Default);
static TAutoConsoleVariable<int> CVarSetTOD(TEXT("TOD.SetTime"), 0, TEXT("Sets Time of day Range 0-16"), ECVF_Default);
int TODCurrent = CVarSetTOD.GetValueOnRenderThread();


// Sets default values
ADayNightCycle::ADayNightCycle(const FObjectInitializer& ObjectInitializer) : ALight(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_fSunPitch = CVarSetTOD.GetValueOnGameThread();
	m_bisNight = false;

	SunLightComp = ObjectInitializer.CreateDefaultSubobject<UDirectionalLightComponent>(this, "SunLightComp");
	SunLightComp->bAutoActivate = true;
	SunLightComp->bUsedAsAtmosphereSunLight = true;
	SunLightComp->SetVisibility(true, true);
	//SunLightComp->SetEnableLightShaftBloom(true);
	//SunLightComp->SetEnableLightShaftOcclusion(true);

	

	//Set Root Comp
	RootComponent = SunLightComp;

}

// Called when the game starts or when spawned
void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADayNightCycle::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//Check Console command TOD
	int TODChecker = CVarSetTOD.GetValueOnRenderThread();
	if (TODCurrent != TODChecker)
	{
		m_fSunPitch = DegreesHoursConverter((float)TODChecker, false);

		TODCurrent = TODChecker;
	}

	if (m_fCycleSpeed != CVarDayNightCycleSpeed.GetValueOnGameThread())
		m_fCycleSpeed = CVarDayNightCycleSpeed.GetValueOnGameThread();

	//Toggle night or day
	if (m_fSunPitch >= 360)
	{
		m_fSunPitch = 0.0f;
	}
	else if (m_fSunPitch > 180 && m_fSunPitch < 360)
	{
		m_bisNight = false;
	}
	else if (m_fSunPitch > 0 && m_fSunPitch < 180)
	{
		m_bisNight = true;
	}

	//Set Intensity
	if (m_bisNight)
	{
		SunLightComp->SetVisibility(false, true);
	}
	else
	{
		SunLightComp->SetVisibility(true, true);
	}

	m_fSunPitch += m_fCycleSpeed * DeltaTime;

	m_rSunRotation = FRotator::FRotator(m_fSunPitch, 0, 0);

	SunLightComp->SetRelativeRotation(m_rSunRotation);

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::SanitizeFloat(DegreesHoursConverter(m_fSunPitch, true)));
}

//Convert the degrees to Hours and vice versa
int ADayNightCycle::DegreesHoursConverter(float Input, bool toHours)
{

	if (toHours)
	{
		return (Input / 24);
	}
	else
	{
		return (Input * 24);
	}

	return 0;
}
