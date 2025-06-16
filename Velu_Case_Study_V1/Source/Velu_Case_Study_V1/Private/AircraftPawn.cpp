#include "AircraftPawn.h"
#include "UdpReceiverActor.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

AAircraftPawn::AAircraftPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube"));
    if (Cube.Succeeded())
    {
        Mesh->SetStaticMesh(Cube.Object);
        RootComponent = Mesh;
    }

    // Spring Arm for chase camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.0f; // 5 meters
    SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f)); // 30 degrees down
    SpringArm->bUsePawnControlRotation = false;

    // Chase camera (third person)
    ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
    ChaseCamera->SetupAttachment(SpringArm);

    // Gimbal camera
    GimbalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GimbalCamera"));
    GimbalCamera->SetupAttachment(RootComponent);
    GimbalCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // adjust height if needed
}

void AAircraftPawn::BeginPlay()
{
    Super::BeginPlay();
    if (!UdpReceiverActorRef) return;

    AUdpReceiverActor* Receiver = Cast<AUdpReceiverActor>(UdpReceiverActorRef);
    if (!Receiver) return;

    const auto& Aircraft = Receiver->GetAircraftData();

    float StartAlt = Aircraft.startposition_altitude__m_amsl;

    UE_LOG(LogTemp, Warning, TEXT("Aircraft Start Altitude (m): %f (Z: %f cm)"), StartAlt, StartAlt * 100.0f);
}

void AAircraftPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!UdpReceiverActorRef) return;

    AUdpReceiverActor* Receiver = Cast<AUdpReceiverActor>(UdpReceiverActorRef);
    if (!Receiver) return;

    const auto& Aircraft = Receiver->GetAircraftData();

    // Update aircraft position
    FVector NewLocation;
    NewLocation.X = (Aircraft.aircraft_latitude__deg - 48.0) * 10000.0f;
    NewLocation.Y = (Aircraft.aircraft_longitude__deg - 11.0) * 10000.0f;
    NewLocation.Z = Aircraft.aircraft_altitude__m_amsl;
    SetActorLocation(NewLocation);

    // Update aircraft rotation
    FRotator NewRotation;
    NewRotation.Roll = FMath::RadiansToDegrees(Aircraft.aircraft_phi__rad);
    NewRotation.Pitch = FMath::RadiansToDegrees(Aircraft.aircraft_theta__rad);
    NewRotation.Yaw = FMath::RadiansToDegrees(Aircraft.aircraft_psi__rad);
    SetActorRotation(NewRotation);

    // Update gimbal camera rotation
    if (GimbalCamera)
    {
        FRotator GimbalRotation;
        GimbalRotation.Roll = FMath::RadiansToDegrees(Aircraft.gimbal_phi__rad);
        GimbalRotation.Pitch = FMath::RadiansToDegrees(Aircraft.gimbal_theta__rad);
        GimbalRotation.Yaw = FMath::RadiansToDegrees(Aircraft.gimbal_psi__rad);

        GimbalCamera->SetWorldRotation(GimbalRotation);
    }
}
