#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OperatorCameraActor.generated.h"

UCLASS()
class VELU_CASE_STUDY_V1_API AOperatorCameraActor : public AActor
{
    GENERATED_BODY()

public:
    AOperatorCameraActor();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* TargetActor;

private:
    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* CameraComponent;
};