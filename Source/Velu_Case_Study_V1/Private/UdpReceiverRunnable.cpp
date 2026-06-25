#include "UdpReceiverRunnable.h"
#include "Misc/ScopeLock.h"
#include "HAL/PlatformProcess.h"  // for Sleep

UdpReceiverRunnable::UdpReceiverRunnable()
    : Thread(nullptr), bStopThread(false), Socket(), LatestData(std::nullopt)
{
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
    return true;
}

uint32 UdpReceiverRunnable::Run()
{
    while (!bStopThread)
    {
        auto Result = Socket.TryGet();  

        if (Result.has_value())
        {
            FScopeLock Lock(&DataMutex);  
            LatestData = Result.value();
        }

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
}

std::optional<case_study::AircraftState> UdpReceiverRunnable::GetLatestData()
{
    FScopeLock Lock(&DataMutex);
    return LatestData;
}
