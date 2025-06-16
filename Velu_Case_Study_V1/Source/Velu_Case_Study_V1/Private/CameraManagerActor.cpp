#include "CameraManagerActor.h"
#include "EngineUtils.h"         // For TActorIterator
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

ACameraManagerActor::ACameraManagerActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root Component for this actor
    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootComp;

    // Third Person Camera
    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(RootComponent);
    ThirdPersonCamera->bAutoActivate = true; // Start active by default
    // Position behind aircraft (relative to pawn, but we will update later)
    ThirdPersonCamera->SetRelativeLocation(FVector(-500.f, 0.f, 150.f));
    ThirdPersonCamera->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));

    // Operator Camera (static location)
    OperatorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OperatorCamera"));
    OperatorCamera->SetupAttachment(RootComponent);
    OperatorCamera->bAutoActivate = false;
    OperatorCamera->SetRelativeLocation(FVector(0.f, -1000.f, 170.f)); // 10m south, 1.7m height approx.
    OperatorCamera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    // Gimbal Camera (attached to aircraft, rotation controlled)
    GimbalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GimbalCamera"));
    GimbalCamera->SetupAttachment(RootComponent);
    GimbalCamera->bAutoActivate = false;
    GimbalCamera->SetRelativeLocation(FVector(0.f, 0.f, 100.f)); // Slightly above aircraft center
    GimbalCamera->SetRelativeRotation(FRotator::ZeroRotator);

    ActiveCameraIndex = 0;
}

void ACameraManagerActor::BeginPlay()
{
    Super::BeginPlay();

    // Find AircraftPawn in the scene by tag "AircraftPawn"
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        if (It->ActorHasTag(FName("AircraftPawn")))
        {
            AircraftPawn = *It;
            break;
        }
    }

    if (!AircraftPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraManagerActor: Could not find AircraftPawn with tag 'AircraftPawn'!"));
    }

    UpdateActiveCamera();
}

void ACameraManagerActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!AircraftPawn)
        return;

    switch (ActiveCameraIndex)
    {
    case 0: // Third Person Camera follows aircraft
    {
        // Follow behind aircraft with offset (relative to aircraft rotation)
        FVector TargetLocation = AircraftPawn->GetActorLocation();
        FRotator TargetRotation = AircraftPawn->GetActorRotation();

        // Calculate offset: 5m behind and 1.5m above aircraft
        FVector Offset = TargetRotation.RotateVector(FVector(-500.f, 0.f, 150.f));
        ThirdPersonCamera->SetWorldLocation(TargetLocation + Offset);
        ThirdPersonCamera->SetWorldRotation((TargetLocation - (TargetLocation + Offset)).Rotation() + FRotator(-30.f, 0.f, 0.f));
        break;
    }
    case 1: // Operator Camera is static - update position only if AircraftPawn moves origin
    {
        // Position the OperatorCamera relative to aircraft's start position
        // Assume OperatorCamera is 10m south (-Y) and 1.7m high above starting point

        // Get aircraft start location (for now use current AircraftPawn location)
        FVector AircraftLocation = AircraftPawn->GetActorLocation();

        FVector OperatorLocation = AircraftLocation + FVector(0.f, -1000.f, 170.f);
        OperatorCamera->SetWorldLocation(OperatorLocation);

        // Look at aircraft
        FRotator LookAtRotation = (AircraftLocation - OperatorLocation).Rotation();
        OperatorCamera->SetWorldRotation(LookAtRotation);
        break;
    }
    case 2: // Gimbal Camera linked to aircraft rotation from AircraftState
    {
        // Attach GimbalCamera to aircraft location
        FVector AircraftLocation = AircraftPawn->GetActorLocation();
        GimbalCamera->SetWorldLocation(AircraftLocation + FVector(0.f, 0.f, 100.f));

        // For rotation, assume AircraftPawn has the rotation info stored in tags or components
        // Or if you have access to aircraft state, get gimbal_phi_theta_psi here.
        // For demo, just match aircraft rotation directly:
        FRotator AircraftRotation = AircraftPawn->GetActorRotation();
        GimbalCamera->SetWorldRotation(AircraftRotation);
        break;
    }
    }
}

void ACameraManagerActor::SetCameraIndex(int32 Index)
{
    if (Index < 0 || Index > 2)
        return;

    ActiveCameraIndex = Index;
    // ?? Debug Output to Log
    UE_LOG(LogTemp, Warning, TEXT("CameraManagerActor: Switched to Camera %d"), Index);

    // ??? On-Screen Debug Message
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.f, FColor::Yellow,
            FString::Printf(TEXT("Switched to Camera %d"), Index));
    }
    UpdateActiveCamera();
    // Smooth view transition
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->SetViewTargetWithBlend(this, 0.5f);
    }
}

void ACameraManagerActor::UpdateActiveCamera()
{
    ThirdPersonCamera->SetActive(ActiveCameraIndex == 0);
    OperatorCamera->SetActive(ActiveCameraIndex == 1);
    GimbalCamera->SetActive(ActiveCameraIndex == 2);
}
