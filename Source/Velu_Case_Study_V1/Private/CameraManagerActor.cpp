#include "CameraManagerActor.h"
#include "EngineUtils.h"         // For TActorIterator
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneComponent.h"
#include "UdpReceiverActor.h"
#include "GeoCoordinateUtils.h"

namespace
{
    // 3rd Person: 
    const FVector ThirdPersonOffsetCm = FVector(-960.f, 0.f, 580.f);
    constexpr float   ThirdPersonPitchDeg = -30.f;

    // Operator: 
    constexpr float OperatorSouthOffsetCm = 1000.f;
    constexpr float OperatorHeightCm      = 170.f;

    // Gimbal: 
    const FVector GimbalMountOffsetCm = FVector(80.f, 0.f, -120.f);

    constexpr float   GimbalPitchDeg  = -85.f;
    constexpr float   GimbalFovDeg  = 155.f;
}

ACameraManagerActor::ACameraManagerActor()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootComp;

    // --- Third Person Camera ---
    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(RootComponent);
    ThirdPersonCamera->bAutoActivate = true;

    // --- Operator Camera ---
    OperatorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OperatorCamera"));
    OperatorCamera->SetupAttachment(RootComponent);
    OperatorCamera->bAutoActivate = false;

    // --- Gimbal Camera  ---
    GimbalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GimbalCamera"));
    GimbalCamera->SetupAttachment(RootComponent);
    GimbalCamera->bAutoActivate = false;
    GimbalCamera->SetFieldOfView(GimbalFovDeg);

    ActiveCameraIndex = 0;
    bHasInitializedOperatorPosition = false;
}

void ACameraManagerActor::BeginPlay()
{
    Super::BeginPlay();

    // Find AircraftPawn
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

    // Find UdpReceiverActor
    for (TActorIterator<AUdpReceiverActor> It(GetWorld()); It; ++It)
    {
        UdpReceiverRef = *It;
        break;
    }
    if (!UdpReceiverRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraManagerActor: Could not find UdpReceiverActor!"));
    }

    // --- Initialize operator camera fixed position ---
    
    if (AircraftPawn)
    {
        const FVector StartLocation = AircraftPawn->GetActorLocation();

        // ENU convention
        
        OperatorCameraFixedPosition = FVector(
            StartLocation.X,
            StartLocation.Y - OperatorSouthOffsetCm,
            OperatorHeightCm
        );
        bHasInitializedOperatorPosition = true;

        UE_LOG(LogTemp, Log, TEXT("Operator camera fixed at (%.0f, %.0f, %.0f)"),
            OperatorCameraFixedPosition.X,
            OperatorCameraFixedPosition.Y,
            OperatorCameraFixedPosition.Z);
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
    case 0: // Third Person — offset 
    {
        const FVector  AircraftLocation = AircraftPawn->GetActorLocation();
        const FRotator AircraftRotation = AircraftPawn->GetActorRotation();

        // Position: body-frame offset rotated into world space
        const FVector WorldOffset = AircraftRotation.RotateVector(ThirdPersonOffsetCm);
        ThirdPersonCamera->SetWorldLocation(AircraftLocation + WorldOffset);

        // Rotation: aircraft attitude composed with -30° local pitch
        const FQuat WorldQuat =
            AircraftRotation.Quaternion() *
            FRotator(ThirdPersonPitchDeg, 0.f, 0.f).Quaternion();
        ThirdPersonCamera->SetWorldRotation(WorldQuat);
        break;
    }

    case 1: // Operator — fixed on ground, tracks aircraft
    {
        if (!bHasInitializedOperatorPosition)
            break;

        OperatorCamera->SetWorldLocation(OperatorCameraFixedPosition);

        const FVector  AircraftLocation = AircraftPawn->GetActorLocation();
        const FRotator LookAtRotation   = (AircraftLocation - OperatorCameraFixedPosition).Rotation();
        OperatorCamera->SetWorldRotation(LookAtRotation);
        break;
    }

    case 2: // Gimbal — offset (80, 0, -120), -85° pitch in body frame, 155° FOV
    {
        const FVector  AircraftLocation = AircraftPawn->GetActorLocation();
        const FRotator AircraftRotation = AircraftPawn->GetActorRotation();

        // Position: mounted slightly forward and below the fuselage
        const FVector GimbalWorldOffset = AircraftRotation.RotateVector(GimbalMountOffsetCm);
        GimbalCamera->SetWorldLocation(AircraftLocation + GimbalWorldOffset);

        // Rotation: aircraft attitude composed with -85° local pitch (down-look)
        const FQuat WorldQuat =
            AircraftRotation.Quaternion() *
            FRotator(GimbalPitchDeg, 0.f, 0.f).Quaternion();
        GimbalCamera->SetWorldRotation(WorldQuat);
        break;
    }

    default:
        break;
    }
}

void ACameraManagerActor::SetCameraIndex(int32 Index)
{
    if (Index < 0 || Index > 2)
        return;

    ActiveCameraIndex = Index;

    const TCHAR* CameraName = GetCameraName(static_cast<ECameraType>(Index));

    UE_LOG(LogTemp, Log, TEXT("CameraManagerActor: Switched to %s"), CameraName);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.f, FColor::Yellow,
            FString::Printf(TEXT("Switched to %s Camera"), CameraName));
    }

    UpdateActiveCamera();

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

const TCHAR* ACameraManagerActor::GetCameraName(ECameraType Type) const
{
    switch (Type)
    {
        case ECameraType::ThirdPerson: return TEXT("Third Person");
        case ECameraType::Operator:    return TEXT("Operator");
        case ECameraType::Gimbal:      return TEXT("Gimbal");
        default:                       return TEXT("Unknown");
    }
}