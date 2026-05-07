#include "UdpReceiverRunnable.h"
#include "Misc/ScopeLock.h"
#include "HAL/PlatformProcess.h"  // for Sleep

UdpReceiverRunnable::UdpReceiverRunnable()
    : Thread(nullptr), bStopThread(false), Socket(), LatestData(std::nullopt)
{
    // Create and start the thread (Thread name must be unique)
    Thread = FRunnableThread::Create(this, TEXT("UdpReceiverRunnableThread"));
}

UdpReceiverRunnable::~UdpReceiverRunnable()
{
    Stop();

    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
        Thread = nullptr;
    }
}

bool UdpReceiverRunnable::Init()
{
    // Called before Run(). Can do setup here if needed.
    return true;
}

uint32 UdpReceiverRunnable::Run()
{
    // Thread's main loop
    while (!bStopThread)
    {
        // Attempt to receive a new AircraftState packet
        auto Result = Socket.TryGet();  // This comes from your provided UdpSocket

        if (Result.has_value())
        {
            FScopeLock Lock(&DataMutex);  // Make it thread-safe
            LatestData = Result.value();
        }

        // Sleep a bit to prevent CPU hogging (~100Hz loop)
        FPlatformProcess::Sleep(0.01f);
    }

    return 0;
}

void UdpReceiverRunnable::Stop()
{
    bStopThread = true;
}

void UdpReceiverRunnable::Exit()
{
    // Called after Run() exits
}

std::optional<case_study::AircraftState> UdpReceiverRunnable::GetLatestData()
{
    FScopeLock Lock(&DataMutex);
    return LatestData;
}
