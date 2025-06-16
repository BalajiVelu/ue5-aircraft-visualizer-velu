#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AircraftPawn.generated.h"

UCLASS()
class VELU_CASE_STUDY_V1_API AAircraftPawn : public AActor
{
    GENERATED_BODY()

public:
    AAircraftPawn();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP")
    AActor* UdpReceiverActorRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* ChaseCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* GimbalCamera;
};
