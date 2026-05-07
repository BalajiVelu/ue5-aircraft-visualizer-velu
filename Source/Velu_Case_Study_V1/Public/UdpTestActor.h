#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UdpTestActor.generated.h"

class UdpReceiverRunnable;

UCLASS()
class VELU_CASE_STUDY_V1_API AUdpTestActor : public AActor
{
    GENERATED_BODY()

public:
    AUdpTestActor();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

private:
    UdpReceiverRunnable* UdpReceiver;
};
