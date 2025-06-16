#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "aircraft_state.hpp"
#include "udp_socket.hpp"
#include "UdpReceiverActor.generated.h"

UCLASS()
class VELU_CASE_STUDY_V1_API AUdpReceiverActor : public AActor
{
    GENERATED_BODY()

public:
    AUdpReceiverActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void CheckForNewData();
    // ? Getter for AircraftData
    const case_study::AircraftState& GetAircraftData() const;

private:
    TUniquePtr<qs::libs::UdpSocket> Socket;
    case_study::AircraftState AircraftData;
};
