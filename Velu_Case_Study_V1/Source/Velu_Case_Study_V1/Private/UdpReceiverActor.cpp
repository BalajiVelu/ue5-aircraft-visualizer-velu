#include "UdpReceiverActor.h"
#include "udp_socket.hpp"

AUdpReceiverActor::AUdpReceiverActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AUdpReceiverActor::BeginPlay()
{
    Super::BeginPlay();

    Socket = MakeUnique<qs::libs::UdpSocket>();

    std::string error;
    if (!Socket->initSocket(error))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize UDP socket: %s"), *FString(error.c_str()));
        Socket.Reset();
        return;
    }

    if (!Socket->setUdpListenPort(6506, error))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to bind UDP socket to port 6506: %s"), *FString(error.c_str()));
        Socket.Reset();
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("UDP socket initialized and bound to port 6506"));
}

void AUdpReceiverActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CheckForNewData();
}

void AUdpReceiverActor::CheckForNewData()
{
    if (!Socket)
        return;

    constexpr int bufferSize = 1024;  // Adjust size as needed
    char buffer[bufferSize] = {};

    int bytesReceived = Socket->readData(buffer, bufferSize);
    if (bytesReceived > 0)
    {
        // Assuming AircraftState is POD and matches the data format exactly
        if (bytesReceived == sizeof(AircraftData))
        {
            memcpy(&AircraftData, buffer, sizeof(AircraftData));

            UE_LOG(LogTemp, Log, TEXT("UDP Data: lat=%.6f lon=%.6f alt=%.2f"),
                AircraftData.aircraft_latitude__deg,
                AircraftData.aircraft_longitude__deg,
                AircraftData.aircraft_altitude__m_amsl);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Received UDP data size mismatch: expected %d, got %d"), sizeof(AircraftData), bytesReceived);
        }
    }
    else if (bytesReceived == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK)
        {
            UE_LOG(LogTemp, Warning, TEXT("UDP receive error: %d"), err);
        }
        // else no data available, no log needed
    }
}
// ? Getter method implementation
const case_study::AircraftState& AUdpReceiverActor::GetAircraftData() const
{
    return AircraftData;
}