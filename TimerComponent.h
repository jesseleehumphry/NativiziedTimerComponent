// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"

#include "TimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimerReset);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPEGAME_API UTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void PrepareTimer(TArray<AActor*> TargetActors);

	void TimerCheckIn(AActor* ActorCheckingIn);

	void TimerSignalToTargets(bool InvertBehavior);

	UPROPERTY(BlueprintAssignable)
	FTimerReset OnResetTimer;
	
	// VARIABLES
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	int32 TimerLength = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	int32 WarningTime = 4;


private:

	void SetDefaults();

	void SetMapEmpty();

	void SetupSoundCue();

	void EndTickFadeIn();

	void TickingFadeOut();

	void RunTimer();

	void TimerResetEvent();
	
	bool AllActorsAreCheckedIn();

	// VARIABLES

	FTimerHandle TriggerEndTickFadeIn;

	FTimerHandle TriggerTickingFadeout;
	
	bool bSoundCompHasInit = false;

	bool bTimerHasRun = false;

	FName TickEndVol = FName(TEXT("TickEndVol"));
	
	UPROPERTY()
	TMap<AActor*, bool> CheckInMap;

	UPROPERTY()
	UAudioComponent* TickSoundComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer", meta=(AllowPrivateAccess="true"))
	USoundCue* TickingSoundCue;

	
		
};
