#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/CriticalSection.h"
#include <optional>
#include "udp_socket.hpp"
#include "aircraft_state.hpp"

class UdpReceiverRunnable : public FRunnable
{
public:
    UdpReceiverRunnable();
    virtual ~UdpReceiverRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    std::optional<case_study::AircraftState> GetLatestData();

private:
    FRunnableThread* Thread;
    bool bStopThread;

    qs::libs::UdpSocket Socket;

    FCriticalSection DataMutex;
    std::optional<case_study::AircraftState> LatestData;
};
