#include "UdpTestActor.h"
#include "UdpReceiverRunnable.h"
#include "Engine/Engine.h"

AUdpTestActor::AUdpTestActor()
{
    PrimaryActorTick.bCanEverTick = true;
    UdpReceiver = nullptr;
}

void AUdpTestActor::BeginPlay()
{
    Super::BeginPlay();

    // Start the UDP receiver thread
    UdpReceiver = new UdpReceiverRunnable();
}

void AUdpTestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (UdpReceiver)
    {
        auto Data = UdpReceiver->GetLatestData();
        if (Data.has_value())
        {
            const auto& State = Data.value();

            FString DebugText = FString::Printf(
                TEXT("Aircraft Pos: Lat=%.6f, Lon=%.6f, Alt=%.2f | Att: Yaw=%.2f°, Pitch=%.2f°, Roll=%.2f°"),
                State.aircraft_latitude__deg,
                State.aircraft_longitude__deg,
                State.aircraft_altitude__m_amsl,
                FMath::RadiansToDegrees(State.aircraft_psi__rad),
                FMath::RadiansToDegrees(State.aircraft_theta__rad),
                FMath::RadiansToDegrees(State.aircraft_phi__rad)
            );

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, DebugText);
            }
        }
    }
}

void AUdpTestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UdpReceiver)
    {
        delete UdpReceiver;
        UdpReceiver = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}
