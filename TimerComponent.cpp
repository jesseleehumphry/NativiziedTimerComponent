// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AudioMixerBlueprintLibrary.h"
#include "EscapeGame/Library/Interfaces/TimerInterface.h"


// Sets default values for this component's properties
UTimerComponent::UTimerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTimerComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UTimerComponent::PrepareTimer(TArray<AActor*> TargetActors)
{
	if (!TargetActors[0])   // Guard clause
		{
		return;
		}

	for (AActor* actor : TargetActors)
	{
		CheckInMap.Add(actor, false);
	}

	SetupSoundCue();
	
}

void UTimerComponent::SetupSoundCue()
{

	// UAudioMixerBlueprintLibrary::PrimeSoundCueForPlayback(TickingSoundCue); // Commented out to avoid linking error. 

	if (!bSoundCompHasInit)
	{
		TickSoundComp = UGameplayStatics::SpawnSound2D(GetOwner(), TickingSoundCue, 0.f, 1.f, 0.f, nullptr, false, false);
		TickSoundComp->Stop();
	}

	bSoundCompHasInit = true;
	
}

void UTimerComponent::EndTickFadeIn()
{
	TickSoundComp->SetFloatParameter(TickEndVol, FMath::FInterpTo(0.f, 1.f, GetWorld()->GetDeltaSeconds(), 50.f));
}

void UTimerComponent::TickingFadeOut()
{
	TickSoundComp->FadeOut(0.2, 0.f, EAudioFaderCurve::Linear);
	TickSoundComp->Stop();
	
	GetWorld()->GetTimerManager().ClearTimer(TriggerEndTickFadeIn);
	TimerResetEvent();
}

void UTimerComponent::SetMapEmpty()
{
	TArray<AActor*> ActorKeys;
	CheckInMap.GetKeys(ActorKeys);

	if (!ActorKeys[0])
	{
		return; // if array is empty
	}
	
	for (AActor* actor : ActorKeys)
	{
		CheckInMap.Add(actor, false);
	}
}

void UTimerComponent::TimerCheckIn(AActor* ActorCheckingIn)
{

	CheckInMap.Add(ActorCheckingIn, true);

	if (AllActorsAreCheckedIn())
	{
		RunTimer();
	}
	
}

void UTimerComponent::TimerSignalToTargets(bool InvertBehavior)
{

	if (InvertBehavior)
	{
		TimerResetEvent();
	} else
	{
		TArray<AActor*> Keys;
		CheckInMap.GetKeys(Keys);
		for (AActor* actor : Keys)
		{
			ITimerInterface* InterfaceActor = Cast<ITimerInterface>(actor);

			if (InterfaceActor)
			{
				InterfaceActor->TimerStart(this);
			}
		}
	}
}

void UTimerComponent::RunTimer()
{
	if (!bTimerHasRun && TickSoundComp)
	{
		TickSoundComp->SetFloatParameter(TickEndVol, 0.1f);
		TickSoundComp->SetVolumeMultiplier(1.f);
		TickSoundComp->Play(0.f);
	}

	GetWorld()->GetTimerManager().SetTimer(TriggerEndTickFadeIn, this, &UTimerComponent::EndTickFadeIn, GetWorld()->GetDeltaSeconds(), true, float (TimerLength - WarningTime));
	GetWorld()->GetTimerManager().SetTimer(TriggerTickingFadeout, this, &UTimerComponent::TickingFadeOut, 0.f, false, float (TimerLength));

	bTimerHasRun = true;
}

void UTimerComponent::TimerResetEvent()
{
	OnResetTimer.Broadcast();
	
	TArray<AActor*> Keys;
	CheckInMap.GetKeys(Keys);
	
	for (AActor* actor : Keys)
	{
		ITimerInterface* InterfaceActor = Cast<ITimerInterface>(actor);

		if (InterfaceActor)
		{
			InterfaceActor->TimerEnd();
		}
	}

	if (TickSoundComp->IsPlaying())
	{
		TickSoundComp->FadeOut(0.2f, 0.0, EAudioFaderCurve::Linear);
		TickSoundComp->Stop();
	}
	
	bTimerHasRun = false;
	
}

bool UTimerComponent::AllActorsAreCheckedIn()
{
	TArray<bool> vals;
	CheckInMap.GenerateValueArray(vals);

	return vals.Contains(false);
}
