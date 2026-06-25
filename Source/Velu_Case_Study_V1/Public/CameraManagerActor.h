#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "CameraManagerActor.generated.h"

UENUM()
enum class ECameraType : uint8
{
    ThirdPerson UMETA(DisplayName = "Third Person"),
    Operator    UMETA(DisplayName = "Operator"),
    Gimbal      UMETA(DisplayName = "Gimbal")
};
UCLASS()
class VELU_CASE_STUDY_V1_API ACameraManagerActor : public AActor
{
    GENERATED_BODY()

public:
    ACameraManagerActor();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Switch active camera by index: 0 = ThirdPerson, 1 = Operator, 2 = Gimbal
    void SetCameraIndex(int32 Index);

protected:
    // Reference to the aircraft pawn to follow or track
    UPROPERTY(EditAnywhere, Category = "CameraManager")
    AActor* AircraftPawn;

    // Reference to UDP receiver for telemetry data
    UPROPERTY(EditAnywhere, Category = "CameraManager")
    class AUdpReceiverActor* UdpReceiverRef;

    // Cameras
    UPROPERTY(VisibleAnywhere, Category = "CameraManager")
    UCameraComponent* ThirdPersonCamera;

    UPROPERTY(VisibleAnywhere, Category = "CameraManager")
    UCameraComponent* OperatorCamera;

    UPROPERTY(VisibleAnywhere, Category = "CameraManager")
    UCameraComponent* GimbalCamera;

private:
    int32 ActiveCameraIndex;
    FVector OperatorCameraFixedPosition; 
    bool bHasInitializedOperatorPosition;
    const TCHAR* GetCameraName(ECameraType Type) const;
    void UpdateActiveCamera();
};


